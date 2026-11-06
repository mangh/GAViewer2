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

// Ported from the Gaigen 1 generated algebras (e3ga.cpp, ...),
// Copyright 2002-2009, Daniel Fontijne, University of Amsterdam.

#ifndef _GA_FORMAT_H_
#define _GA_FORMAT_H_

/*
Text representation of multivectors, as in the Gaigen 1 algebras GAViewer was
built on: toString() writes "1.00*e1 + -0.50*e3^e1" (display basis, see
traits.h), parse() reads such text (as the .ga files contain it).
*/

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <utility>
#include <vector>

#include "algorithms.h"
#include "traits.h"

namespace ga
{

// The multivector as text: the terms "<coordinate>*<basis blade>" (in display
// basis order) of the coordinates with an absolute value > 1e-14, separated by
// " + "; "0" if there are none. 'fp' is the printf conversion of a coordinate
// (e.g. "%2.2f", "%e"); the coordinate is printed as a float (as in Gaigen).
template <garamonMV MV> std::string toString(const MV &a, const char *fp = "%2.2f")
{
    char format[64], term[512];
    std::snprintf(format, sizeof(format), "%%s%s%%s%%s", fp);

    std::string s;
    bool first = true;
    for (std::size_t i = 0; i < algebraTraits<MV>::displayBasis.size(); i++)
    {
        double c = coordinate(a, i);
        if (std::fabs(c) > 1e-14)
        {
            std::snprintf(term, sizeof(term), format, first ? "" : " + ", (float)c, (i) ? "*" : "",
                          (i) ? algebraTraits<MV>::displayBasis[i].name.c_str() : "");
            s += term;
            first = false;
        }
    }
    return first ? std::string("0") : s;
}

// Renames a basis vector in the display names of the basis blades (e.g. "e1" -> "x"
// turns "e1^e2" into "x^e2"); every occurrence of 'oldName' is replaced, as in Gaigen.
template <garamonMV MV> void renameBasisVector(const std::string &oldName, const std::string &newName)
{
    if (oldName.empty())
        return;
    for (displayBlade &b : algebraTraits<MV>::displayBasis)
    {
        std::string name = b.name;
        for (std::size_t pos = 0; (pos = name.find(oldName, pos)) != std::string::npos; pos += newName.size())
            name.replace(pos, oldName.size(), newName);
        b.name = name;
    }
}

// The names parse() knows: the basis vectors (by default) and any added name
// (e.g. "I3" for the pseudoscalar), plus optional start and end delimiters
// (e.g. '[' and ']') around the multivector.
template <garamonMV MV> class basisNames
{
  public:
    basisNames(char startDelimiter = 0, char endDelimiter = 0)
        : m_startDelimiter(startDelimiter), m_endDelimiter(endDelimiter)
    {
        for (std::size_t i = 0; i < algebraTraits<MV>::displayBasis.size(); i++)
            if (bitmapGrade(algebraTraits<MV>::displayBasis[i].bitmap) == 1)
                add(algebraTraits<MV>::displayBasis[i].name, basisBlade<MV>(i));
    }

    // adds a name (returns false if it is already known)
    bool add(const std::string &name, const MV &value)
    {
        MV tmp;
        if (lookup(name, tmp))
            return false;
        m_names.emplace_back(name, value);
        return true;
    }

    bool lookup(const std::string &name, MV &value) const
    {
        for (const auto &n : m_names)
            if (n.first == name)
            {
                value = n.second;
                return true;
            }
        return false;
    }

    char m_startDelimiter, m_endDelimiter;

  private:
    std::vector<std::pair<std::string, MV>> m_names;
};

// Parses a multivector, e.g. "1.0*e1 + -2.5*e2^e3" (or "[...]" with delimiters):
// sets 'result' and returns the number of characters read, or -1 on error.
template <garamonMV MV> int parse(const char *str, MV &result, const basisNames<MV> &names = basisNames<MV>())
{
    auto isFloatChar = [](char c) { return (c != 0) && std::strchr("0123456789.", c); };
    int idx = 0, beCnt = 0;
    result = MV();

    if (names.m_startDelimiter)
    {
        // the first character found must be the start delimiter
        while (str[idx] <= ' ')
        {
            if (str[idx] == 0)
                return -1; // nothing read
            idx++;
        }
        if (str[idx] != names.m_startDelimiter)
            return -1;
        idx++;
    }

    for (;;)
    {
        MV blade(1.0);
        double scalar = 1.0;

        while (str[idx] <= ' ')
        {
            if (str[idx] == 0)
                return (names.m_endDelimiter) ? -1 : idx; // done or error
            idx++;
        }

        int pmCnt = 0; // parse [+|-]+
        for (;;)
        {
            if (str[idx] == '+')
                pmCnt++;
            else if (str[idx] == '-')
            {
                scalar = -scalar;
                pmCnt++;
            }
            else if (isFloatChar(str[idx]))
                break; // start of the float
            else if ((str[idx] > 0) && (str[idx] <= ' '))
                ; // skip space
            else
                break;
            idx++;
        }
        if (beCnt && (!pmCnt))
            return -1;

        if (isFloatChar(str[idx]))
        { // parse the float and '*' or '^'
            int floatStart = idx;
            while ((str[idx] != 0 && std::strchr("0123456789.eE", str[idx])) ||
                   (idx && ((str[idx - 1] == 'e') || (str[idx - 1] == 'E')) && ((str[idx] == '-') || (str[idx] == '+'))))
                idx++;
            if (idx - floatStart <= 0)
                return -1;
            scalar *= std::atof(std::string(str + floatStart, idx - floatStart).c_str());

            while (str[idx] <= ' ')
            {
                if (str[idx] == 0)
                {
                    result += MV(scalar);
                    return (names.m_endDelimiter) ? -1 : idx; // done or error
                }
                idx++;
            }

            // the next char must be +, -, *, ^ or the end delimiter
            if ((str[idx] == '+') || (str[idx] == '-'))
            {
                result += MV(scalar);
                beCnt++;
                continue;
            }
            else if (names.m_endDelimiter && (str[idx] == names.m_endDelimiter))
            {
                result += MV(scalar);
                return idx + 1;
            }
            else if ((str[idx] != '*') && (str[idx] != '^'))
                return -1;
            else
                idx++; // skip over '*' or '^'

            while (str[idx] <= ' ')
            {
                if (str[idx] == 0)
                    return -1;
                idx++;
            }
        }

        for (;;)
        { // parse a basis element ex ^ ey ^ ez
            int nameStart = idx;
            while ((str[idx] > ' ') && (str[idx] != '-') && (str[idx] != '+') && (str[idx] != '^') &&
                   (str[idx] != names.m_endDelimiter))
                idx++;
            MV be;
            if (!names.lookup(std::string(str + nameStart, idx - nameStart), be))
                return -1; // invalid basis vector name

            blade = blade ^ be; // wedge the basis element to the blade

            while (str[idx] <= ' ')
            {
                if ((str[idx] == 0) || (str[idx] == names.m_endDelimiter))
                    break;
                idx++;
            }

            // the next char must be a wedge, or this is the end of the blade
            if (str[idx] != '^')
                break;
            idx++;
        }

        result += blade * scalar;
        beCnt++;

        if (str[idx] == names.m_endDelimiter)
            return idx + 1;
    }
}

} // namespace ga

#endif /* _GA_FORMAT_H_ */
