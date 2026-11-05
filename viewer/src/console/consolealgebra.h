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

#ifndef _CONSOLE_ALGEBRA_H_
#define _CONSOLE_ALGEBRA_H_

/*
The algebras (models) known to the console, and what the console needs to know
about each of them: its model id (MVI_XXX), the name of its cast function, how
a console variable is cast to it, how its factors are set and how an object is
created from it. (The multivector of a console variable is accessed with
consoleVariable::mv<MV>(); its model id is modelId<MV>, see models.h.)

This is the one place where the console binds a model id to a multivector type:
code that is the same for every algebra is written as a template over the
multivector type MV (see consolealgebrafunctions.cpp) and uses
consoleAlgebra<MV> and mv<MV>() only.
*/

#include <string>

#include "../gaincludes.h" // IWYU pragma: export
#include "../models.h"     // IWYU pragma: export
#include "../state.h"
#include "consolevariable.h"

template <class MV> struct consoleAlgebra; // specialized for every algebra below

template <> struct consoleAlgebra<e3ga>
{
    static constexpr int model = modelId<e3ga>;
    static constexpr const char *castName = "cast_e3ga";
    static consoleVariable *castTo(const consoleVariable *cv)
    {
        return cv->castToE3ga();
    }
    static int setFactor(const e3ga &mv, int idx)
    {
        return g_state->setE3gaFactor(mv, idx);
    }
    static int addObject(const e3ga &mv, const std::string &name, int drawMode, int creationFlags, int forceFlags)
    {
        return g_state->addE3gaObject(mv, name, drawMode, creationFlags, forceFlags);
    }
};

template <> struct consoleAlgebra<p3ga>
{
    static constexpr int model = modelId<p3ga>;
    static constexpr const char *castName = "cast_p3ga";
    static consoleVariable *castTo(const consoleVariable *cv)
    {
        return cv->castToP3ga();
    }
    static int setFactor(const p3ga &mv, int idx)
    {
        return g_state->setP3gaFactor(mv, idx);
    }
    static int addObject(const p3ga &mv, const std::string &name, int drawMode, int creationFlags, int forceFlags)
    {
        return g_state->addP3gaObject(mv, name, drawMode, creationFlags, forceFlags);
    }
};

template <> struct consoleAlgebra<c3ga>
{
    static constexpr int model = modelId<c3ga>;
    static constexpr const char *castName = "cast_c3ga";
    static consoleVariable *castTo(const consoleVariable *cv)
    {
        return cv->castToC3ga();
    }
    static int setFactor(const c3ga &mv, int idx)
    {
        return g_state->setC3gaFactor(mv, idx);
    }
    static int addObject(const c3ga &mv, const std::string &name, int drawMode, int creationFlags, int forceFlags)
    {
        return g_state->addC3gaObject(mv, name, drawMode, creationFlags, forceFlags);
    }
};

template <> struct consoleAlgebra<c5ga>
{
    static constexpr int model = modelId<c5ga>;
    static constexpr const char *castName = "cast_c5ga";
    static consoleVariable *castTo(const consoleVariable *cv)
    {
        return cv->castToC5ga();
    }
    static int setFactor(const c5ga &mv, int idx)
    {
        return g_state->setC5gaFactor(mv, idx);
    }
    static int addObject(const c5ga &mv, const std::string &name, int drawMode, int creationFlags, int forceFlags)
    {
        return g_state->addC5gaObject(mv, name, drawMode, creationFlags, forceFlags);
    }
};

template <> struct consoleAlgebra<i2ga>
{
    static constexpr int model = modelId<i2ga>;
    static constexpr const char *castName = "cast_i2ga";
    static consoleVariable *castTo(const consoleVariable *cv)
    {
        return cv->castToI2ga();
    }
    static int setFactor(const i2ga &mv, int idx)
    {
        return g_state->setI2gaFactor(mv, idx);
    }
    static int addObject(const i2ga &mv, const std::string &name, int drawMode, int creationFlags, int forceFlags)
    {
        return g_state->addI2gaObject(mv, name, drawMode, creationFlags, forceFlags);
    }
};

// the multivector of console variable 'cv', which must be of model consoleAlgebra<MV>::model
template <class MV> inline MV &mv(const consoleVariable *cv)
{
    return cv->mv<MV>();
}

// Calls f.template operator()<MV>() for the algebra MV with model id 'model';
// returns false (and does not call f) if 'model' is not a model id.
template <class F> inline bool withConsoleAlgebra(int model, F &&f)
{
    switch (model)
    {
    case MVI_E3GA:
        f.template operator()<e3ga>();
        return true;
    case MVI_P3GA:
        f.template operator()<p3ga>();
        return true;
    case MVI_C3GA:
        f.template operator()<c3ga>();
        return true;
    case MVI_C5GA:
        f.template operator()<c5ga>();
        return true;
    case MVI_I2GA:
        f.template operator()<i2ga>();
        return true;
    default:
        return false;
    }
}

// Calls f.template operator()<MV>() for every algebra, in the order of the model ids
// (e3ga, p3ga, c3ga, c5ga, i2ga); this order is also the order of the built-in
// function table.
template <class F> inline void forEachConsoleAlgebra(F &&f)
{
    f.template operator()<e3ga>();
    f.template operator()<p3ga>();
    f.template operator()<c3ga>();
    f.template operator()<c5ga>();
    f.template operator()<i2ga>();
}

#endif /* _CONSOLE_ALGEBRA_H_ */
