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

#ifndef _GA_I2GA_H_
#define _GA_I2GA_H_

/*
The 'image algebra' i2ga (Garamon generated from garamon/i2ga.conf, namespace
i2ga), as used by GAViewer: the multivector type i2ga::mv and its
ga::algebraTraits.

Basis vectors e1, e2, no (Garamon name 'o'), ni ('i'), go ('O'), gi ('I'), with
e1.e1 = e2.e2 = 1, no.ni = go.gi = -1 and all other products 0, ordered as in the
Gaigen i2ga GAViewer was built on, so that the Garamon basis blades have the
Gaigen orientations (e.g. E1o = e1^no, EOI = go^gi).
*/

#include <array>

#include <i2ga/Mvec.hpp> // IWYU pragma: export

#include "traits.h" // IWYU pragma: export

namespace i2ga
{
using mv = Mvec<double>; // the multivector type of i2ga in GAViewer

// the vector x e1 + y e2 + o no + i ni + go go + gi gi
inline mv vector(double x, double y, double o, double i, double go, double gi)
{
    mv v;
    v[E1] = x;
    v[E2] = y;
    v[Eo] = o;
    v[Ei] = i;
    v[EO] = go;
    v[EI] = gi;
    return v;
}
} // namespace i2ga

template <> struct ga::algebraTraits<i2ga::mv>
{
    static constexpr int dimension = 6;
    static constexpr std::array<const char *, 6> vectorNames = {"e1", "e2", "no", "ni", "go", "gi"};
    static constexpr bool euclideanMetric = false; // no.ni = go.gi = -1

    static i2ga::mv I()
    {
        return i2ga::I<double>();
    }
    static i2ga::mv Iinv()
    {
        return i2ga::Iinv<double>();
    }

    // the basis of the Gaigen i2ga algebra
    static inline std::array<displayBlade, 64> displayBasis = {{
        {"1", i2ga::scalar, 1.0},
        {"e1", i2ga::E1, 1.0},
        {"e2", i2ga::E2, 1.0},
        {"no", i2ga::Eo, 1.0},
        {"ni", i2ga::Ei, 1.0},
        {"go", i2ga::EO, 1.0},
        {"gi", i2ga::EI, 1.0},
        {"e1^e2", i2ga::E12, 1.0},
        {"e1^no", i2ga::E1o, 1.0},
        {"e2^no", i2ga::E2o, 1.0},
        {"e1^ni", i2ga::E1i, 1.0},
        {"e2^ni", i2ga::E2i, 1.0},
        {"no^ni", i2ga::Eoi, 1.0},
        {"e1^go", i2ga::E1O, 1.0},
        {"e2^go", i2ga::E2O, 1.0},
        {"no^go", i2ga::EoO, 1.0},
        {"ni^go", i2ga::EiO, 1.0},
        {"e1^gi", i2ga::E1I, 1.0},
        {"e2^gi", i2ga::E2I, 1.0},
        {"no^gi", i2ga::EoI, 1.0},
        {"ni^gi", i2ga::EiI, 1.0},
        {"go^gi", i2ga::EOI, 1.0},
        {"e1^e2^no", i2ga::E12o, 1.0},
        {"e1^e2^ni", i2ga::E12i, 1.0},
        {"e1^no^ni", i2ga::E1oi, 1.0},
        {"e2^no^ni", i2ga::E2oi, 1.0},
        {"e1^e2^go", i2ga::E12O, 1.0},
        {"e1^no^go", i2ga::E1oO, 1.0},
        {"e2^no^go", i2ga::E2oO, 1.0},
        {"e1^ni^go", i2ga::E1iO, 1.0},
        {"e2^ni^go", i2ga::E2iO, 1.0},
        {"no^ni^go", i2ga::EoiO, 1.0},
        {"e1^e2^gi", i2ga::E12I, 1.0},
        {"e1^no^gi", i2ga::E1oI, 1.0},
        {"e2^no^gi", i2ga::E2oI, 1.0},
        {"e1^ni^gi", i2ga::E1iI, 1.0},
        {"e2^ni^gi", i2ga::E2iI, 1.0},
        {"no^ni^gi", i2ga::EoiI, 1.0},
        {"e1^go^gi", i2ga::E1OI, 1.0},
        {"e2^go^gi", i2ga::E2OI, 1.0},
        {"no^go^gi", i2ga::EoOI, 1.0},
        {"ni^go^gi", i2ga::EiOI, 1.0},
        {"e1^e2^no^ni", i2ga::E12oi, 1.0},
        {"e1^e2^no^go", i2ga::E12oO, 1.0},
        {"e1^e2^ni^go", i2ga::E12iO, 1.0},
        {"e1^no^ni^go", i2ga::E1oiO, 1.0},
        {"e2^no^ni^go", i2ga::E2oiO, 1.0},
        {"e1^e2^no^gi", i2ga::E12oI, 1.0},
        {"e1^e2^ni^gi", i2ga::E12iI, 1.0},
        {"e1^no^ni^gi", i2ga::E1oiI, 1.0},
        {"e2^no^ni^gi", i2ga::E2oiI, 1.0},
        {"e1^e2^go^gi", i2ga::E12OI, 1.0},
        {"e1^no^go^gi", i2ga::E1oOI, 1.0},
        {"e2^no^go^gi", i2ga::E2oOI, 1.0},
        {"e1^ni^go^gi", i2ga::E1iOI, 1.0},
        {"e2^ni^go^gi", i2ga::E2iOI, 1.0},
        {"no^ni^go^gi", i2ga::EoiOI, 1.0},
        {"e1^e2^no^ni^go", i2ga::E12oiO, 1.0},
        {"e1^e2^no^ni^gi", i2ga::E12oiI, 1.0},
        {"e1^e2^no^go^gi", i2ga::E12oOI, 1.0},
        {"e1^e2^ni^go^gi", i2ga::E12iOI, 1.0},
        {"e1^no^ni^go^gi", i2ga::E1oiOI, 1.0},
        {"e2^no^ni^go^gi", i2ga::E2oiOI, 1.0},
        {"e1^e2^no^ni^go^gi", i2ga::E12oiOI, 1.0},
    }};
};

#endif /* _GA_I2GA_H_ */
