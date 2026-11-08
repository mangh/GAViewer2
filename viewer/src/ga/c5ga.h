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

#ifndef _GA_C5GA_H_
#define _GA_C5GA_H_

/*
The conformal model of 5D Euclidean space (Garamon generated from
garamon/c5ga.conf, namespace c5ga), as used by GAViewer: the multivector type
c5ga::mv and its ga::algebraTraits.

Basis vectors e1, ..., e5, no (Garamon name 'o'), ni ('i'), with no.ni = -1 and
no.no = ni.ni = 0, ordered as in the Gaigen c5ga GAViewer was built on, so that
the Garamon basis blades have the Gaigen orientations (e.g. E1o = e1^no).
*/

#include <array>

#include <c5ga/Mvec.hpp> // IWYU pragma: export

#include "traits.h" // IWYU pragma: export

namespace c5ga
{
using mv = Mvec<double>; // the multivector type of c5ga in GAViewer

// the vector x1 e1 + ... + x5 e5 + o no + i ni
inline mv vector(double x1, double x2, double x3, double x4, double x5, double o, double i)
{
    mv v;
    v[E1] = x1;
    v[E2] = x2;
    v[E3] = x3;
    v[E4] = x4;
    v[E5] = x5;
    v[Eo] = o;
    v[Ei] = i;
    return v;
}
} // namespace c5ga

template <> struct ga::algebraTraits<c5ga::mv>
{
    static constexpr int dimension = 7;
    static constexpr std::array<const char *, 7> vectorNames = {"e1", "e2", "e3", "e4", "e5", "no", "ni"};
    static constexpr bool euclideanMetric = false; // no.ni = -1

    static c5ga::mv I()
    {
        return c5ga::I<double>();
    }
    static c5ga::mv Iinv()
    {
        return c5ga::Iinv<double>();
    }

    // the basis of the Gaigen c5ga algebra
    static inline std::array<displayBlade, 128> displayBasis = {{
        {"1", c5ga::scalar, 1.0},
        {"e1", c5ga::E1, 1.0},
        {"e2", c5ga::E2, 1.0},
        {"e3", c5ga::E3, 1.0},
        {"e4", c5ga::E4, 1.0},
        {"e5", c5ga::E5, 1.0},
        {"no", c5ga::Eo, 1.0},
        {"ni", c5ga::Ei, 1.0},
        {"e1^e2", c5ga::E12, 1.0},
        {"e1^e3", c5ga::E13, 1.0},
        {"e2^e3", c5ga::E23, 1.0},
        {"e1^e4", c5ga::E14, 1.0},
        {"e2^e4", c5ga::E24, 1.0},
        {"e3^e4", c5ga::E34, 1.0},
        {"e1^e5", c5ga::E15, 1.0},
        {"e2^e5", c5ga::E25, 1.0},
        {"e3^e5", c5ga::E35, 1.0},
        {"e4^e5", c5ga::E45, 1.0},
        {"e1^no", c5ga::E1o, 1.0},
        {"e2^no", c5ga::E2o, 1.0},
        {"e3^no", c5ga::E3o, 1.0},
        {"e4^no", c5ga::E4o, 1.0},
        {"e5^no", c5ga::E5o, 1.0},
        {"e1^ni", c5ga::E1i, 1.0},
        {"e2^ni", c5ga::E2i, 1.0},
        {"e3^ni", c5ga::E3i, 1.0},
        {"e4^ni", c5ga::E4i, 1.0},
        {"e5^ni", c5ga::E5i, 1.0},
        {"no^ni", c5ga::Eoi, 1.0},
        {"e1^e2^e3", c5ga::E123, 1.0},
        {"e1^e2^e4", c5ga::E124, 1.0},
        {"e1^e3^e4", c5ga::E134, 1.0},
        {"e2^e3^e4", c5ga::E234, 1.0},
        {"e1^e2^e5", c5ga::E125, 1.0},
        {"e1^e3^e5", c5ga::E135, 1.0},
        {"e2^e3^e5", c5ga::E235, 1.0},
        {"e1^e4^e5", c5ga::E145, 1.0},
        {"e2^e4^e5", c5ga::E245, 1.0},
        {"e3^e4^e5", c5ga::E345, 1.0},
        {"e1^e2^no", c5ga::E12o, 1.0},
        {"e1^e3^no", c5ga::E13o, 1.0},
        {"e2^e3^no", c5ga::E23o, 1.0},
        {"e1^e4^no", c5ga::E14o, 1.0},
        {"e2^e4^no", c5ga::E24o, 1.0},
        {"e3^e4^no", c5ga::E34o, 1.0},
        {"e1^e5^no", c5ga::E15o, 1.0},
        {"e2^e5^no", c5ga::E25o, 1.0},
        {"e3^e5^no", c5ga::E35o, 1.0},
        {"e4^e5^no", c5ga::E45o, 1.0},
        {"e1^e2^ni", c5ga::E12i, 1.0},
        {"e1^e3^ni", c5ga::E13i, 1.0},
        {"e2^e3^ni", c5ga::E23i, 1.0},
        {"e1^e4^ni", c5ga::E14i, 1.0},
        {"e2^e4^ni", c5ga::E24i, 1.0},
        {"e3^e4^ni", c5ga::E34i, 1.0},
        {"e1^e5^ni", c5ga::E15i, 1.0},
        {"e2^e5^ni", c5ga::E25i, 1.0},
        {"e3^e5^ni", c5ga::E35i, 1.0},
        {"e4^e5^ni", c5ga::E45i, 1.0},
        {"e1^no^ni", c5ga::E1oi, 1.0},
        {"e2^no^ni", c5ga::E2oi, 1.0},
        {"e3^no^ni", c5ga::E3oi, 1.0},
        {"e4^no^ni", c5ga::E4oi, 1.0},
        {"e5^no^ni", c5ga::E5oi, 1.0},
        {"e1^e2^e3^e4", c5ga::E1234, 1.0},
        {"e1^e2^e3^e5", c5ga::E1235, 1.0},
        {"e1^e2^e4^e5", c5ga::E1245, 1.0},
        {"e1^e3^e4^e5", c5ga::E1345, 1.0},
        {"e2^e3^e4^e5", c5ga::E2345, 1.0},
        {"e1^e2^e3^no", c5ga::E123o, 1.0},
        {"e1^e2^e4^no", c5ga::E124o, 1.0},
        {"e1^e3^e4^no", c5ga::E134o, 1.0},
        {"e2^e3^e4^no", c5ga::E234o, 1.0},
        {"e1^e2^e5^no", c5ga::E125o, 1.0},
        {"e1^e3^e5^no", c5ga::E135o, 1.0},
        {"e2^e3^e5^no", c5ga::E235o, 1.0},
        {"e1^e4^e5^no", c5ga::E145o, 1.0},
        {"e2^e4^e5^no", c5ga::E245o, 1.0},
        {"e3^e4^e5^no", c5ga::E345o, 1.0},
        {"e1^e2^e3^ni", c5ga::E123i, 1.0},
        {"e1^e2^e4^ni", c5ga::E124i, 1.0},
        {"e1^e3^e4^ni", c5ga::E134i, 1.0},
        {"e2^e3^e4^ni", c5ga::E234i, 1.0},
        {"e1^e2^e5^ni", c5ga::E125i, 1.0},
        {"e1^e3^e5^ni", c5ga::E135i, 1.0},
        {"e2^e3^e5^ni", c5ga::E235i, 1.0},
        {"e1^e4^e5^ni", c5ga::E145i, 1.0},
        {"e2^e4^e5^ni", c5ga::E245i, 1.0},
        {"e3^e4^e5^ni", c5ga::E345i, 1.0},
        {"e1^e2^no^ni", c5ga::E12oi, 1.0},
        {"e1^e3^no^ni", c5ga::E13oi, 1.0},
        {"e2^e3^no^ni", c5ga::E23oi, 1.0},
        {"e1^e4^no^ni", c5ga::E14oi, 1.0},
        {"e2^e4^no^ni", c5ga::E24oi, 1.0},
        {"e3^e4^no^ni", c5ga::E34oi, 1.0},
        {"e1^e5^no^ni", c5ga::E15oi, 1.0},
        {"e2^e5^no^ni", c5ga::E25oi, 1.0},
        {"e3^e5^no^ni", c5ga::E35oi, 1.0},
        {"e4^e5^no^ni", c5ga::E45oi, 1.0},
        {"e1^e2^e3^e4^e5", c5ga::E12345, 1.0},
        {"e1^e2^e3^e4^no", c5ga::E1234o, 1.0},
        {"e1^e2^e3^e5^no", c5ga::E1235o, 1.0},
        {"e1^e2^e4^e5^no", c5ga::E1245o, 1.0},
        {"e1^e3^e4^e5^no", c5ga::E1345o, 1.0},
        {"e2^e3^e4^e5^no", c5ga::E2345o, 1.0},
        {"e1^e2^e3^e4^ni", c5ga::E1234i, 1.0},
        {"e1^e2^e3^e5^ni", c5ga::E1235i, 1.0},
        {"e1^e2^e4^e5^ni", c5ga::E1245i, 1.0},
        {"e1^e3^e4^e5^ni", c5ga::E1345i, 1.0},
        {"e2^e3^e4^e5^ni", c5ga::E2345i, 1.0},
        {"e1^e2^e3^no^ni", c5ga::E123oi, 1.0},
        {"e1^e2^e4^no^ni", c5ga::E124oi, 1.0},
        {"e1^e3^e4^no^ni", c5ga::E134oi, 1.0},
        {"e2^e3^e4^no^ni", c5ga::E234oi, 1.0},
        {"e1^e2^e5^no^ni", c5ga::E125oi, 1.0},
        {"e1^e3^e5^no^ni", c5ga::E135oi, 1.0},
        {"e2^e3^e5^no^ni", c5ga::E235oi, 1.0},
        {"e1^e4^e5^no^ni", c5ga::E145oi, 1.0},
        {"e2^e4^e5^no^ni", c5ga::E245oi, 1.0},
        {"e3^e4^e5^no^ni", c5ga::E345oi, 1.0},
        {"e1^e2^e3^e4^e5^no", c5ga::E12345o, 1.0},
        {"e1^e2^e3^e4^e5^ni", c5ga::E12345i, 1.0},
        {"e1^e2^e3^e4^no^ni", c5ga::E1234oi, 1.0},
        {"e1^e2^e3^e5^no^ni", c5ga::E1235oi, 1.0},
        {"e1^e2^e4^e5^no^ni", c5ga::E1245oi, 1.0},
        {"e1^e3^e4^e5^no^ni", c5ga::E1345oi, 1.0},
        {"e2^e3^e4^e5^no^ni", c5ga::E2345oi, 1.0},
        {"e1^e2^e3^e4^e5^no^ni", c5ga::E12345oi, 1.0},
    }};
};

#endif /* _GA_C5GA_H_ */
