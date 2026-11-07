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

// #include <stdio.h>
#include <cmath>

#include "gaincludes.h"
#include "models.h"
#include "mvint.h"
#include "object.h"
#include "state.h"
#include "util.h"

int mvInt::interpret(const c3ga::mv &X, int creationFlags /* = 0*/)
{
    c3ga::mv I(c3ga::e1<double>() ^ c3ga::e2<double>() ^ c3ga::e3<double>());
    c3ga::mv no(c3ga::eo<double>()), ni(c3ga::ei<double>());
    c3ga::mv nino(ni ^ no), noni(no ^ ni);

    const double epsilon = 1e-6; // rather arbitrary limit on fp-noise
    c3ga::mv Xd, tmp;

    initToNothing();

    m_type = 0;
    m_type = MVI_C3GA;

    // todo: dualize?

    // check for zero euclidean norm multivectors
    if (ga::norm_a(X) < epsilon)
    { // *********************** zero blade *******************************
        m_type |= MVI_ZERO;
        m_valid = 1;
        return 0; /*c3gaZero(X); todo */
    }

    // determine the type of the multivectors
    int grade, type = ga::mvType(X, &grade, epsilon); // temp todo test epsilon

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

        return 0; /* c3gaUnknown(X); todo */
    }

    if (type == ga::mvTypeBlade)
    { // ************************************* all blades *************************************
        double X2 = ga::scalar(ga::gp(X, X)), weight2;
        c3ga::mv opniX(ni ^ X), ipniX(ga::lcont(ni, X));
        c3ga::mv attitude, pos;

        int X2zero = (fabs(X2) < epsilon);
        int opniXzero = (ga::norm_a(opniX) < epsilon);
        int ipniXzero = (ga::norm_a(ipniX) < epsilon);

        if (opniXzero && ipniXzero)
        { // *********************** free flat *******************************
            attitude = -ga::rcont(X, no);
            weight2 = ga::scalar(ga::lcont(attitude, attitude));
            return c3gaFreeFlat(X, grade, attitude, weight2);
        }
        else if (opniXzero && (!ipniXzero))
        { // *********************** bound flat *******************************
            attitude = ga::lcont(noni, X);
            pos = ga::lcont(ga::lcont(noni, X), ga::inverse(X)); // reverse is also good?
            weight2 = ga::scalar(ga::lcont(attitude, attitude));

            return c3gaBoundFlat(X, grade, attitude, pos, weight2);
        }
        else if ((!opniXzero) && ipniXzero)
        { // *********************** dual bound flat *******************************
            attitude = ga::lcont(nino, ga::dual(X));
            // pos = (noni ^ X) * X.inverse();
            pos = ga::lcont(ga::inverse(X), c3ga::mv(noni ^ X));
            weight2 = ga::scalar(ga::lcont(attitude, attitude));

            m_type ^= MVI_DUAL;
            return c3gaBoundFlat(ga::dual(X), 5 - grade, attitude, pos, weight2);
        }
        else if ((!opniXzero) && (!ipniXzero) && X2zero)
        {                                // *********************** bound tangent *******************************
                                         //			attitude = ni << X; // (this is what the paper says, but it is wrong...?)
            attitude = ga::lcont(nino, c3ga::mv(ni ^ X)); // (this one works)
            pos = ga::lcont(ga::inverse(ga::rcont(X, ni)), X);
            weight2 = ga::scalar(ga::lcont(attitude, attitude));

            return c3gaBoundTangent(X, grade, attitude, pos, weight2);
        }
        else if ((!opniXzero) && (!ipniXzero) && (!X2zero))
        { // *********************** (dual or non-dual) round *******************************
            c3ga::mv niX(ni ^ X);
            double niX2 = ga::scalar(ga::gp(niX, niX));
            //			attitude = no << (ni ^ X);
            attitude = ga::lcont(nino, c3ga::mv(ni ^ X)); // this is what you want...
            pos = (X * ni * X) * (1.0 / (2.0 * niX2));
            weight2 = ((grade & 1) ? -1.0 : 1.0) * ga::scalar(ga::lcont(X, X)) / niX2;

            return c3gaRound(X, grade, attitude, pos, weight2);
        }
        else
        {
            m_type |= MVI_UNKNOWN;
            m_valid = 0;
        }
    }
    // todo: versors & such
    else
    {
        m_type |= MVI_UNKNOWN;
        m_valid = 0;
    }

    return 0;
}

int mvInt::c3gaFreeFlat(const c3ga::mv &X, int grade, const c3ga::mv &attitude, double weight2)
{
    m_valid = 1;
    c3ga::mv normal, o, pe1, pe2, rt;

    switch (grade)
    {
    case 0:
    case 5:
        m_valid = 0;
        m_type |= MVI_UNKNOWN;
        cprintf("mvInt::c3gaFreeFlat(): grade %d free flats do not exist\n", grade);
        return 0;
    case 1: // free scalar
        m_type |= MVI_FREE_SCALAR;
        /*
        scalar 0: weight
        */
        m_scalar[0] = ga::scalar(attitude);
        return 0;
    case 2: // free vector
        m_type |= MVI_FREE_VECTOR;
        /*
        scalar 0: weight
        vector 0: direction
        */
        m_scalar[0] = sqrt(fabs(weight2));
        o = attitude * (1.0 / m_scalar[0]);
        m_vector[0][0] = o[c3ga::E1];
        m_vector[0][1] = o[c3ga::E2];
        m_vector[0][2] = o[c3ga::E3];
        return 0;

    case 3: // free bivector
        m_type |= MVI_FREE_BIVECTOR;
        /*
        scalar 0: weight
        vector 0: normal
        vector 1: orthogonal to vector 0 and 2
        vector 2: orthogonal to vector 0 and 1
        */
        m_scalar[0] = sqrt(fabs(weight2));
        c3gaFactorBivector(o = attitude * (1.0 / m_scalar[0]), m_vector[0], m_vector[1], m_vector[2]);
        return 0;
    case 4: // free trivector
        m_type |= MVI_FREE_TRIVECTOR;
        /*
        scalar 0: weight

        */
        m_scalar[0] = attitude[c3ga::E123];
        return 0;
    }

    return 0;
}

int mvInt::c3gaBoundFlat(const c3ga::mv &X, int grade, const c3ga::mv &attitude, const c3ga::mv &pos, double weight2)
{
    c3ga::mv o;

    /*
    For all types:
    point 0: point
    */
    if (pos[c3ga::Eoi] != 0.0)
    {
        m_point[0][0] = pos[c3ga::E1i] / pos[c3ga::Eoi];
        m_point[0][1] = pos[c3ga::E2i] / pos[c3ga::Eoi];
        m_point[0][2] = pos[c3ga::E3i] / pos[c3ga::Eoi];
    }

    switch (grade)
    {
    case 0:
    case 1:
        m_valid = 0;
        m_type |= MVI_UNKNOWN;
        cprintf("mvInt::c3gaBoundFlat(): grade %d bound flats do not exist\n", grade);
        return 0;
    case 2: // flat point
        m_type |= MVI_FLAT_POINT;
        /*
        scalar 0: weight
        point 0: point
        */
        m_scalar[0] = sqrt(fabs(weight2));
        m_valid = 1;
        return 0;
    case 3: // line
        m_type |= MVI_LINE;
        /*
        scalar 0: weight
        point 0: point
        vector 0: direction
        */
        m_scalar[0] = sqrt(fabs(weight2));
        m_vector[0][0] = -attitude[c3ga::E1] / m_scalar[0];
        m_vector[0][1] = -attitude[c3ga::E2] / m_scalar[0];
        m_vector[0][2] = -attitude[c3ga::E3] / m_scalar[0];
        m_valid = 1;
        return 0;
    case 4: // plane
        m_type |= MVI_PLANE;
        /*
        scalar 0: weight
        vector 0: normal
        vector 1: orthogonal to vector 0
        vector 2: orthogonal to vector 0 and vector 1
        point 0: point on plane closest to chosen origin
        */
        m_scalar[0] = sqrt(fabs(weight2));
        c3gaFactorBivector(o = attitude * (1.0 / m_scalar[0]), m_vector[0], m_vector[1], m_vector[2]);
        m_valid = 1;
        return 0;
    case 5: // space?
        // MVI_SPACE
        /*		m_valid = 0;
                m_type |= MVI_UNKNOWN;
                cprintf("mvInt::c3gaBoundFlat(): don't know what a grade %d bound flat is\n", grade);*/

        // TODO: Ask Leo this is where the scalars end up?
        m_type ^= MVI_DUAL;
        m_valid = 1;
        m_type |= MVI_SCALAR;
        o = ga::dual(X);
        m_scalar[0] = ga::scalar(o);
        // cprintf("mvInt::c3gaBoundFlat(): don't know what a grade %d bound flat is\n", grade);
        return 0;
    }

    return 0;
}

int mvInt::c3gaBoundTangent(const c3ga::mv &X, int grade, const c3ga::mv &attitude, const c3ga::mv &pos, double weight2)
{
    c3ga::mv o;

    /*
    For all types:
    point 0: point
    */
    if (pos[c3ga::Eo] != 0.0)
    {
        m_point[0][0] = pos[c3ga::E1] / pos[c3ga::Eo];
        m_point[0][1] = pos[c3ga::E2] / pos[c3ga::Eo];
        m_point[0][2] = pos[c3ga::E3] / pos[c3ga::Eo];
    }

    switch (grade)
    {
    case 0:
    case 5:
        m_valid = 0;
        m_type |= MVI_UNKNOWN;
        cprintf("mvInt::c3gaBoundTangent(): grade %d bound tangents do not exist\n", grade);
        return 0;
    case 1: // points
        m_type |= MVI_POINT;
        /*
        scalar 0: weight
        point 0: point
        */
        m_scalar[0] = sqrt(fabs(weight2));
        m_valid = 1;
        return 0;
    case 2: // bound vector
        m_type |= MVI_BOUND_TANGENT_VECTOR;
        /*
        scalar 0: weight
        point 0: point
        vector 0: direction
        */
        m_scalar[0] = sqrt(fabs(weight2));
        m_vector[0][0] = attitude[c3ga::E1] / m_scalar[0];
        m_vector[0][1] = attitude[c3ga::E2] / m_scalar[0];
        m_vector[0][2] = attitude[c3ga::E3] / m_scalar[0];
        m_valid = 1;
        return 0;
    case 3: // bound bivector
        m_type |= MVI_BOUND_TANGENT_BIVECTOR;
        /*
        scalar 0: weight
        point 0: point
        vector 0: normal
        vector 1: orthogonal to vector 0
        vector 2: orthogonal to vector 0 and vector 1
        */
        m_scalar[0] = sqrt(fabs(weight2));
        c3gaFactorBivector(o = attitude * (1.0 / m_scalar[0]), m_vector[0], m_vector[1], m_vector[2]);
        m_valid = 1;
        return 0;
    case 4: // bound trivector
        m_type |= MVI_BOUND_TANGENT_TRIVECTOR;
        /*
        scalar 0: weight
        point 0: point
        */
        m_scalar[0] = attitude[c3ga::E123];
        m_valid = 1;
        return 0;
    }

    return 0;
}

int mvInt::c3gaRound(const c3ga::mv &X, int grade, const c3ga::mv &attitude, const c3ga::mv &pos, double weight2)
{
    if (grade == 1)
    { // filter out dual spheres
        m_type ^= MVI_DUAL;
        // maybe put a '-' over here: -X.dual()? or adjust attitude, pos?
        // todo...
        return c3gaRound(-ga::dual(X), 5 - grade, -ga::dual(attitude), -pos, -1.0 * weight2);
    }

    // compute some basic properties
    c3ga::mv nX;
    double r = sqrt(fabs(weight2));
    c3ga::mv o(attitude * (1.0 / r)), pt1, pt2, v1, v2, v3;
    double sr = r * ((weight2 > 0.0) ? 1.0 : -1.0), n;

    /*
    For all types:
    scalar 0: signed radius
    */
    m_scalar[0] = sr;

    /*
    For all types:
    scalar 1: signed weight
    */
    c3ga::mv leoAtt = -0.5 * c3ga::ei<double>() * X * c3ga::ei<double>();
    double leoSqWeight = ga::norm_b(ga::lcont(c3ga::eo<double>(), leoAtt));
    m_scalar[1] = sqrt(fabs(leoSqWeight)) * ((leoSqWeight < 0.0) ? -1.0 : 1.0);

    /*
    For all types:
    point 0: point
    */
    if (pos[c3ga::Eo] != 0.0)
    {
        m_point[0][0] = pos[c3ga::E1] / pos[c3ga::Eo];
        m_point[0][1] = pos[c3ga::E2] / pos[c3ga::Eo];
        m_point[0][2] = pos[c3ga::E3] / pos[c3ga::Eo];
    }

    switch (grade)
    {
    case 0:
    case 5:
        m_valid = 0;
        m_type |= MVI_UNKNOWN;
        cprintf("mvInt::c3gaRound(): grade %d rounds do not exist\n", grade);
        return 0;
    case 2: // point pair
        m_type |= MVI_POINT_PAIR;
        /*
        scalar 0: signed radius
        scalar 1: weight
        point 0: center point
        point 1: point 1
        point 2: point 2
        vector 1: attitude
        */

        n = sqrt(fabs(ga::scalar(ga::lcont(X, X))));
        v1 = ga::lcont(c3ga::ei<double>(), X);
        v2 = n * v1;
        v3 = ga::lcont(c3ga::mv(-v1), X);

        pt1 = v3 + v2;
        pt2 = v3 - v2;

        nX = ga::normalize(attitude) * (2.0 * sr);

        m_vector[1][0] = ((m_scalar[0] < 0.0) ? -1.0 : 1.0) * nX[c3ga::E1];
        m_vector[1][1] = ((m_scalar[0] < 0.0) ? -1.0 : 1.0) * nX[c3ga::E2];
        m_vector[1][2] = ((m_scalar[0] < 0.0) ? -1.0 : 1.0) * nX[c3ga::E3];

        m_point[1][0] = m_point[0][0] - 0.5 * m_vector[1][0];
        m_point[1][1] = m_point[0][1] - 0.5 * m_vector[1][1];
        m_point[1][2] = m_point[0][2] - 0.5 * m_vector[1][2];

        m_point[2][0] = m_point[0][0] + 0.5 * m_vector[1][0];
        m_point[2][1] = m_point[0][1] + 0.5 * m_vector[1][1];
        m_point[2][2] = m_point[0][2] + 0.5 * m_vector[1][2];

        m_valid = 1;
        return 0;
    case 3: // circle
        m_type |= MVI_CIRCLE;
        /*
        scalar 0: signed radius
        scalar 1: weight
        point 0: center point
        vector 0: normal
        vector 1: orthogonal to vector 0
        vector 2: orthogonal to vector 0 and vector 1
        */

        c3gaFactorBivector(o, m_vector[0], m_vector[1], m_vector[2]);

        m_valid = 1;
        return 0;
    case 4: // sphere
        m_type |= MVI_SPHERE;
        /*
        scalar 0: signed radius
        scalar 1: weight
        point 0: center point
        */
        // temp test: is this weight ok?? (the sign???)
        m_scalar[1] = -ga::scalar(
            ga::lcont(X, c3ga::mv(c3ga::e1<double>() ^ c3ga::e2<double>() ^ c3ga::e3<double>() ^ c3ga::ei<double>())));

        m_valid = 1;
        return 0;
    }

    // todo...

    return 0;
}

int mvInt::c3gaFactorBivector(const c3ga::mv &B, double *normal, double *factor1, double *factor2)
{
    // get normal
    double scale;
    c3ga::mv nl(ga::normalize(
        ga::lcont(B, ga::inverse(c3ga::mv(c3ga::e1<double>() ^ c3ga::e2<double>() ^ c3ga::e3<double>())))));
    c3ga::mv pe1, pe2, rt;

    // get normal
    if (normal)
    {
        normal[0] = nl[c3ga::E1];
        normal[1] = nl[c3ga::E2];
        normal[2] = nl[c3ga::E3];
    }

    // set factor 1 and 2 (orthogonal to normal)
    if ((ga::norm_a(g_state->m_c3gaFactor[0]) > 0.0) && // user factorization
        (ga::norm_a(g_state->m_c3gaFactor[1]) > 0.0))
    {
        pe1 = ga::lcont(ga::lcont(g_state->m_c3gaFactor[0], B), ga::inverse(B));
        pe2 = ga::lcont(ga::lcont(g_state->m_c3gaFactor[1], B), ga::inverse(B));
    }
    else
    { // auto factorization
        c3gaRve3(rt, nl);
        pe1 = ga::takeGrade(c3ga::mv(rt * c3ga::e1<double>() * ga::inverse(rt)), ga::GRADE1);
        pe2 = ga::takeGrade(c3ga::mv(rt * c3ga::e2<double>() * ga::inverse(rt)), ga::GRADE1);
    }

    // if the bivector wasn't unit, adjust factors
    scale = ga::scalar(ga::igp(B, c3ga::mv(pe1 ^ pe2)));
    pe1 *= ((scale < 0.0) ? -1.0 : 1.0) * sqrt(fabs(scale));
    pe2 *= sqrt(fabs(scale));

    // set factors
    if (factor1)
    {
        factor1[0] = pe1[c3ga::E1];
        factor1[1] = pe1[c3ga::E2];
        factor1[2] = pe1[c3ga::E3];
    }
    if (factor2)
    {
        factor2[0] = pe2[c3ga::E1];
        factor2[1] = pe2[c3ga::E2];
        factor2[2] = pe2[c3ga::E3];
    }

    return 0;
}
