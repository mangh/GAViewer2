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

#ifndef _GA_E3GA_H_
#define _GA_E3GA_H_

/*
The Euclidean geometric algebra of 3D space (Garamon generated, namespace
e3ga), as used by GAViewer: the multivector type e3ga::mv and its
ga::algebraTraits.
*/

#include <array>

#include <e3ga/Mvec.hpp> // IWYU pragma: export

#include "traits.h" // IWYU pragma: export

namespace e3ga
{
using mv = Mvec<double>; // the multivector type of e3ga in GAViewer

// the vector x e1 + y e2 + z e3
inline mv vector(double x, double y, double z)
{
    mv v;
    v[E1] = x;
    v[E2] = y;
    v[E3] = z;
    return v;
}

// the vector with the coordinates xyz[0], xyz[1], xyz[2]
template <class F> mv vector(const F *xyz)
{
    return vector((double)xyz[0], (double)xyz[1], (double)xyz[2]);
}

// the coordinates of the vector part of 'v' (e1, e2, e3), e.g. for glVertex3dv(xyz(v).data())
inline std::array<double, 3> xyz(const mv &v)
{
    return {v[E1], v[E2], v[E3]};
}
} // namespace e3ga

template <> struct ga::algebraTraits<e3ga::mv>
{
    static constexpr int dimension = 3;
    static constexpr std::array<const char *, 3> vectorNames = {"e1", "e2", "e3"};
    static constexpr bool euclideanMetric = true;

    static e3ga::mv I()
    {
        return e3ga::I<double>();
    }
    static e3ga::mv Iinv()
    {
        return e3ga::Iinv<double>();
    }

    // the basis of the Gaigen e3ga algebra
    static inline std::array<displayBlade, 8> displayBasis = {{
        {"1", e3ga::scalar, 1.0},
        {"e1", e3ga::E1, 1.0},
        {"e2", e3ga::E2, 1.0},
        {"e3", e3ga::E3, 1.0},
        {"e2^e3", e3ga::E23, 1.0},
        {"e3^e1", e3ga::E13, -1.0},
        {"e1^e2", e3ga::E12, 1.0},
        {"e1^e2^e3", e3ga::E123, 1.0},
    }};
};

#endif /* _GA_E3GA_H_ */
