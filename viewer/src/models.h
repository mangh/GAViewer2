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

// Copyright 2002-2010, Daniel Fontijne, University of Amsterdam -- fontijne@science.uva.nl

#ifndef _MODELS_H_
#define _MODELS_H_

// which model the multivector came from:
#define MVI_E3GA (1 << 13)
#define MVI_P3GA (2 << 13)
#define MVI_C3GA (3 << 13)
#define MVI_I2GA (4 << 13)
#define MVI_C5GA (5 << 13)
#define MVI_MODEL_MASK (7 << 13)

namespace e3ga // Garamon (see ga/e3ga.h)
{
template <typename T> class Mvec;
using mv = Mvec<double>;
} // namespace e3ga
namespace p3ga // Garamon (see ga/p3ga.h)
{
template <typename T> class Mvec;
using mv = Mvec<double>;
} // namespace p3ga
namespace c3ga // Garamon (see ga/c3ga.h)
{
template <typename T> class Mvec;
using mv = Mvec<double>;
} // namespace c3ga
class c5ga;
class i2ga;

// the model id (MVI_XXX) of multivector type MV
template <class MV> constexpr int modelId = 0; // (not an algebra)
template <> inline constexpr int modelId<e3ga::mv> = MVI_E3GA;
template <> inline constexpr int modelId<p3ga::mv> = MVI_P3GA;
template <> inline constexpr int modelId<c3ga::mv> = MVI_C3GA;
template <> inline constexpr int modelId<c5ga> = MVI_C5GA;
template <> inline constexpr int modelId<i2ga> = MVI_I2GA;

// the name of a model ("e3ga", ...), or NULL if 'model' is not a model id
inline const char *modelName(int model)
{
    switch (model)
    {
    case MVI_E3GA:
        return "e3ga";
    case MVI_P3GA:
        return "p3ga";
    case MVI_C3GA:
        return "c3ga";
    case MVI_C5GA:
        return "c5ga";
    case MVI_I2GA:
        return "i2ga";
    default:
        return 0;
    }
}
// more to come???

#endif /* _MODELS_H_ */
