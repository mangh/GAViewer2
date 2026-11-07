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

#include <array>

#include "gaincludes.h"
#include "osdep.h"
#include "util.h"

void e3gaRotorToOpenGLMatrix(const e3ga::mv &rot, float rotM[16])
{
    // the columns of the (column-major) OpenGL matrix are the images of e1, e2, e3
    e3ga::mv ri = ga::inverse(rot);
    const e3ga::mv basis[3] = {e3ga::e1<double>(), e3ga::e2<double>(), e3ga::e3<double>()};
    for (int col = 0; col < 3; col++)
    {
        std::array<double, 3> image = e3ga::xyz(rot * basis[col] * ri);
        for (int row = 0; row < 3; row++)
            rotM[col * 4 + row] = (float)image[row];
        rotM[col * 4 + 3] = 0.0f;
    }
    rotM[12] = rotM[13] = rotM[14] = 0.0f;
    rotM[15] = 1.0f;
}

void GLmatrixVectorMul(const double m[16], const double v[4], double r[4])
{
    int i, j;
    for (i = 0; i < 4; i++)
    {
        r[i] = 0;
        for (j = 0; j < 4; j++)
            r[i] += m[j * 4 + i] * v[j];
    }
}

int e3gaRve3(e3ga::mv &R, const e3ga::mv &v)
{
    R = 1.0 + ga::normalize(v) * e3ga::e3<double>();
    if (sqrt(ga::norm_a(R)) < 1e-8)
        R = e3ga::e3<double>() ^ e3ga::e1<double>(); // any 180 degree rot in a plane containing e3?
    return 0;
}

int p3gaRve3(p3ga::mv &R, const p3ga::mv &v)
{
    R = 1.0 + ga::normalize(v) * p3ga::e3<double>();
    if (sqrt(ga::norm_a(R)) < 1e-8)
        R = p3ga::e3<double>() ^ p3ga::e1<double>(); // any 180 degree rot in a plane containing e3?
    return 0;
}

int c3gaRve3(c3ga::mv &R, const c3ga::mv &v)
{
    R = 1.0 + ga::normalize(v) * c3ga::e3<double>();
    if (sqrt(ga::norm_a(R)) < 1e-8)
        R = c3ga::e3<double>() ^ c3ga::e1<double>(); // any 180 degree rot in a plane containing e3?
    return 0;
}

int rotor2Bivector(e3ga::mv &B, const e3ga::mv &R)
{
    e3ga::mv Rn(R * (1.0 / sqrt(ga::norm_a(R))));
    double ca = ga::scalar(Rn);
    if ((ga::gradeNorm_a(Rn, 2) < 1e-9) || (sin(acos(ca)) < 1e-9))
        B = 0;
    else
        B = ga::takeGrade(e3ga::mv(acos(ca) * Rn * (1.0 / sin(acos(ca)))), GRADE2);

    return 0;
}

static int setColor(float c[4], float r, float g, float b, float a)
{
    c[0] = r;
    c[1] = g;
    c[2] = b;
    c[3] = a;
    return 0;
}

int getColor(const char *name, float c[4])
{
    if ((!strcmpnc(name, "r")) || (!strcmpnc(name, "red")))
        return setColor(c, 1.0, 0.0, 0.0, 1.0);
    else if ((!strcmpnc(name, "g")) || (!strcmpnc(name, "green")))
        return setColor(c, 0.0, 1.0, 0.0, 1.0);
    else if ((!strcmpnc(name, "b")) || (!strcmpnc(name, "blue")))
        return setColor(c, 0.0, 0.0, 1.0, 1.0);
    else if ((!strcmpnc(name, "w")) || (!strcmpnc(name, "white")))
        return setColor(c, 1.0, 1.0, 1.0, 1.0);
    else if ((!strcmpnc(name, "m")) || (!strcmpnc(name, "magenta")))
        return setColor(c, 1.0, 0.0, 1.0, 1.0);
    else if ((!strcmpnc(name, "y")) || (!strcmpnc(name, "yellow")))
        return setColor(c, 1.0, 1.0, 0.0, 1.0);
    else if ((!strcmpnc(name, "c")) || (!strcmpnc(name, "cyan")))
        return setColor(c, 0.0, 1.0, 1.0, 1.0);
    else if ((!strcmpnc(name, "k")) || (!strcmpnc(name, "black")))
        return setColor(c, 0.0, 0.0, 0.0, 1.0);
    else if ((!strcmpnc(name, "grey")) || (!strcmpnc(name, "gray")))
        return setColor(c, 0.5f, 0.5f, 0.5f, 1.0f);
    return -1;
}
