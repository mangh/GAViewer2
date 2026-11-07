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

#ifndef _GA_P3GA_H_
#define _GA_P3GA_H_

/*
The homogeneous ('projective') model of 3D Euclidean space (Garamon generated
from garamon/p3ga.conf, namespace p3ga), as used by GAViewer: the multivector
type p3ga::mv and its ga::algebraTraits.

The metric is Euclidean (e0.e0 = 1), as in the Gaigen p3ga GAViewer was built
on; a point at x e1 + y e2 + z e3 is represented by e0 + x e1 + y e2 + z e3.
*/

#include <array>

#include <p3ga/Mvec.hpp> // IWYU pragma: export

#include "traits.h" // IWYU pragma: export

namespace p3ga
{
using mv = Mvec<double>; // the multivector type of p3ga in GAViewer

// the vector x e1 + y e2 + z e3 + w e0
inline mv vector(double x, double y, double z, double w)
{
    mv v;
    v[E1] = x;
    v[E2] = y;
    v[E3] = z;
    v[E0] = w;
    return v;
}

// the coordinates of the vector part of 'v' (e1, e2, e3, e0)
inline std::array<double, 4> xyzw(const mv &v)
{
    return {v[E1], v[E2], v[E3], v[E0]};
}
} // namespace p3ga

template <> struct ga::algebraTraits<p3ga::mv>
{
    static constexpr int dimension = 4;

    static p3ga::mv I()
    {
        return p3ga::I<double>();
    }
    static p3ga::mv Iinv()
    {
        return p3ga::Iinv<double>();
    }

    // the basis of the Gaigen p3ga algebra
    static inline std::array<displayBlade, 16> displayBasis = {{
        {"1", p3ga::scalar, 1.0},
        {"e1", p3ga::E1, 1.0},
        {"e2", p3ga::E2, 1.0},
        {"e3", p3ga::E3, 1.0},
        {"e0", p3ga::E0, 1.0},
        {"e2^e3", p3ga::E23, 1.0},
        {"e3^e1", p3ga::E13, -1.0},
        {"e1^e2", p3ga::E12, 1.0},
        {"e1^e0", p3ga::E10, 1.0},
        {"e2^e0", p3ga::E20, 1.0},
        {"e3^e0", p3ga::E30, 1.0},
        {"e2^e3^e0", p3ga::E230, 1.0},
        {"e3^e1^e0", p3ga::E130, -1.0},
        {"e1^e2^e0", p3ga::E120, 1.0},
        {"e1^e2^e3", p3ga::E123, 1.0},
        {"e1^e2^e3^e0", p3ga::E1230, 1.0},
    }};
};

#endif /* _GA_P3GA_H_ */
