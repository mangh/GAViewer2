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

// Copyright 2002-2009, Daniel Fontijne, University of Amsterdam -- fontijne@science.uva.nl

#include <cmath>
#include <cstdio>
#include <cstring>

#include <string>
#include <type_traits>
#include <variant>

#include "../ga/cast.h"
#include "../gaincludes.h"
#include "../models.h"
#include "../mvint.h"
#include "../object.h"
#include "../state.h"
#include "console.h"
#include "consolevariable.h"
// #include "consoleparse.h"

int consoleVariable::m_tempVarCount = 1;

consoleVariable::consoleVariable(const std::string &pname, const consoleVariable *cv)
{
    initToNothing();
    cv->visitMV([&](const auto &mv) { return setMV((pname.length() > 0) ? pname : cv->name(), mv); }, 0);

    // copy other properties
    m_op = cv->op();
    m_flags = cv->m_flags;
    m_drawMode =
        cv->m_drawMode ^ (cv->m_drawMode & (((cv->m_forceFlags & OFF_HIDE) ? 0 : OD_HIDE) |
                                            OD_HIDE_ALWAYS)); // never inherit hide (unless forced) or hide always
    m_creationFlags = cv->m_creationFlags;
    m_forceFlags = cv->m_forceFlags;
    int i;
    for (i = 0; i < 4; i++)
        m_color[i] = cv->m_color[i];
}

consoleVariable::consoleVariable(const std::string &name, const e3ga::mv *mv)
{
    initToNothing();
    set(name, mv);
}

consoleVariable::consoleVariable(const std::string &name, const p3ga::mv *mv)
{
    initToNothing();
    set(name, mv);
}

consoleVariable::consoleVariable(const std::string &name, const c3ga::mv *mv)
{
    initToNothing();
    set(name, mv);
}

consoleVariable::consoleVariable(const std::string &name, const c5ga::mv *mv)
{
    initToNothing();
    set(name, mv);
}

consoleVariable::consoleVariable(const std::string &name, const i2ga::mv *mv)
{
    initToNothing();
    set(name, mv);
}

consoleVariable::consoleVariable(const std::string &name, const e3ga::mv &mv)
{
    initToNothing();
    set(name, &mv);
}

consoleVariable::consoleVariable(const std::string &name, const p3ga::mv &mv)
{
    initToNothing();
    set(name, &mv);
}

consoleVariable::consoleVariable(const std::string &name, const c3ga::mv &mv)
{
    initToNothing();
    set(name, &mv);
}

consoleVariable::consoleVariable(const std::string &name, const c5ga::mv &mv)
{
    initToNothing();
    set(name, &mv);
}

consoleVariable::consoleVariable(const std::string &name, const i2ga::mv &mv)
{
    initToNothing();
    set(name, &mv);
}

consoleVariable::consoleVariable(const consoleVariable &cv)
{
    initToNothing();
    set(cv);
}

consoleVariable::~consoleVariable()
{
}

void consoleVariable::initToNothing()
{
    m_mv = std::monostate();
    m_op = 0;
    m_flags = 0;
    m_next = NULL;

    m_creationFlags = 0;
    m_drawMode = 0;
    m_forceFlags = 0;
    int i;
    for (i = 0; i < 4; i++)
        m_color[i] = 0.0f;
}

consoleVariable &consoleVariable::operator=(const consoleVariable &cv)
{
    if (&cv != this)
        set(cv);
    return *this;
}

void consoleVariable::set(const consoleVariable &cv)
{
    name(cv.name());

    m_op = cv.op();
    m_flags = cv.m_flags;

    m_mv = cv.m_mv;

    m_drawMode = cv.drawMode();
    m_forceFlags = cv.forceFlags();
    color4(cv.color());
}

// sets the name and the multivector, and the default draw mode and color
template <class MV> int consoleVariable::setMV(const std::string &pname, const MV &mv)
{
    int err;
    if ((err = name(pname)))
        return err;

    int i;
    m_drawMode = OD_MAGNITUDE; //| OD_ORI;
    for (i = 0; i < 4; i++)
        m_color[i] = g_state->m_fgColor[i];

    m_mv = mv;

    return 0;
}

int consoleVariable::set(const std::string &pname, const e3ga::mv *mv)
{
    return setMV(pname, *mv);
}

int consoleVariable::set(const std::string &pname, const p3ga::mv *mv)
{
    return setMV(pname, *mv);
}

int consoleVariable::set(const std::string &pname, const c3ga::mv *mv)
{
    return setMV(pname, *mv);
}

int consoleVariable::set(const std::string &pname, const c5ga::mv *mv)
{
    return setMV(pname, *mv);
}

int consoleVariable::set(const std::string &pname, const i2ga::mv *mv)
{
    return setMV(pname, *mv);
}

int consoleVariable::model() const
{
    return visitMV([](const auto &mv) { return modelId<std::remove_cvref_t<decltype(mv)>>; }, 0);
}

mvInt *consoleVariable::interpretation() const
{
    return visitMV([](const auto &mv) { return new mvInt(mv); }, (mvInt *)NULL);
}

int consoleVariable::grade() const
{
    int grade = -1;
    int type = visitMV([&](const auto &mv) { return ga::mvType(mv, &grade); }, 0);

    if (type == ga::mvTypeBlade)
        grade = (int)(log((double)grade) / log((double)2) + 0.45);
    else
        grade = -1;

    return grade;
}

int consoleVariable::print(const char *str) const
{
    if (!visitMV(
            [&](const auto &mv) {
                printf("%s%s\n", (str) ? str : "", ga::toString(mv, "%f").c_str());
                return true;
            },
            false))
        printf("%s: unknown model\n", str);
    return 0;
}

std::string consoleVariable::string() const
{
    return visitMV([](const auto &mv) { return ga::toString(mv, "%e"); }, std::string("unknown model"));
}

int consoleVariable::isTrue() const
{
    return visitMV([](const auto &mv) { return (int)(ga::norm_a(mv) != 0.0); }, 0);
}

double consoleVariable::scalar() const
{
    return visitMV([](const auto &mv) { return (double)ga::scalar(mv); }, (double)0);
}

int consoleVariable::name(const std::string &name)
{
    /*	char tmpName[64];
        if (m_name) free(m_name);

        if (name == NULL) {
            sprintf(tmpName, "temporary console variable %d", m_tempVarCount++);
            name = tmpName;
        }

        return ((m_name = strdup(name)) == NULL) ? -1 : 0;*/
    m_name = name;
    return 0;
}

const std::string &consoleVariable::name() const
{
    return m_name;
}

int consoleVariable::nameEquals(const std::string &name) const
{
    /*if ((pname == NULL) || (name() == NULL)) return 0;
    return (!strcmp(pname, name()));*/
    return (m_name == name);
}

int consoleVariable::op() const
{
    //	printf("Get cv: %08X, op = %d\n", this, m_op);
    return m_op;
}

int consoleVariable::op(int o)
{
    m_op = o;
    //	printf("Set cv: %08X, op = %d\n", this, m_op);
    return 0;
}

int consoleVariable::getFlag(int flag) const
{
    return (m_flags & flag);
}

int consoleVariable::setFlag(int flag, int val)
{
    m_flags = (m_flags ^ (m_flags & flag)) | (val ? flag : 0);
    return 0;
}

/*int consoleVariable::constant(int c) {
    return setFlag(CVF_CONSTANT, c);
}
int consoleVariable::constant() const {
    return getFlag(CVF_CONSTANT);
}*/
int consoleVariable::rhs(int c)
{
    return setFlag(CVF_RHS, c);
}
int consoleVariable::rhs() const
{
    return getFlag(CVF_RHS);
}
int consoleVariable::lhs(int c)
{
    return setFlag(CVF_LHS, c);
}
int consoleVariable::lhs() const
{
    return getFlag(CVF_LHS);
}

int consoleVariable::global(int g)
{
    return setFlag(CVF_GLOBAL, g);
}
int consoleVariable::global() const
{
    return getFlag(CVF_GLOBAL);
}

int consoleVariable::functionArgument(int g)
{
    return setFlag(CVF_FUNCTION_ARGUMENT, g);
}
int consoleVariable::functionArgument() const
{
    return getFlag(CVF_FUNCTION_ARGUMENT);
}

int consoleVariable::drawMode() const
{
    return m_drawMode;
}

int consoleVariable::drawMode(int mode)
{
    m_drawMode = mode;
    return 0;
}

int consoleVariable::creationFlags() const
{
    return m_creationFlags;
}

int consoleVariable::creationFlags(int flags)
{
    m_creationFlags = flags;
    return 0;
}

int consoleVariable::forceFlags() const
{
    return m_forceFlags;
}

int consoleVariable::modifiedColor()
{
    m_forceFlags |= OFF_COLOR;
    return 0;
}
int consoleVariable::modifiedAlpha()
{
    m_forceFlags |= OFF_ALPHA;
    return 0;
}
int consoleVariable::modifiedDrawMethod()
{
    m_forceFlags |= OFF_DRAWMETHOD;
    return 0;
}
int consoleVariable::modifiedHide()
{
    m_forceFlags |= OFF_HIDE;
    return 0;
}
int consoleVariable::modifiedStipple()
{
    m_forceFlags |= OFF_STIPPLE;
    return 0;
}
int consoleVariable::modifiedOri()
{
    m_forceFlags |= OFF_ORI;
    return 0;
}
int consoleVariable::modifiedMute()
{
    m_forceFlags |= OFF_MUTE;
    return 0;
}
int consoleVariable::modifiedWireframe()
{
    m_forceFlags |= OFF_WIREFRAME;
    return 0;
}
int consoleVariable::modifiedOutline()
{
    m_forceFlags |= OFF_OUTLINE;
    return 0;
}
int consoleVariable::modifiedMagnitude()
{
    m_forceFlags |= OFF_MAGNITUDE;
    return 0;
}
int consoleVariable::modifiedShade()
{
    m_forceFlags |= OFF_SHADE;
    return 0;
}

const float *consoleVariable::color() const
{
    return m_color;
}

int consoleVariable::color4(float r, float g, float b, float a)
{
    m_color[0] = r;
    m_color[1] = g;
    m_color[2] = b;
    m_color[3] = a;
    return 0;
}

int consoleVariable::color4(const float c[4])
{
    m_color[0] = c[0];
    m_color[1] = c[1];
    m_color[2] = c[2];
    m_color[3] = c[3];
    return 0;
}

int consoleVariable::color3(float r, float g, float b)
{
    m_color[0] = r;
    m_color[1] = g;
    m_color[2] = b;
    return 0;
}

int consoleVariable::color3(const float c[3])
{
    m_color[0] = c[0];
    m_color[1] = c[1];
    m_color[2] = c[2];
    return 0;
}

int consoleVariable::alpha(float alpha)
{
    m_color[3] = alpha;
    return 0;
}

float consoleVariable::alpha() const
{
    return m_color[3];
}

void consoleVariable::next(consoleVariable *pnext)
{
    m_next = pnext;
}

consoleVariable *consoleVariable::next()
{
    return m_next;
}

// The multivector cast to the algebra To (see ga/cast.h); a copy if it is of that algebra already.
// (These were 20 hand-written functions, eight of which mixed up coordinates.)
template <class To> static consoleVariable *castTo(const consoleVariable &v)
{
    if (v.model() == modelId<To>)
        return new consoleVariable("", v.mv<To>());
    return v.visitMV(
        [](const auto &mv) { return new consoleVariable("", ga::compress(ga::castByVectors<To>(mv))); },
        (consoleVariable *)NULL);
}

consoleVariable *consoleVariable::castToE3ga() const
{
    return castTo<e3ga::mv>(*this);
}

consoleVariable *consoleVariable::castToP3ga() const
{
    return castTo<p3ga::mv>(*this);
}

consoleVariable *consoleVariable::castToC3ga() const
{
    return castTo<c3ga::mv>(*this);
}

consoleVariable *consoleVariable::castToC5ga() const
{
    return castTo<c5ga::mv>(*this);
}

consoleVariable *consoleVariable::castToI2ga() const
{
    return castTo<i2ga::mv>(*this);
}

void *consoleVariableScalar(const char *name, double val)
{
    e3ga::mv tmp(val);
    consoleVariable *cv = new consoleVariable(name, tmp);
    cv->rhs(1);
    return cv;
}

void consoleVariableSetLhs(void *cv, int v)
{
    ((consoleVariable *)cv)->lhs(v);
}

void consoleVariableSetRhs(void *cv, int v)
{
    ((consoleVariable *)cv)->rhs(v);
}

int consoleVariableSetOperator(void *cv, int op)
{
    return ((consoleVariable *)cv)->op(op);
}

int consolePrintVariableModel(void *cv)
{
    printf("consolePrintVariableType: %08X\n", ((consoleVariable *)cv)->model());
    return 0;
}

void consoleVariableFree(void *cv)
{
    if (cv)
    {
        delete (consoleVariable *)cv;
    }
}
