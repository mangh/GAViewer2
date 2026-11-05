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
about each of them: its model id (MVI_XXX), its name, how a console variable is
cast to it and how its multivector is accessed.

This is the one place where the console binds a model id to a multivector type:
code that is the same for every algebra is written as a template over the
multivector type MV (see consolealgebrafunctions.cpp) and uses
consoleAlgebra<MV> and mv<MV>() only.
*/

#include "../gaincludes.h" // IWYU pragma: export
#include "../models.h"     // IWYU pragma: export
#include "../state.h"
#include "consolevariable.h"

template <class MV> struct consoleAlgebra; // specialized for every algebra below

template <> struct consoleAlgebra<e3ga>
{
    static constexpr int model = MVI_E3GA;
    static constexpr const char *castName = "cast_e3ga";
    static e3ga &get(const consoleVariable *cv)
    {
        return cv->e();
    }
    static consoleVariable *castTo(const consoleVariable *cv)
    {
        return cv->castToE3ga();
    }
    static int setFactor(const e3ga &mv, int idx)
    {
        return g_state->setE3gaFactor(mv, idx);
    }
};

template <> struct consoleAlgebra<p3ga>
{
    static constexpr int model = MVI_P3GA;
    static constexpr const char *castName = "cast_p3ga";
    static p3ga &get(const consoleVariable *cv)
    {
        return cv->p();
    }
    static consoleVariable *castTo(const consoleVariable *cv)
    {
        return cv->castToP3ga();
    }
    static int setFactor(const p3ga &mv, int idx)
    {
        return g_state->setP3gaFactor(mv, idx);
    }
};

template <> struct consoleAlgebra<c3ga>
{
    static constexpr int model = MVI_C3GA;
    static constexpr const char *castName = "cast_c3ga";
    static c3ga &get(const consoleVariable *cv)
    {
        return cv->c();
    }
    static consoleVariable *castTo(const consoleVariable *cv)
    {
        return cv->castToC3ga();
    }
    static int setFactor(const c3ga &mv, int idx)
    {
        return g_state->setC3gaFactor(mv, idx);
    }
};

template <> struct consoleAlgebra<c5ga>
{
    static constexpr int model = MVI_C5GA;
    static constexpr const char *castName = "cast_c5ga";
    static c5ga &get(const consoleVariable *cv)
    {
        return cv->c5();
    }
    static consoleVariable *castTo(const consoleVariable *cv)
    {
        return cv->castToC5ga();
    }
    static int setFactor(const c5ga &mv, int idx)
    {
        return g_state->setC5gaFactor(mv, idx);
    }
};

template <> struct consoleAlgebra<i2ga>
{
    static constexpr int model = MVI_I2GA;
    static constexpr const char *castName = "cast_i2ga";
    static i2ga &get(const consoleVariable *cv)
    {
        return cv->i2();
    }
    static consoleVariable *castTo(const consoleVariable *cv)
    {
        return cv->castToI2ga();
    }
    static int setFactor(const i2ga &mv, int idx)
    {
        return g_state->setI2gaFactor(mv, idx);
    }
};

// the multivector of console variable 'cv', which must be of model consoleAlgebra<MV>::model
template <class MV> inline MV &mv(const consoleVariable *cv)
{
    return consoleAlgebra<MV>::get(cv);
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
