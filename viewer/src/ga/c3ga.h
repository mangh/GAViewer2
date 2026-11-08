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

#ifndef _GA_C3GA_H_
#define _GA_C3GA_H_

/*
The conformal model of 3D Euclidean space (Garamon generated from
garamon/c3ga.conf, namespace c3ga), as used by GAViewer: the multivector type
c3ga::mv and its ga::algebraTraits.

Basis vectors e1, e2, e3, no (Garamon name 'o'), ni ('i'), with no.ni = -1 and
no.no = ni.ni = 0, ordered as in the Gaigen c3ga GAViewer was built on, so that
the Garamon basis blades have the Gaigen orientations (e.g. E1o = e1^no).
*/

#include <array>

#include <c3ga/Mvec.hpp> // IWYU pragma: export

#include "traits.h" // IWYU pragma: export

namespace c3ga
{
using mv = Mvec<double>; // the multivector type of c3ga in GAViewer

// the vector x e1 + y e2 + z e3 + o no + i ni
inline mv vector(double x, double y, double z, double o, double i)
{
    mv v;
    v[E1] = x;
    v[E2] = y;
    v[E3] = z;
    v[Eo] = o;
    v[Ei] = i;
    return v;
}

// the coordinates of the Euclidean part of the vector part of 'v' (e1, e2, e3)
inline std::array<double, 3> xyz(const mv &v)
{
    return {v[E1], v[E2], v[E3]};
}
} // namespace c3ga

template <> struct ga::algebraTraits<c3ga::mv>
{
    static constexpr int dimension = 5;
    static constexpr std::array<const char *, 5> vectorNames = {"e1", "e2", "e3", "no", "ni"};
    static constexpr bool euclideanMetric = false; // no.ni = -1

    static c3ga::mv I()
    {
        return c3ga::I<double>();
    }
    static c3ga::mv Iinv()
    {
        return c3ga::Iinv<double>();
    }

    // the basis of the Gaigen c3ga algebra
    static inline std::array<displayBlade, 32> displayBasis = {{
        {"1", c3ga::scalar, 1.0},
        {"e1", c3ga::E1, 1.0},
        {"e2", c3ga::E2, 1.0},
        {"e3", c3ga::E3, 1.0},
        {"no", c3ga::Eo, 1.0},
        {"ni", c3ga::Ei, 1.0},
        {"e1^e2", c3ga::E12, 1.0},
        {"e1^e3", c3ga::E13, 1.0},
        {"e2^e3", c3ga::E23, 1.0},
        {"e1^no", c3ga::E1o, 1.0},
        {"e2^no", c3ga::E2o, 1.0},
        {"e3^no", c3ga::E3o, 1.0},
        {"e1^ni", c3ga::E1i, 1.0},
        {"e2^ni", c3ga::E2i, 1.0},
        {"e3^ni", c3ga::E3i, 1.0},
        {"no^ni", c3ga::Eoi, 1.0},
        {"e1^e2^e3", c3ga::E123, 1.0},
        {"e1^e2^no", c3ga::E12o, 1.0},
        {"e1^e3^no", c3ga::E13o, 1.0},
        {"e2^e3^no", c3ga::E23o, 1.0},
        {"e1^e2^ni", c3ga::E12i, 1.0},
        {"e1^e3^ni", c3ga::E13i, 1.0},
        {"e2^e3^ni", c3ga::E23i, 1.0},
        {"e1^no^ni", c3ga::E1oi, 1.0},
        {"e2^no^ni", c3ga::E2oi, 1.0},
        {"e3^no^ni", c3ga::E3oi, 1.0},
        {"e1^e2^e3^no", c3ga::E123o, 1.0},
        {"e1^e2^e3^ni", c3ga::E123i, 1.0},
        {"e1^e2^no^ni", c3ga::E12oi, 1.0},
        {"e1^e3^no^ni", c3ga::E13oi, 1.0},
        {"e2^e3^no^ni", c3ga::E23oi, 1.0},
        {"e1^e2^e3^no^ni", c3ga::E123oi, 1.0},
    }};
};

#endif /* _GA_C3GA_H_ */
