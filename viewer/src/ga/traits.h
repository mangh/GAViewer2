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

#ifndef _GA_TRAITS_H_
#define _GA_TRAITS_H_

/*
What GAViewer needs to know about a (Garamon generated) algebra, beyond the
multivector type itself: ga::algebraTraits<MV> is specialized for every
multivector type MV (e.g. in ga/e3ga.h) with

    static constexpr int dimension;          // number of basis vectors
    static constexpr bool euclideanMetric;   // true if the basis vectors are orthonormal
    static MV I();                           // the pseudoscalar
    static MV Iinv();                        // its inverse
    static std::array<displayBlade, 1 << dimension> displayBasis;

A Garamon multivector addresses its coordinates by 'bitmap' (xor index: bit
i set = basis vector i is a factor of the basis blade, e.g. e1^e3 = 0b101).
GAViewer shows multivectors in the basis of the Gaigen algebras it used to
be built on (e.g. e2^e3, e3^e1, e1^e2 for the bivectors of e3ga, where
Garamon has e1^e2, e1^e3, e2^e3): the display basis lists, in that order,
the name of each basis blade, its Garamon bitmap and the sign that relates
the two orientations (display coordinate = sign * Garamon coordinate).
The order of the display basis also matters to some algorithms (e.g. which
coordinate is the largest when two are equal), so they iterate in display
order, as Gaigen did.
*/

#include <string>

namespace ga
{

struct displayBlade
{
    std::string name; // e.g. "e3^e1" (can be changed, see renameBasisVector())
    unsigned bitmap;  // Garamon bitmap (xor index) of the basis blade, e.g. 0b101 (e1^e3)
    double sign;      // display coordinate = sign * Garamon coordinate (+1 or -1)
};

template <class MV> struct algebraTraits; // specialized for every algebra

// a Garamon multivector type (one with algebraTraits)
template <class MV>
concept garamonMV = requires { algebraTraits<MV>::dimension; };

// the grade of a basis blade, given its bitmap
constexpr int bitmapGrade(unsigned bitmap)
{
    int g = 0;
    for (; bitmap; bitmap &= bitmap - 1)
        g++;
    return g;
}

} // namespace ga

#endif /* _GA_TRAITS_H_ */
