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

#include "gaincludes.h"
#include "models.h"
#include "mvint.h"
#include "object.h"
#include "state.h"
#include "util.h"

int mvInt::interpret(const p3ga::mv &X, int creationFlags /* = 0*/)
{
    p3ga::mv I(p3ga::e1<double>() ^ p3ga::e2<double>() ^ p3ga::e3<double>());
    p3ga::mv e0(p3ga::e0<double>());

    const GAIM_FLOAT epsilon = 1e-6; // rather arbitrary limit on fp-noise
    p3ga::mv Xd, tmp;

    initToNothing();

    m_type = 0;
    m_type = MVI_P3GA;

    // todo: dualize?

    // check for zero euclidean norm multivectors
    if (ga::norm_a(X) < epsilon)
    { // *********************** zero blade *******************************
        m_type |= MVI_ZERO;
        m_valid = 1;
        return 0;
    }

    // determine the type of the multivectors
    int grade, type = ga::mvType(X, &grade);
    grade = (int)(log((double)grade) / log((double)2) + 0.45);
    if (creationFlags & OC_BLADE)
        type = ga::mvTypeBlade; // force blade interpretation
    else if (creationFlags & OC_VERSOR)
        type = ga::mvTypeVersor; // force versor interpretation
    // maybe allow force null, dual, and other properties?

    if (type == ga::mvTypeMultivector)
    {
        m_type |= MVI_UNKNOWN;
        m_valid = 0;
        return 0;
    }

    if (type == ga::mvTypeBlade)
    { // ************************************* all blades *************************************
        p3ga::mv ipe0X(ga::lcont(e0, X));
        p3ga::mv ori, pos;
        GAIM_FLOAT size2;
        int ipe0Xzero = (ga::norm_a(ipe0X) < epsilon);

        if (ipe0Xzero)
        { // ************************************* Euclidean subspaces (scalar, vector, bivector, trivector)
          // *************************************
            ori = X;
            size2 = ga::scalar(ga::lcont(ori, ori));
            // cprintf("\n\nEuclidean subspace, grade %d\nOri: %s\nSize2: %f\n", grade, ori.string(), size2);
            return p3gaEuclideanSubspace(X, grade, ori, size2);
        }
        else
        { //  ************************************* homogeneous subspaces (point, line, plane, space)
          //  *************************************
            ori = ga::lcont(e0, X);
            size2 = ga::scalar(ga::lcont(ori, ori));
            pos = ga::lcont(ori, ga::inverse(X));
            // cprintf("\n\nHomogeneous subspace, grade %d\nOri: %s\nPos: %s\nSize2: %f\n", grade, ori.string(),
            // pos.string(), size2);
            return p3gaHomogeneousSubspace(X, grade, ori, pos, size2);
        }
    }
    // todo: versor & such
    else
    {
        m_type |= MVI_UNKNOWN;
        m_valid = 0;
    }

    return 0;
}

int mvInt::p3gaEuclideanSubspace(const p3ga::mv &X, int grade, const p3ga::mv &ori, GAIM_FLOAT size2)
{
    m_valid = 1;
    p3ga::mv normal, o, pe1, pe2, rt;

    switch (grade)
    {
    case 4:
        m_valid = 0;
        m_type |= MVI_UNKNOWN;
        // cprintf("mvInt::p3gaEuclideanSubspace(): grade %d Euclidean subspaces do not exist\n", grade);
        return 0;
    case 0: // scalar
        m_type |= MVI_SCALAR;
        /*
        scalar 0: magnitude
        */
        m_scalar[0] = ga::scalar(ori);
        return 0;
    case 1: // vector
        m_type |= MVI_VECTOR;
        /*
        scalar 0: magnitude
        vector 0: direction
        point 0: position of tip
        */
        m_scalar[0] = sqrt(fabs(size2));
        o = ori * (1.0 / m_scalar[0]);
        m_vector[0][0] = o[p3ga::E1];
        m_vector[0][1] = o[p3ga::E2];
        m_vector[0][2] = o[p3ga::E3];

        m_point[0][0] = ori[p3ga::E1];
        m_point[0][1] = ori[p3ga::E2];
        m_point[0][2] = ori[p3ga::E3];
        return 0;

    case 2: // bivector
        m_type |= MVI_BIVECTOR;
        /*
        scalar 0: magnitude
        vector 0: normal
        vector 1: orthogonal to vector 0 and 2
        vector 2: orthogonal to vector 0 and 1
        */
        m_scalar[0] = sqrt(fabs(size2));
        p3gaFactorBivector(o = ori * (1.0 / m_scalar[0]), m_vector[0], m_vector[1], m_vector[2]);
        return 0;
    case 3: // trivector
        m_type |= MVI_TRIVECTOR;
        /*
        scalar 0: magnitude
        */
        m_scalar[0] = -ori[p3ga::E123];
        return 0;
    }
    return 0;
}

int mvInt::p3gaHomogeneousSubspace(const p3ga::mv &X, int grade, const p3ga::mv &ori, const p3ga::mv &pos, GAIM_FLOAT size2)
{
    m_valid = 1;
    p3ga::mv normal, o, pe1, pe2, rt;

    /*
    For all types:
    point 0: point
    */
    if (pos[p3ga::E0] != 0.0)
    {
        m_point[0][0] = pos[p3ga::E1] / pos[p3ga::E0];
        m_point[0][1] = pos[p3ga::E2] / pos[p3ga::E0];
        m_point[0][2] = pos[p3ga::E3] / pos[p3ga::E0];
    }

    switch (grade)
    {
    case 0:
        m_valid = 0;
        m_type |= MVI_UNKNOWN;
        // cprintf("mvInt::p3gaHomogeneousSubspace(): grade %d homogeneous subspaces do not exist\n", grade);
        return 0;
    case 1: // points
        m_type |= MVI_POINT;
        /*
        scalar 0: magnitude
        point 0: point
        */
        m_scalar[0] = X[p3ga::E0];
        return 0;
    case 2: // line
        m_type |= MVI_LINE;
        /*
        scalar 0: magnitude
        point 0: point
        vector 0: direction
        */
        m_scalar[0] = sqrt(fabs(size2));
        o = ori * (1.0 / m_scalar[0]);
        m_vector[0][0] = o[p3ga::E1];
        m_vector[0][1] = o[p3ga::E2];
        m_vector[0][2] = o[p3ga::E3];
        return 0;
    case 3: // plane
        m_type |= MVI_PLANE;
        /*
        scalar 0: magnitude
        vector 0: normal
        vector 1: orthogonal to vector 0 and 2
        vector 2: orthogonal to vector 0 and 1
        */
        m_scalar[0] = sqrt(fabs(size2));
        p3gaFactorBivector(o = ori * (1.0 / m_scalar[0]), m_vector[0], m_vector[1], m_vector[2]);
        return 0;
    case 4: // space
        m_type |= MVI_SPACE;
        /*
        scalar 0: magnitude
        */
        m_scalar[0] = ori[p3ga::E1230];
        return 0;
    }
    return 0;
}

int mvInt::p3gaFactorBivector(const p3ga::mv &B, GAIM_FLOAT *normal, GAIM_FLOAT *factor1, GAIM_FLOAT *factor2)
{
    // get normal
    GAIM_FLOAT scale;
    p3ga::mv nl(ga::normalize(
        ga::lcont(B, ga::inverse(p3ga::mv(p3ga::e1<double>() ^ p3ga::e2<double>() ^ p3ga::e3<double>())))));
    p3ga::mv pe1, pe2, rt;

    // get normal
    if (normal)
    {
        normal[0] = nl[p3ga::E1];
        normal[1] = nl[p3ga::E2];
        normal[2] = nl[p3ga::E3];
    }

    // set factor 1 and 2 (orthogonal to normal)
    if ((ga::norm_a(g_state->m_p3gaFactor[0]) > 0.0) && // user factorization
        (ga::norm_a(g_state->m_p3gaFactor[1]) > 0.0))
    {
        pe1 = ga::lcont(ga::lcont(g_state->m_p3gaFactor[0], B), ga::inverse(B));
        pe2 = ga::lcont(ga::lcont(g_state->m_p3gaFactor[1], B), ga::inverse(B));
    }
    else
    { // auto factorization
        p3gaRve3(rt, nl);
        pe1 = ga::takeGrade(p3ga::mv(rt * p3ga::e1<double>() * ga::inverse(rt)), GRADE1);
        pe2 = ga::takeGrade(p3ga::mv(rt * p3ga::e2<double>() * ga::inverse(rt)), GRADE1);
    }

    // if the bivector wasn't unit, adjust factors
    scale = ga::scalar(ga::igp(B, p3ga::mv(pe1 ^ pe2)));
    pe1 *= ((scale < 0.0) ? -1.0 : 1.0) * sqrt(fabs(scale));
    pe2 *= sqrt(fabs(scale));

    // set factors
    if (factor1)
    {
        factor1[0] = pe1[p3ga::E1];
        factor1[1] = pe1[p3ga::E2];
        factor1[2] = pe1[p3ga::E3];
    }
    if (factor2)
    {
        factor2[0] = pe2[p3ga::E1];
        factor2[1] = pe2[p3ga::E2];
        factor2[2] = pe2[p3ga::E3];
    }

    return 0;
}
