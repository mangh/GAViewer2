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

#ifndef _GA_CAST_H_
#define _GA_CAST_H_

/*
The cast of a multivector from one algebra to another (the console's
cast_e3ga(), (c3ga)x, ...): the basis vectors the two algebras share are
preserved, the others are dropped. Basis vectors are identified by their
canonical names (algebraTraits<MV>::vectorNames), where "e0" (p3ga) and "no"
(c3ga, c5ga, i2ga) are the same vector. A basis blade of the source maps to
the outer product of the images of its factors (the orientations of the two
display bases do not matter), or to 0 if one of its factors has no image.

E.g. c3ga -> p3ga: e1^no -> e1^e0, e3^ni -> 0; e3ga -> i2ga: e1^e3 -> 0.
*/

#include <string_view>
#include <vector>

#include "algorithms.h"
#include "traits.h"

namespace ga
{

namespace detail
{

inline std::string_view canonicalVectorName(std::string_view name)
{
    return (name == "e0") ? std::string_view("no") : name;
}

// the index (Garamon order) of the basis vector of MV with canonical name 'name', -1 if none
template <garamonMV MV> int vectorIndex(std::string_view name)
{
    const auto &names = algebraTraits<MV>::vectorNames;
    for (int i = 0; i < (int)names.size(); i++)
        if (canonicalVectorName(names[i]) == canonicalVectorName(name))
            return i;
    return -1;
}

struct castTerm
{
    unsigned from; // bitmap of the basis blade in the source
    unsigned to;   // bitmap of its image in the target
    double sign;   // image = sign * basis blade 'to'
};

} // namespace detail

template <garamonMV To, garamonMV From> To castByVectors(const From &a)
{
    // the image of every basis blade of the source (computed once per pair of algebras)
    static const std::vector<detail::castTerm> terms = [] {
        std::vector<detail::castTerm> t;
        constexpr int n = algebraTraits<From>::dimension;
        for (unsigned bitmap = 0; bitmap < (1u << n); bitmap++)
        {
            To image(1.0);
            unsigned to = 0;
            bool preserved = true;
            for (int k = 0; (k < n) && preserved; k++)
                if (bitmap & (1u << k))
                {
                    const int j = detail::vectorIndex<To>(algebraTraits<From>::vectorNames[k]);
                    if (j < 0)
                        preserved = false;
                    else
                    {
                        image = image ^ basisVector<To>(j);
                        to |= 1u << j;
                    }
                }
            if (preserved)
                t.push_back({bitmap, to, image[to]});
        }
        return t;
    }();

    To result;
    for (const detail::castTerm &t : terms)
        if (a[t.from] != 0.0)
            result[t.to] += t.sign * a[t.from];
    return result;
}

} // namespace ga

#endif /* _GA_CAST_H_ */
