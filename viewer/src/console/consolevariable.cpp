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

#include "../ga/gaigencompat.h" // the ga:: operations also for the Gaigen algebras
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

consoleVariable::consoleVariable(const std::string &name, const c5ga *mv)
{
    initToNothing();
    set(name, mv);
}

consoleVariable::consoleVariable(const std::string &name, const i2ga *mv)
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

consoleVariable::consoleVariable(const std::string &name, const c5ga &mv)
{
    initToNothing();
    set(name, &mv);
}

consoleVariable::consoleVariable(const std::string &name, const i2ga &mv)
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

int consoleVariable::set(const std::string &pname, const c5ga *mv)
{
    return setMV(pname, *mv);
}

int consoleVariable::set(const std::string &pname, const i2ga *mv)
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

GAIM_FLOAT consoleVariable::scalar() const
{
    return visitMV([](const auto &mv) { return (GAIM_FLOAT)ga::scalar(mv); }, (GAIM_FLOAT)0);
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

consoleVariable *consoleVariable::castE3gaToP3ga() const
{
    p3ga::mv vp;
    const e3ga::mv &ve = e();
    vp =
        // grade 0
        ga::scalar(ve) +
        // grade 1
        ve[e3ga::E1] * p3ga::e1<double>() + ve[e3ga::E2] * p3ga::e2<double>() + ve[e3ga::E3] * p3ga::e3<double>() +
        // grade 2
        ve[e3ga::E12] * (p3ga::e1<double>() ^ p3ga::e2<double>()) + ve[e3ga::E23] * (p3ga::e2<double>() ^ p3ga::e3<double>()) +
        (-ve[e3ga::E13]) * (p3ga::e3<double>() ^ p3ga::e1<double>()) +
        // grade 3
        ve[e3ga::E123] * (p3ga::e1<double>() ^ p3ga::e2<double>() ^ p3ga::e3<double>());

    vp = ga::compress(vp);
    return new consoleVariable("", vp);
}

consoleVariable *consoleVariable::castE3gaToC3ga() const
{
    c3ga::mv vc;
    const e3ga::mv &ve = e();
    vc =
        // grade 0
        ga::scalar(ve) +
        // grade 1
        ve[e3ga::E1] * c3ga::e1<double>() + ve[e3ga::E2] * c3ga::e2<double>() + ve[e3ga::E3] * c3ga::e3<double>() +
        // grade 2
        ve[e3ga::E12] * (c3ga::e1<double>() ^ c3ga::e2<double>()) + ve[e3ga::E23] * (c3ga::e2<double>() ^ c3ga::e3<double>()) +
        (-ve[e3ga::E13]) * (c3ga::e3<double>() ^ c3ga::e1<double>()) +
        // grade 3
        ve[e3ga::E123] * (c3ga::e1<double>() ^ c3ga::e2<double>() ^ c3ga::e3<double>());

    vc = ga::compress(vc);
    return new consoleVariable("", vc);
}

consoleVariable *consoleVariable::castE3gaToC5ga() const
{
    c5ga vc;
    const e3ga::mv &ve = e();
    vc =
        // grade 0
        ga::scalar(ve) +
        // grade 1
        ve[e3ga::E1] * c5ga::e1 + ve[e3ga::E2] * c5ga::e2 + ve[e3ga::E3] * c5ga::e3 +
        // grade 2
        ve[e3ga::E12] * (c5ga::e1 ^ c5ga::e2) + ve[e3ga::E23] * (c5ga::e2 ^ c5ga::e3) +
        (-ve[e3ga::E13]) * (c5ga::e3 ^ c5ga::e1) +
        // grade 3
        ve[e3ga::E123] * (c5ga::e1 ^ c5ga::e2 ^ c5ga::e3);

    vc.compress();
    return new consoleVariable("", vc);
}

consoleVariable *consoleVariable::castP3gaToC3ga() const
{
    c3ga::mv vc;
    const p3ga::mv &vp = p();
    vc =
        // grade 0
        ga::scalar(vp) +
        // grade 1
        vp[p3ga::E1] * c3ga::e1<double>() + vp[p3ga::E2] * c3ga::e2<double>() + vp[p3ga::E3] * c3ga::e3<double>() +
        vp[p3ga::E0] * c3ga::eo<double>() +
        // grade 2
        vp[p3ga::E12] * (c3ga::e1<double>() ^ c3ga::e2<double>()) + vp[p3ga::E23] * (c3ga::e2<double>() ^ c3ga::e3<double>()) +
        (-vp[p3ga::E13]) * (c3ga::e3<double>() ^ c3ga::e1<double>()) + vp[p3ga::E10] * (c3ga::e1<double>() ^ c3ga::eo<double>()) +
        vp[p3ga::E20] * (c3ga::e2<double>() ^ c3ga::eo<double>()) + vp[p3ga::E30] * (c3ga::e3<double>() ^ c3ga::eo<double>()) +
        // grade 3
        vp[p3ga::E230] * (c3ga::e2<double>() ^ c3ga::e3<double>() ^ c3ga::eo<double>()) +
        (-vp[p3ga::E130]) * (c3ga::e3<double>() ^ c3ga::e1<double>() ^ c3ga::eo<double>()) +
        vp[p3ga::E120] * (c3ga::e1<double>() ^ c3ga::e2<double>() ^ c3ga::eo<double>()) +
        vp[p3ga::E123] * (c3ga::e1<double>() ^ c3ga::e2<double>() ^ c3ga::e3<double>()) +
        // grade 4
        vp[p3ga::E1230] * (c3ga::e1<double>() ^ c3ga::e2<double>() ^ c3ga::e3<double>() ^ c3ga::eo<double>());

    vc = ga::compress(vc);
    return new consoleVariable("", vc);
}
consoleVariable *consoleVariable::castP3gaToC5ga() const
{
    c5ga vc;
    const p3ga::mv &vp = p();
    vc =
        // grade 0
        ga::scalar(vp) +
        // grade 1
        vp[p3ga::E1] * c5ga::e1 + vp[p3ga::E2] * c5ga::e2 + vp[p3ga::E3] * c5ga::e3 +
        vp[p3ga::E0] * c5ga::no +
        // grade 2
        vp[p3ga::E12] * (c5ga::e1 ^ c5ga::e2) + vp[p3ga::E23] * (c5ga::e2 ^ c5ga::e3) +
        (-vp[p3ga::E13]) * (c5ga::e3 ^ c5ga::e1) + vp[p3ga::E10] * (c5ga::e1 ^ c5ga::no) +
        vp[p3ga::E20] * (c5ga::e2 ^ c5ga::no) + vp[p3ga::E30] * (c5ga::e3 ^ c5ga::no) +
        // grade 3
        vp[p3ga::E230] * (c5ga::e2 ^ c5ga::e3 ^ c5ga::no) +
        (-vp[p3ga::E130]) * (c5ga::e3 ^ c5ga::e1 ^ c5ga::no) +
        vp[p3ga::E120] * (c5ga::e1 ^ c5ga::e2 ^ c5ga::no) +
        vp[p3ga::E123] * (c5ga::e1 ^ c5ga::e2 ^ c5ga::e3) +
        // grade 4
        vp[p3ga::E1230] * (c5ga::e1 ^ c5ga::e2 ^ c5ga::e3 ^ c5ga::no);

    vc.compress();
    return new consoleVariable("", vc);
}

consoleVariable *consoleVariable::castP3gaToE3ga() const
{
    e3ga::mv ve;
    const p3ga::mv &vp = p();
    ve =
        // grade 0
        ga::scalar(vp) +
        // grade 1
        vp[p3ga::E1] * e3ga::e1<double>() + vp[p3ga::E2] * e3ga::e2<double>() + vp[p3ga::E3] * e3ga::e3<double>() +
        // grade 2
        vp[p3ga::E12] * (e3ga::e1<double>() ^ e3ga::e2<double>()) + vp[p3ga::E23] * (e3ga::e2<double>() ^ e3ga::e3<double>()) +
        (-vp[p3ga::E13]) * (e3ga::e3<double>() ^ e3ga::e1<double>()) +
        // grade 3
        vp[p3ga::E123] * (e3ga::e1<double>() ^ e3ga::e2<double>() ^ e3ga::e3<double>());

    ve = ga::compress(ve);
    return new consoleVariable("", ve);
}

consoleVariable *consoleVariable::castC3gaToE3ga() const
{
    e3ga::mv ve;
    const c3ga::mv &vc = c();
    ve =
        // grade 0
        ga::scalar(vc) +
        // grade 1
        vc[c3ga::E1] * e3ga::e1<double>() + vc[c3ga::E2] * e3ga::e2<double>() + vc[c3ga::E3] * e3ga::e3<double>() +
        // grade 2
        vc[c3ga::E12] * (e3ga::e1<double>() ^ e3ga::e2<double>()) + vc[c3ga::E23] * (e3ga::e2<double>() ^ e3ga::e3<double>()) +
        vc[c3ga::E13] * (e3ga::e1<double>() ^ e3ga::e3<double>()) +
        // grade 3
        vc[c3ga::E123] * (e3ga::e1<double>() ^ e3ga::e2<double>() ^ e3ga::e3<double>());

    ve = ga::compress(ve);
    return new consoleVariable("", ve);
}

consoleVariable *consoleVariable::castC5gaToE3ga() const
{
    e3ga::mv ve;
    const c5ga &vc = c5();
    ve =
        // grade 0
        vc.scalar() +
        // grade 1
        vc[GRADE1][C5GA_E1] * e3ga::e1<double>() + vc[GRADE1][C5GA_E2] * e3ga::e2<double>() + vc[GRADE1][C5GA_E3] * e3ga::e3<double>() +
        // grade 2
        vc[GRADE2][C5GA_E1_E2] * (e3ga::e1<double>() ^ e3ga::e2<double>()) + vc[GRADE2][C5GA_E2_E3] * (e3ga::e2<double>() ^ e3ga::e3<double>()) +
        vc[GRADE2][C5GA_E1_E3] * (e3ga::e1<double>() ^ e3ga::e3<double>()) +
        // grade 3
        vc[GRADE3][C5GA_E1_E2_E3] * (e3ga::e1<double>() ^ e3ga::e2<double>() ^ e3ga::e3<double>());

    ve = ga::compress(ve);
    return new consoleVariable("", ve);
}

consoleVariable *consoleVariable::castC3gaToP3ga() const
{
    p3ga::mv vp;
    const c3ga::mv &vc = c();
    vp =
        // grade 0
        ga::scalar(vc) +
        // grade 1
        vc[c3ga::E1] * p3ga::e1<double>() + vc[c3ga::E2] * p3ga::e2<double>() + vc[c3ga::E3] * p3ga::e3<double>() +
        vc[c3ga::Eo] * p3ga::e0<double>() +
        // grade 2
        // NOTE: inherited bug, kept as is: the e1^e2 term is added twice and the e1^e3 term is missing
        vc[c3ga::E12] * (p3ga::e1<double>() ^ p3ga::e2<double>()) + vc[c3ga::E23] * (p3ga::e2<double>() ^ p3ga::e3<double>()) +
        vc[c3ga::E12] * (p3ga::e1<double>() ^ p3ga::e2<double>()) + vc[c3ga::E1o] * (p3ga::e1<double>() ^ p3ga::e0<double>()) +
        vc[c3ga::E2o] * (p3ga::e2<double>() ^ p3ga::e0<double>()) + vc[c3ga::E3o] * (p3ga::e3<double>() ^ p3ga::e0<double>()) +
        // grade 3
        vc[c3ga::E23o] * (p3ga::e2<double>() ^ p3ga::e3<double>() ^ p3ga::e0<double>()) +
        vc[c3ga::E13o] * (p3ga::e1<double>() ^ p3ga::e3<double>() ^ p3ga::e0<double>()) +
        vc[c3ga::E12o] * (p3ga::e1<double>() ^ p3ga::e2<double>() ^ p3ga::e0<double>()) +
        vc[c3ga::E123] * (p3ga::e1<double>() ^ p3ga::e2<double>() ^ p3ga::e3<double>()) +
        // grade 4
        vc[c3ga::E123o] * (p3ga::e1<double>() ^ p3ga::e2<double>() ^ p3ga::e3<double>() ^ p3ga::e0<double>());

    vp = ga::compress(vp);
    return new consoleVariable("", vp);
}

consoleVariable *consoleVariable::castC5gaToP3ga() const
{
    p3ga::mv vp;
    const c5ga &vc = c5();
    vp =
        // grade 0
        vc.scalar() +
        // grade 1
        vc[GRADE1][C5GA_E1] * p3ga::e1<double>() + vc[GRADE1][C5GA_E2] * p3ga::e2<double>() + vc[GRADE1][C5GA_E3] * p3ga::e3<double>() +
        vc[GRADE1][C5GA_NO] * p3ga::e0<double>() +
        // grade 2
        // NOTE: inherited bug, kept as is: the e1^e2 term is added twice and the e1^e3 term is missing
        vc[GRADE2][C5GA_E1_E2] * (p3ga::e1<double>() ^ p3ga::e2<double>()) + vc[GRADE2][C5GA_E2_E3] * (p3ga::e2<double>() ^ p3ga::e3<double>()) +
        vc[GRADE2][C5GA_E1_E2] * (p3ga::e1<double>() ^ p3ga::e2<double>()) + vc[GRADE2][C5GA_E1_NO] * (p3ga::e1<double>() ^ p3ga::e0<double>()) +
        vc[GRADE2][C5GA_E2_NO] * (p3ga::e2<double>() ^ p3ga::e0<double>()) + vc[GRADE2][C5GA_E3_NO] * (p3ga::e3<double>() ^ p3ga::e0<double>()) +
        // grade 3
        vc[GRADE3][C5GA_E2_E3_NO] * (p3ga::e2<double>() ^ p3ga::e3<double>() ^ p3ga::e0<double>()) +
        vc[GRADE3][C5GA_E1_E3_NO] * (p3ga::e1<double>() ^ p3ga::e3<double>() ^ p3ga::e0<double>()) +
        vc[GRADE3][C5GA_E1_E2_NO] * (p3ga::e1<double>() ^ p3ga::e2<double>() ^ p3ga::e0<double>()) +
        vc[GRADE3][C5GA_E1_E2_E3] * (p3ga::e1<double>() ^ p3ga::e2<double>() ^ p3ga::e3<double>()) +
        // grade 4
        vc[GRADE4][C5GA_E1_E2_E3_NO] * (p3ga::e1<double>() ^ p3ga::e2<double>() ^ p3ga::e3<double>() ^ p3ga::e0<double>());

    vp = ga::compress(vp);
    return new consoleVariable("", vp);
}

consoleVariable *consoleVariable::castC5gaToC3ga() const
{
    c3ga::mv vp;
    castC5gaToC3ga(c5(), vp);

    return new consoleVariable("", vp);
}

void consoleVariable::castC5gaToC3ga(const c5ga &vc, c3ga::mv &vp)
{
    // grade 0
    vp = vc.scalar();

    // grade 1
    if (vc.gradeUsage() & GRADE1)
    {
        vp += vc[GRADE1][C5GA_E1] * c3ga::e1<double>() + vc[GRADE1][C5GA_E2] * c3ga::e2<double>() + vc[GRADE1][C5GA_E3] * c3ga::e3<double>() +
              vc[GRADE1][C5GA_NO] * c3ga::eo<double>() + vc[GRADE1][C5GA_NI] * c3ga::ei<double>();
    }
    // grade 2
    if (vc.gradeUsage() & GRADE2)
    {
        vp += vc[GRADE2][C5GA_E1_E2] * (c3ga::e1<double>() ^ c3ga::e2<double>()) + vc[GRADE2][C5GA_E1_E3] * (c3ga::e1<double>() ^ c3ga::e3<double>()) +
              vc[GRADE2][C5GA_E2_E3] * (c3ga::e2<double>() ^ c3ga::e3<double>()) + vc[GRADE2][C5GA_E1_NO] * (c3ga::e1<double>() ^ c3ga::eo<double>()) +
              vc[GRADE2][C5GA_E2_NO] * (c3ga::e2<double>() ^ c3ga::eo<double>()) + vc[GRADE2][C5GA_E3_NO] * (c3ga::e3<double>() ^ c3ga::eo<double>()) +
              vc[GRADE2][C5GA_E1_NI] * (c3ga::e1<double>() ^ c3ga::ei<double>()) + vc[GRADE2][C5GA_E2_NI] * (c3ga::e2<double>() ^ c3ga::ei<double>()) +
              vc[GRADE2][C5GA_E3_NI] * (c3ga::e3<double>() ^ c3ga::ei<double>()) + vc[GRADE2][C5GA_NO_NI] * (c3ga::eo<double>() ^ c3ga::ei<double>());
    }
    // grade 3
    if (vc.gradeUsage() & GRADE3)
    {
        vp += vc[GRADE3][C5GA_E1_E2_E3] * (c3ga::e1<double>() ^ c3ga::e2<double>() ^ c3ga::e3<double>()) +
              vc[GRADE3][C5GA_E1_E2_NO] * (c3ga::e1<double>() ^ c3ga::e2<double>() ^ c3ga::eo<double>()) +
              vc[GRADE3][C5GA_E1_E3_NO] * (c3ga::e1<double>() ^ c3ga::e3<double>() ^ c3ga::eo<double>()) +
              vc[GRADE3][C5GA_E2_E3_NO] * (c3ga::e2<double>() ^ c3ga::e3<double>() ^ c3ga::eo<double>()) +
              vc[GRADE3][C5GA_E1_E2_NI] * (c3ga::e1<double>() ^ c3ga::e2<double>() ^ c3ga::ei<double>()) +
              vc[GRADE3][C5GA_E1_E3_NI] * (c3ga::e1<double>() ^ c3ga::e3<double>() ^ c3ga::ei<double>()) +
              vc[GRADE3][C5GA_E2_E3_NI] * (c3ga::e2<double>() ^ c3ga::e3<double>() ^ c3ga::ei<double>()) +
              vc[GRADE3][C5GA_E1_NO_NI] * (c3ga::e1<double>() ^ c3ga::eo<double>() ^ c3ga::ei<double>()) +
              vc[GRADE3][C5GA_E2_NO_NI] * (c3ga::e2<double>() ^ c3ga::eo<double>() ^ c3ga::ei<double>()) +
              vc[GRADE3][C5GA_E3_NO_NI] * (c3ga::e3<double>() ^ c3ga::eo<double>() ^ c3ga::ei<double>());
    }
    // grade 4
    if (vc.gradeUsage() & GRADE4)
    {
        vp += vc[GRADE4][C5GA_E1_E2_E3_NO] * (c3ga::e1<double>() ^ c3ga::e2<double>() ^ c3ga::e3<double>() ^ c3ga::eo<double>()) +
              vc[GRADE4][C5GA_E1_E2_E3_NI] * (c3ga::e1<double>() ^ c3ga::e2<double>() ^ c3ga::e3<double>() ^ c3ga::ei<double>()) +
              vc[GRADE4][C5GA_E1_E2_NO_NI] * (c3ga::e1<double>() ^ c3ga::e2<double>() ^ c3ga::eo<double>() ^ c3ga::ei<double>()) +
              vc[GRADE4][C5GA_E1_E3_NO_NI] * (c3ga::e1<double>() ^ c3ga::e3<double>() ^ c3ga::eo<double>() ^ c3ga::ei<double>()) +
              vc[GRADE4][C5GA_E2_E3_NO_NI] * (c3ga::e2<double>() ^ c3ga::e3<double>() ^ c3ga::eo<double>() ^ c3ga::ei<double>());
    }
    // grade 5
    if (vc.gradeUsage() & GRADE5)
    {
        vp += vc[GRADE5][C5GA_E1_E2_E3_NO_NI] * (c3ga::e1<double>() ^ c3ga::e2<double>() ^ c3ga::e3<double>() ^ c3ga::eo<double>() ^ c3ga::ei<double>());
    }

    vp = ga::compress(vp);
}

consoleVariable *consoleVariable::castC3gaToC5ga() const
{
    c5ga vp;
    const c3ga::mv &vc = c();

    // grade 0
    if (ga::gradeUsage(vc) & GRADE0)
    {
        vp += ga::scalar(vc);
    }

    // grade 1
    if (ga::gradeUsage(vc) & GRADE1)
    {
        vp += vc[c3ga::E1] * c5ga::e1 + vc[c3ga::E2] * c5ga::e2 + vc[c3ga::E3] * c5ga::e3 +
              vc[c3ga::Eo] * c5ga::no + vc[c3ga::Ei] * c5ga::ni;
    }

    // grade 2
    if (ga::gradeUsage(vc) & GRADE2)
    {
        vp += vc[c3ga::E12] * (c5ga::e1 ^ c5ga::e2) + vc[c3ga::E13] * (c5ga::e1 ^ c5ga::e3) +
              vc[c3ga::E23] * (c5ga::e2 ^ c5ga::e3) + vc[c3ga::E1o] * (c5ga::e1 ^ c5ga::no) +
              vc[c3ga::E2o] * (c5ga::e2 ^ c5ga::no) + vc[c3ga::E3o] * (c5ga::e3 ^ c5ga::no) +
              vc[c3ga::E1i] * (c5ga::e1 ^ c5ga::ni) + vc[c3ga::E2i] * (c5ga::e2 ^ c5ga::ni) +
              vc[c3ga::E3i] * (c5ga::e3 ^ c5ga::ni) + vc[c3ga::Eoi] * (c5ga::no ^ c5ga::ni);
    }

    // grade 3
    if (ga::gradeUsage(vc) & GRADE3)
    {
        vp += vc[c3ga::E123] * (c5ga::e1 ^ c5ga::e2 ^ c5ga::e3) +
              vc[c3ga::E12o] * (c5ga::e1 ^ c5ga::e2 ^ c5ga::no) +
              vc[c3ga::E13o] * (c5ga::e1 ^ c5ga::e3 ^ c5ga::no) +
              vc[c3ga::E23o] * (c5ga::e2 ^ c5ga::e3 ^ c5ga::no) +
              vc[c3ga::E12i] * (c5ga::e1 ^ c5ga::e2 ^ c5ga::ni) +
              vc[c3ga::E13i] * (c5ga::e1 ^ c5ga::e3 ^ c5ga::ni) +
              vc[c3ga::E23i] * (c5ga::e2 ^ c5ga::e3 ^ c5ga::ni) +
              vc[c3ga::E1oi] * (c5ga::e1 ^ c5ga::no ^ c5ga::ni) +
              vc[c3ga::E2oi] * (c5ga::e2 ^ c5ga::no ^ c5ga::ni) +
              vc[c3ga::E3oi] * (c5ga::e3 ^ c5ga::no ^ c5ga::ni);
    }

    // grade 4
    if (ga::gradeUsage(vc) & GRADE4)
    {
        vp += vc[c3ga::E123o] * (c5ga::e1 ^ c5ga::e2 ^ c5ga::e3 ^ c5ga::no) +
              vc[c3ga::E123i] * (c5ga::e1 ^ c5ga::e2 ^ c5ga::e3 ^ c5ga::ni) +
              vc[c3ga::E12oi] * (c5ga::e1 ^ c5ga::e2 ^ c5ga::no ^ c5ga::ni) +
              vc[c3ga::E13oi] * (c5ga::e1 ^ c5ga::e3 ^ c5ga::no ^ c5ga::ni) +
              vc[c3ga::E23oi] * (c5ga::e2 ^ c5ga::e3 ^ c5ga::no ^ c5ga::ni);
    }

    // grade 5
    if (ga::gradeUsage(vc) & GRADE5)
    {
        vp += vc[c3ga::E123oi] * (c5ga::e1 ^ c5ga::e2 ^ c5ga::e3 ^ c5ga::no ^ c5ga::ni);
    }

    vp.compress();
    return new consoleVariable("", vp);
}

consoleVariable *consoleVariable::castE3gaToI2ga() const
{
    i2ga vi2;
    const e3ga::mv &ve = e();

    // only e1, e2 are preserved
    vi2 =
        // grade 0
        ga::scalar(ve) +
        // grade 1
        ve[e3ga::E1] * i2ga::e1 + ve[e3ga::E2] * i2ga::e2 +
        // grade 2
        ve[e3ga::E12] * (i2ga::e1 ^ i2ga::e2);

    vi2.compress();
    return new consoleVariable("", vi2);
}

consoleVariable *consoleVariable::castP3gaToI2ga() const
{
    i2ga vi2;
    const p3ga::mv &vp = p();

    // e1, e2, e0 (-> no) are preserved
    vi2 =
        // grade 0
        ga::scalar(vp) +
        // grade 1
        vp[p3ga::E1] * i2ga::e1 + vp[p3ga::E2] * i2ga::e2 + vp[p3ga::E0] * i2ga::no +
        // grade 2
        vp[p3ga::E12] * (i2ga::e1 ^ i2ga::e2) + vp[p3ga::E10] * (i2ga::e1 ^ i2ga::no) +
        vp[p3ga::E20] * (i2ga::e2 ^ i2ga::no) +
        // grade 3
        // NOTE: inherited bug, kept as is: the Gaigen code read the grade-3 coordinate e1^e2^e0 from
        // grade 2 (vp[GRADE2][P3GA_E1_E2_E0]), i.e. the e1^e2 coordinate; should be vp[p3ga::E120]
        vp[p3ga::E12] * (i2ga::e1 ^ i2ga::e2 ^ i2ga::no);

    vi2.compress();
    return new consoleVariable("", vi2);
}

consoleVariable *consoleVariable::castC3gaToI2ga() const
{
    i2ga vi2;
    const c3ga::mv &vc = c();

    // e1, e2, ni, no are preserved
    vi2 =
        // grade 0
        ga::scalar(vc) +
        // grade 1
        vc[c3ga::E1] * i2ga::e1 + vc[c3ga::E2] * i2ga::e2 + vc[c3ga::Eo] * i2ga::no +
        vc[c3ga::Ei] * i2ga::ni +
        // grade 2
        vc[c3ga::E12] * (i2ga::e1 ^ i2ga::e2) + vc[c3ga::E1o] * (i2ga::e1 ^ i2ga::no) +
        vc[c3ga::E2o] * (i2ga::e2 ^ i2ga::no) + vc[c3ga::E1i] * (i2ga::e1 ^ i2ga::ni) +
        vc[c3ga::E2i] * (i2ga::e2 ^ i2ga::ni) + vc[c3ga::Eoi] * (i2ga::no ^ i2ga::ni) +
        // NOTE: inherited bug, kept as is: the Gaigen code read the grade 3 and 4 coordinates below from
        // grade 2 (vc[GRADE2][C3GA_E1_E2_NO], ...), i.e. the coordinates of other (grade 2) blades; they
        // should be vc[c3ga::E12o], vc[c3ga::E12i], vc[c3ga::E1oi], vc[c3ga::E2oi] and vc[c3ga::E12oi]
        // grade 3
        vc[c3ga::E13] * (i2ga::e1 ^ i2ga::e2 ^ i2ga::no) +
        vc[c3ga::E2o] * (i2ga::e1 ^ i2ga::e2 ^ i2ga::ni) +
        vc[c3ga::E2i] * (i2ga::e1 ^ i2ga::no ^ i2ga::ni) +
        vc[c3ga::E3i] * (i2ga::e2 ^ i2ga::no ^ i2ga::ni) +
        // grade 4
        vc[c3ga::E23] * (i2ga::e1 ^ i2ga::e2 ^ i2ga::no ^ i2ga::ni);

    vi2.compress();
    return new consoleVariable("", vi2);
}

consoleVariable *consoleVariable::castC5gaToI2ga() const
{
    i2ga vi2;
    const c5ga &vc = c5();

    // e1, e2, ni, no are preserved
    vi2 =
        // grade 0
        vc.scalar() +
        // grade 1
        vc[GRADE1][C5GA_E1] * i2ga::e1 + vc[GRADE1][C5GA_E2] * i2ga::e2 + vc[GRADE1][C5GA_NO] * i2ga::no +
        vc[GRADE1][C5GA_NI] * i2ga::ni +
        // grade 2
        vc[GRADE2][C5GA_E1_E2] * (i2ga::e1 ^ i2ga::e2) + vc[GRADE2][C5GA_E1_NO] * (i2ga::e1 ^ i2ga::no) +
        vc[GRADE2][C5GA_E2_NO] * (i2ga::e2 ^ i2ga::no) + vc[GRADE2][C5GA_E1_NI] * (i2ga::e1 ^ i2ga::ni) +
        vc[GRADE2][C5GA_E2_NI] * (i2ga::e2 ^ i2ga::ni) + vc[GRADE2][C5GA_NO_NI] * (i2ga::no ^ i2ga::ni) +
        // grade 3
        vc[GRADE2][C5GA_E1_E2_NO] * (i2ga::e1 ^ i2ga::e2 ^ i2ga::no) +
        vc[GRADE2][C5GA_E1_E2_NI] * (i2ga::e1 ^ i2ga::e2 ^ i2ga::ni) +
        vc[GRADE2][C5GA_E1_NO_NI] * (i2ga::e1 ^ i2ga::no ^ i2ga::ni) +
        vc[GRADE2][C5GA_E2_NO_NI] * (i2ga::e2 ^ i2ga::no ^ i2ga::ni) +
        // grade 4
        vc[GRADE2][C5GA_E1_E2_NO_NI] * (i2ga::e1 ^ i2ga::e2 ^ i2ga::no ^ i2ga::ni);

    vi2.compress();
    return new consoleVariable("", vi2);
}

consoleVariable *consoleVariable::castI2gaToE3ga() const
{
    e3ga::mv ve;
    const i2ga &vi2 = i2();

    // only e1, e2 are preserved
    ve =
        // grade 0
        vi2.scalar() +
        // grade 1
        vi2[GRADE1][I2GA_E1] * e3ga::e1<double>() + vi2[GRADE1][I2GA_E2] * e3ga::e2<double>() +
        // grade 2
        vi2[GRADE2][I2GA_E1_E2] * (e3ga::e1<double>() ^ e3ga::e2<double>());

    ve = ga::compress(ve);
    return new consoleVariable("", ve);
}

consoleVariable *consoleVariable::castI2gaToP3ga() const
{
    p3ga::mv vp;
    const i2ga &vi2 = i2();

    // e1, e2, e0 (-> no) are preserved
    vp =
        // grade 0
        vi2.scalar() +
        // grade 1
        vi2[GRADE1][I2GA_E1] * p3ga::e1<double>() + vi2[GRADE1][I2GA_E2] * p3ga::e2<double>() + vi2[GRADE1][I2GA_NO] * p3ga::e0<double>() +
        // grade 2
        vi2[GRADE2][I2GA_E1_E2] * (p3ga::e1<double>() ^ p3ga::e2<double>()) + vi2[GRADE2][I2GA_E1_NO] * (p3ga::e1<double>() ^ p3ga::e0<double>()) +
        vi2[GRADE2][I2GA_E2_NO] * (p3ga::e2<double>() ^ p3ga::e0<double>()) +
        // grade 3
        vi2[GRADE2][I2GA_E1_E2_NO] * (p3ga::e1<double>() ^ p3ga::e2<double>() ^ p3ga::e0<double>());

    vp = ga::compress(vp);
    return new consoleVariable("", vp);
}

consoleVariable *consoleVariable::castI2gaToC3ga() const
{
    c3ga::mv vc;
    const i2ga &vi2 = i2();

    // e1, e2, ni, no are preserved
    vc =
        // grade 0
        vi2.scalar() +
        // grade 1
        vi2[GRADE1][I2GA_E1] * c3ga::e1<double>() + vi2[GRADE1][I2GA_E2] * c3ga::e2<double>() + vi2[GRADE1][I2GA_NO] * c3ga::eo<double>() +
        vi2[GRADE1][I2GA_NI] * c3ga::ei<double>() +
        // grade 2
        vi2[GRADE2][I2GA_E1_E2] * (c3ga::e1<double>() ^ c3ga::e2<double>()) + vi2[GRADE2][I2GA_E1_NO] * (c3ga::e1<double>() ^ c3ga::eo<double>()) +
        vi2[GRADE2][I2GA_E2_NO] * (c3ga::e2<double>() ^ c3ga::eo<double>()) + vi2[GRADE2][I2GA_E1_NI] * (c3ga::e1<double>() ^ c3ga::ei<double>()) +
        vi2[GRADE2][I2GA_E2_NI] * (c3ga::e2<double>() ^ c3ga::ei<double>()) + vi2[GRADE2][I2GA_NO_NI] * (c3ga::eo<double>() ^ c3ga::ei<double>()) +
        // grade 3
        vi2[GRADE2][I2GA_E1_E2_NO] * (c3ga::e1<double>() ^ c3ga::e2<double>() ^ c3ga::eo<double>()) +
        vi2[GRADE2][I2GA_E1_E2_NI] * (c3ga::e1<double>() ^ c3ga::e2<double>() ^ c3ga::ei<double>()) +
        vi2[GRADE2][I2GA_E1_NO_NI] * (c3ga::e1<double>() ^ c3ga::eo<double>() ^ c3ga::ei<double>()) +
        vi2[GRADE2][I2GA_E2_NO_NI] * (c3ga::e2<double>() ^ c3ga::eo<double>() ^ c3ga::ei<double>()) +
        // grade 4
        vi2[GRADE2][I2GA_E1_E2_NO_NI] * (c3ga::e1<double>() ^ c3ga::e2<double>() ^ c3ga::eo<double>() ^ c3ga::ei<double>());

    vc = ga::compress(vc);
    return new consoleVariable("", vc);
}

consoleVariable *consoleVariable::castI2gaToC5ga() const
{
    c5ga vc;
    const i2ga &vi2 = i2();

    // e1, e2, ni, no are preserved
    vc =
        // grade 0
        vi2.scalar() +
        // grade 1
        vi2[GRADE1][I2GA_E1] * c5ga::e1 + vi2[GRADE1][I2GA_E2] * c5ga::e2 + vi2[GRADE1][I2GA_NO] * c5ga::no +
        vi2[GRADE1][I2GA_NI] * c5ga::ni +
        // grade 2
        vi2[GRADE2][I2GA_E1_E2] * (c5ga::e1 ^ c5ga::e2) + vi2[GRADE2][I2GA_E1_NO] * (c5ga::e1 ^ c5ga::no) +
        vi2[GRADE2][I2GA_E2_NO] * (c5ga::e2 ^ c5ga::no) + vi2[GRADE2][I2GA_E1_NI] * (c5ga::e1 ^ c5ga::ni) +
        vi2[GRADE2][I2GA_E2_NI] * (c5ga::e2 ^ c5ga::ni) + vi2[GRADE2][I2GA_NO_NI] * (c5ga::no ^ c5ga::ni) +
        // grade 3
        vi2[GRADE2][I2GA_E1_E2_NO] * (c5ga::e1 ^ c5ga::e2 ^ c5ga::no) +
        vi2[GRADE2][I2GA_E1_E2_NI] * (c5ga::e1 ^ c5ga::e2 ^ c5ga::ni) +
        vi2[GRADE2][I2GA_E1_NO_NI] * (c5ga::e1 ^ c5ga::no ^ c5ga::ni) +
        vi2[GRADE2][I2GA_E2_NO_NI] * (c5ga::e2 ^ c5ga::no ^ c5ga::ni) +
        // grade 4
        vi2[GRADE2][I2GA_E1_E2_NO_NI] * (c5ga::e1 ^ c5ga::e2 ^ c5ga::no ^ c5ga::ni);

    vc.compress();
    return new consoleVariable("", vc);
}

consoleVariable *consoleVariable::castToE3ga() const
{
    switch (model())
    {
    case MVI_E3GA:
        return new consoleVariable("", e());
        break;
    case MVI_P3GA:
        return castP3gaToE3ga();
        break;
    case MVI_C3GA:
        return castC3gaToE3ga();
        break;
    case MVI_C5GA:
        return castC5gaToE3ga();
        break;
    case MVI_I2GA:
        return castI2gaToE3ga();
        break;
    }
    return NULL;
}

consoleVariable *consoleVariable::castToP3ga() const
{
    switch (model())
    {
    case MVI_E3GA:
        return castE3gaToP3ga();
        break;
    case MVI_P3GA:
        return new consoleVariable("", p());
        break;
    case MVI_C3GA:
        return castC3gaToP3ga();
        break;
    case MVI_C5GA:
        return castC5gaToP3ga();
        break;
    case MVI_I2GA:
        return castI2gaToP3ga();
        break;
    }
    return NULL;
}

consoleVariable *consoleVariable::castToC3ga() const
{
    switch (model())
    {
    case MVI_E3GA:
        return castE3gaToC3ga();
        break;
    case MVI_P3GA:
        return castP3gaToC3ga();
        break;
    case MVI_C3GA:
        return new consoleVariable("", c());
        break;
    case MVI_C5GA:
        return castC5gaToC3ga();
        break;
    case MVI_I2GA:
        return castI2gaToC3ga();
        break;
    }
    return NULL;
}

consoleVariable *consoleVariable::castToC5ga() const
{
    switch (model())
    {
    case MVI_E3GA:
        return castE3gaToC5ga();
        break;
    case MVI_P3GA:
        return castP3gaToC5ga();
        break;
    case MVI_C3GA:
        return castC3gaToC5ga();
        break;
    case MVI_C5GA:
        return new consoleVariable("", c5());
        break;
    case MVI_I2GA:
        return castI2gaToC5ga();
        break;
    }
    return NULL;
}

consoleVariable *consoleVariable::castToI2ga() const
{
    switch (model())
    {
    case MVI_E3GA:
        return castE3gaToI2ga();
        break;
    case MVI_P3GA:
        return castP3gaToI2ga();
        break;
    case MVI_C3GA:
        return castC3gaToI2ga();
        break;
    case MVI_C5GA:
        return castC5gaToI2ga();
        break;
    case MVI_I2GA:
        return new consoleVariable("", i2());
        break;
    }
    return NULL;
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
