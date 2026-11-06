// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

// Copyright 2002-2009, Daniel Fontijne, University of Amsterdam -- fontijne@science.uva.nl

#include <cmath>
#include <cstring>

#include "gaincludes.h"
#include "models.h"
#include "mvint.h"
#include "object.h"
#include "state.h"
#include "util.h"

int mvInt::interpret(const e3ga::mv &amv, int creationFlags /* = 0*/)
{
    e3ga::mv mv(amv);
    const GAIM_FLOAT epsilon = 10e-8, epsilon2 = epsilon * epsilon;
    e3ga::mv tmp, rt, pe1, pe2, pe3;

    // sets m_vector[i] to the vector v
    auto setVector = [this](int i, const e3ga::mv &v) {
        m_vector[i][0] = v[e3ga::E1];
        m_vector[i][1] = v[e3ga::E2];
        m_vector[i][2] = v[e3ga::E3];
    };
    GAIM_FLOAT scale;

    if (creationFlags == 0)
        creationFlags = m_creationFlags;
    m_creationFlags = creationFlags;

    m_type = MVI_E3GA;

    int grade, type = ga::mvType(mv, &grade), allGrades;

    if (m_creationFlags & OC_VERSOR)
        type = ga::mvTypeVersor; // force versor interpretation
    if (m_creationFlags & OC_BLADE)
        type = ga::mvTypeBlade; // force blade interpretation

    allGrades = 0X1FF; // OC_GRADE0 | ... | OC_GRADE8
    if (m_creationFlags & allGrades)
    { // force grade X interpretation
        grade = m_creationFlags & allGrades;
        mv = ga::takeGrade(mv, grade);
    }

    if (type == ga::mvTypeBlade)
    {
        switch (grade)
        {
        case GRADE0: // scalar
            m_type |= MVI_SCALAR;
            /*
            scalar 0: magnitude
            */
            m_scalar[0] = ga::scalar(mv);
            break;
        case GRADE1:
            m_type |= MVI_VECTOR;
            /*
            scalar 0: magnitude
            vector 0: direction
            */
            m_scalar[0] = sqrt(ga::norm_a(mv));

            if (fabs(m_scalar[0]) > 0.0)
            {
                m_vector[0][0] = mv[e3ga::E1] / m_scalar[0];
                m_vector[0][1] = mv[e3ga::E2] / m_scalar[0];
                m_vector[0][2] = mv[e3ga::E3] / m_scalar[0];
            }
            else
            {
                m_vector[0][0] = m_vector[0][1] = m_vector[0][2] = 0.0;
            }
            m_point[0][0] = m_vector[0][0] * m_scalar[0];
            m_point[0][1] = m_vector[0][1] * m_scalar[0];
            m_point[0][2] = m_vector[0][2] * m_scalar[0];
            break;
        case GRADE2:
            m_type |= MVI_BIVECTOR;
            /*
            scalar 0: magnitude
            vector 0: normal
            vector 1: orthogonal to vector 0 and 2
            vector 2: orthogonal to vector 0 and 1
            */
            m_scalar[0] = sqrt(ga::norm_a(mv));

            // set vector 0 (normal)
            tmp = ga::dual(mv) * (1.0 / m_scalar[0]);
            setVector(0, tmp);

            // set vector 1 and 2 (orthogonal to normal)
            if ((ga::norm_a(g_state->m_e3gaFactor[0]) > 0.0) && // user factorization
                (ga::norm_a(g_state->m_e3gaFactor[1]) > 0.0))
            {
                pe1 = ga::lcont(ga::lcont(g_state->m_e3gaFactor[0], mv), ga::inverse(mv));
                pe2 = ga::lcont(ga::lcont(g_state->m_e3gaFactor[1], mv), ga::inverse(mv));
                scale = ga::scalar(ga::igp(mv, e3ga::mv(pe1 ^ pe2)));
                // mv.print("target: ");
                //(pe1 ^ pe2).print("1: ");
                pe1 *= ((scale < 0.0) ? -1.0 : 1.0) * sqrt(fabs(scale));
                pe2 *= sqrt(fabs(scale));
                //(pe1 ^ pe2).print("2: ");
            }
            else
            { // auto factorization
                e3gaRve3(rt, tmp);
                pe1 = ga::takeGrade(e3ga::mv(rt * e3ga::e1<double>() * ga::inverse(rt)), GRADE1);
                pe2 = ga::takeGrade(e3ga::mv(rt * e3ga::e2<double>() * ga::inverse(rt)), GRADE1);
            }
            setVector(1, pe1);
            setVector(2, pe2);
            break;
        case GRADE3: // pseudoscalar / trivector
            m_type |= MVI_TRIVECTOR;

            /*
            scalar 0: magnitude
            */

            m_scalar[0] = mv[e3ga::E123];

            // factor into vector 0, 1, 2
            if ((scale = ga::norm_a(e3ga::mv(g_state->m_e3gaFactor[0] ^ g_state->m_e3gaFactor[1] ^
                                             g_state->m_e3gaFactor[2]))) > 0.0)
            {
                // factor using the presupplied factors
                pe1 = g_state->m_e3gaFactor[0];
                pe2 = g_state->m_e3gaFactor[1];
                pe3 = g_state->m_e3gaFactor[2];
                scale = pow(sqrt(scale), 1.0 / 3.0);
            }
            else
            {
                // factor using the basis vectors
                pe1 = e3ga::e1<double>();
                pe2 = e3ga::e2<double>();
                pe3 = e3ga::e3<double>();
                scale = 1.0;
            }

            // printf("Dividing by scale %f\n", scale);

            // scale the factors
            pe1 *= 1.0 / scale;
            pe2 *= 1.0 / scale;
            pe3 *= 1.0 / scale;

            // set the factors
            setVector(0, pe1);
            setVector(1, pe2);
            setVector(2, pe3);

            break;
        }
    }
    else if (type == ga::mvTypeVersor)
    {
        GAIM_FLOAT odd = ga::gradeNorm_a(mv, 1) + ga::gradeNorm_a(mv, 3);
        GAIM_FLOAT even = ga::gradeNorm_a(mv, 0) + ga::gradeNorm_a(mv, 2);
        if (odd < even)
        { // even 3D versor = rotor
            m_type |= MVI_ROTOR;
            /*
            scalar 0: magnitude
            scalar 1: angle
            vector 0: normal
            vector 1: orthogonal to vector 0 and 2
            vector 2: orthogonal to vector 0 and 1
            point 0: position (todo for conformal?)
            */
            even = sqrt(even);
            m_scalar[0] = even;

            // interpret the bivector part to retrieve plane
            e3ga::mv plane(m_scalar[0] * ga::normalize(ga::takeGrade(mv, GRADE2)));
            mvInt bivectorInt(plane, OC_BLADE | OC_GRADE2);
            memcpy(m_vector[0], bivectorInt.m_vector[0], sizeof(GAIM_FLOAT) * 3);
            memcpy(m_vector[1], bivectorInt.m_vector[1], sizeof(GAIM_FLOAT) * 3);
            memcpy(m_vector[2], bivectorInt.m_vector[2], sizeof(GAIM_FLOAT) * 3);

            // retrieve the angle
            e3ga::mv Rn(mv * (1.0 / even));
            GAIM_FLOAT ca = ga::scalar(Rn);
            GAIM_FLOAT sa = sqrt(ga::gradeNorm_a(Rn, 2));
            if ((sa < epsilon) || (sin(acos(ca)) < epsilon))
                m_scalar[1] = 0; // angle = 0
            else
                m_scalar[1] = 2.0 * atan2(sa, ca);
            // cprintf("Angle = %f\n", m_scalar[1]);
        }
        else
        { // odd 3D versor
            m_type |= MVI_ODD_3D_VERSOR;
        }
    }
    else
    {
        m_type |= MVI_UNKNOWN;
        m_valid = 0;
        return 0;
    }

    m_valid = 1;
    return 0;
}
