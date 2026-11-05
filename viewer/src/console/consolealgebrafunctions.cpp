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

/*
The built-in console functions that exist for every algebra (gp, dual, print,
...): each is a template over the multivector type MV, instantiated for every
algebra in appendAlgebraFunctions().

(This replaces the code that the 'codegen' tool used to generate from
consolecodetemplates.txt, once for every algebra.)

To add a function: write its template below and add its entry (or entries,
one per argument signature) to the table in algebraFunctions(); M is the model
id of MV.
*/

#include <cmath>
#include <cstring>
#include <numbers>

#include <string>
#include <vector>

#include "../mt19937-2.h"
#include "../state.h"
#include "consolealgebra.h"
#include "consolefunction.h"
#include "consolescope.h"
#include "consolevariable.h"

namespace
{

// (these helpers are used only by the templates below: [[maybe_unused]] keeps clangd from
// reporting them as unneeded, the compiler itself does not)
[[maybe_unused]] const char *sc_tempVarPrefix = "temporary console variable ";

// Converts the precision argument of print(x, prec), e.g. "e", "1.8e" or "10.4f",
// into a printf conversion ("%1.8e"). Only flags, a width and a precision of
// at most two digits each and one of the conversions e, E, f, F, g, G are
// accepted: the result is used as a printf format.
[[maybe_unused]] bool printPrecision(const std::string &prec, std::string &format)
{
    size_t i = 0;
    while ((i < prec.size()) && std::strchr("-+ #0", prec[i]))
        i++;
    for (int part = 0; part < 2; part++)
    {
        if ((part == 1) && ((i >= prec.size()) || (prec[i] != '.')))
            break;
        if (part == 1)
            i++;
        size_t start = i;
        while ((i < prec.size()) && (prec[i] >= '0') && (prec[i] <= '9'))
            i++;
        if (i - start > 2)
            return false;
    }
    if ((i + 1 != prec.size()) || !std::strchr("eEfFgG", prec[i]))
        return false;
    format = "%" + prec;
    return true;
}

[[maybe_unused]] inline double asinhScalar(double x)
{
    return log(x + sqrt(x * x + 1));
}

// ********************************* SCALAR *********************************
template <class MV> consoleVariable *cf_scalar(consoleScope *s, consoleVariable *arg[])
{
    return new consoleVariable("", MV(mv<MV>(arg[0]).scalar()));
}

// ********************************* DUAL *********************************
template <class MV> consoleVariable *cf_dual(consoleScope *s, consoleVariable *arg[])
{
    return new consoleVariable("", mv<MV>(arg[0]).dual());
}

// ********************************* DUAL2 *********************************
template <class MV> consoleVariable *cf_dual2(consoleScope *s, consoleVariable *arg[])
{
    return new consoleVariable("", mv<MV>(arg[0]) << mv<MV>(arg[1]).inverse());
}

// ********************************* REVERSE *********************************
template <class MV> consoleVariable *cf_reverse(consoleScope *s, consoleVariable *arg[])
{
    return new consoleVariable("", mv<MV>(arg[0]).reverse());
}

// ********************************* CLIFFORD_CONJUGATE *********************************
template <class MV> consoleVariable *cf_clifford_conjugate(consoleScope *s, consoleVariable *arg[])
{
    return new consoleVariable("", mv<MV>(arg[0]).cliffordConjugate());
}

// ********************************* GRADE INVOLUTION *********************************
template <class MV> consoleVariable *cf_grade_involution(consoleScope *s, consoleVariable *arg[])
{
    return new consoleVariable("", mv<MV>(arg[0]).gradeInvolution());
}

// ********************************* INVERSE *********************************
template <class MV> consoleVariable *cf_inverse(consoleScope *s, consoleVariable *arg[])
{
    return new consoleVariable("", mv<MV>(arg[0]).inverse());
}

// ********************************* GENERAL INVERSE *********************************
template <class MV> consoleVariable *cf_general_inverse(consoleScope *s, consoleVariable *arg[])
{
    return new consoleVariable("", mv<MV>(arg[0]).generalInverse());
}

// ********************************* NEGATE *********************************
template <class MV> consoleVariable *cf_negate(consoleScope *s, consoleVariable *arg[])
{
    return new consoleVariable("", mv<MV>(arg[0]).negate());
}

// ********************************* GP *********************************
template <class MV> consoleVariable *cf_gp(consoleScope *s, consoleVariable *arg[])
{
    return new consoleVariable("", gp(mv<MV>(arg[0]), mv<MV>(arg[1])));
}

// ********************************* IGP *********************************
template <class MV> consoleVariable *cf_igp(consoleScope *s, consoleVariable *arg[])
{
    return new consoleVariable("", igp(mv<MV>(arg[0]), mv<MV>(arg[1])));
}

// ********************************* OP *********************************
template <class MV> consoleVariable *cf_op(consoleScope *s, consoleVariable *arg[])
{
    return new consoleVariable("", op(mv<MV>(arg[0]), mv<MV>(arg[1])));
}

// ********************************* HIP *********************************
template <class MV> consoleVariable *cf_hip(consoleScope *s, consoleVariable *arg[])
{
    return new consoleVariable("", hip(mv<MV>(arg[0]), mv<MV>(arg[1])));
}

// ********************************* MHIP *********************************
template <class MV> consoleVariable *cf_mhip(consoleScope *s, consoleVariable *arg[])
{
    return new consoleVariable("", mhip(mv<MV>(arg[0]), mv<MV>(arg[1])));
}

// ********************************* LCONT *********************************
template <class MV> consoleVariable *cf_lcont(consoleScope *s, consoleVariable *arg[])
{
    return new consoleVariable("", lcont(mv<MV>(arg[0]), mv<MV>(arg[1])));
}

// ********************************* GPEM *********************************
template <class MV> consoleVariable *cf_gpem(consoleScope *s, consoleVariable *arg[])
{
    return new consoleVariable("", gpem(mv<MV>(arg[0]), mv<MV>(arg[1])));
}

// ********************************* LCEM *********************************
template <class MV> consoleVariable *cf_lcem(consoleScope *s, consoleVariable *arg[])
{
    return new consoleVariable("", lcem(mv<MV>(arg[0]), mv<MV>(arg[1])));
}

// ********************************* RCONT *********************************
template <class MV> consoleVariable *cf_rcont(consoleScope *s, consoleVariable *arg[])
{
    return new consoleVariable("", rcont(mv<MV>(arg[0]), mv<MV>(arg[1])));
}

// ********************************* SCP *********************************
template <class MV> consoleVariable *cf_scp(consoleScope *s, consoleVariable *arg[])
{
    return new consoleVariable("", scp(mv<MV>(arg[0]), mv<MV>(arg[1])));
}

// ********************************* ADD *********************************
template <class MV> consoleVariable *cf_add(consoleScope *s, consoleVariable *arg[])
{
    return new consoleVariable("", add(mv<MV>(arg[0]), mv<MV>(arg[1])));
}

// ********************************* SUB *********************************
template <class MV> consoleVariable *cf_sub(consoleScope *s, consoleVariable *arg[])
{
    return new consoleVariable("", sub(mv<MV>(arg[0]), mv<MV>(arg[1])));
}

// ********************************* MEET *********************************
template <class MV> consoleVariable *cf_meet(consoleScope *s, consoleVariable *arg[])
{
    return new consoleVariable("", meet(mv<MV>(arg[0]), mv<MV>(arg[1])));
}

// ********************************* JOIN *********************************
template <class MV> consoleVariable *cf_join(consoleScope *s, consoleVariable *arg[])
{
    return new consoleVariable("", join(mv<MV>(arg[0]), mv<MV>(arg[1])));
}

// ********************************* CP *********************************
template <class MV> consoleVariable *cf_cp(consoleScope *s, consoleVariable *arg[])
{
    return new consoleVariable("",
                               op(0.5, sub(gp(mv<MV>(arg[0]), mv<MV>(arg[1])), gp(mv<MV>(arg[1]), mv<MV>(arg[0])))));
}

// ********************************* PRINT *********************************
template <class MV> consoleVariable *cf_print(consoleScope *s, consoleVariable *arg[])
{
    std::string name = (arg[0]->name().find(std::string(sc_tempVarPrefix)) == 0) ? std::string("ans") : arg[0]->name();
    cprintf("%s = %s\n", name.c_str(), mv<MV>(arg[0]).string());
    return arg[0];
}

template <class MV> consoleVariable *cf_print_prec(consoleScope *s, consoleVariable *arg[])
{
    std::string name = (arg[0]->name().find(std::string(sc_tempVarPrefix)) == 0) ? std::string("ans") : arg[0]->name();
    std::string format;
    if (!printPrecision(arg[1]->name(), format))
    {
        cprintf("print(): invalid precision '%s' (use e.g. \"e\", \"1.8e\" or \"10.4f\")\n", arg[1]->name().c_str());
        return arg[0];
    }
    cprintf("%s = %s\n", name.c_str(), mv<MV>(arg[0]).string(format.c_str()));
    return arg[0];
}

// ********************************* ASSIGN *********************************
template <class MV> consoleVariable *cf_assign(consoleScope *s, consoleVariable *arg[])
{
    consoleVariable *cv = s->assignVariable(arg[0], arg[1]);
    cv->name(arg[0]->name());
    return cv;
}

// ********************************* GRADE *********************************
template <class MV> consoleVariable *cf_grade(consoleScope *s, consoleVariable *arg[])
{
    return new consoleVariable("", mv<MV>(arg[0])(1 << ((int)mv<MV>(arg[1]).scalar())));
}

// ********************************* GRADE *********************************
template <class MV> consoleVariable *cf_grade_nb(consoleScope *s, consoleVariable *arg[])
{
    int grade, type = mv<MV>(arg[0]).mvType(&grade); // epsilon?
    if (type == GA_BLADE)
        grade = (int)(log((double)grade) / log((double)2) + 0.45);
    else
        grade = -1;

    return new consoleVariable("", MV((GAIM_FLOAT)grade));
}

// ********************************* VERSOR PARITY *********************************
template <class MV> consoleVariable *cf_versor_parity(consoleScope *s, consoleVariable *arg[])
{
    int grade, type = mv<MV>(arg[0]).mvType(&grade); // epsilon?
    if ((type == GA_BLADE) || (type == GA_VERSOR))
        grade = mv<MV>(arg[0]).largestGrade() & 1;
    else
        grade = -1;

    return new consoleVariable("", MV((GAIM_FLOAT)grade));
}

// ********************************* NORM *********************************
template <class MV> consoleVariable *cf_norm(consoleScope *s, consoleVariable *arg[])
{
    GAIM_FLOAT n = mv<MV>(arg[0]).norm_b();
    n = sqrt(fabs(n)) * ((n < 0) ? -1.0 : 1.0);
    return new consoleVariable("", MV(n));
}

// ********************************* NORM_A *********************************
template <class MV> consoleVariable *cf_norm_2(consoleScope *s, consoleVariable *arg[])
{
    return new consoleVariable("", MV(mv<MV>(arg[0]).norm_a()));
}
// ********************************* NORM_B *********************************
template <class MV> consoleVariable *cf_norm_r(consoleScope *s, consoleVariable *arg[])
{
    return new consoleVariable("", MV(mv<MV>(arg[0]).norm_b()));
}

// ********************************* NORMALIZE *********************************
template <class MV> consoleVariable *cf_normalize(consoleScope *s, consoleVariable *arg[])
{
    GAIM_FLOAT n = sqrt(fabs(mv<MV>(arg[0]).norm_b()));
    return new consoleVariable("", MV(mv<MV>(arg[0]) * ((n == 0.0) ? 0.0 : (1.0 / n))));
}

// ********************************* SQRT *********************************
template <class MV> consoleVariable *cf_sqrt(consoleScope *s, consoleVariable *arg[])
{
    return new consoleVariable("", MV(sqrt(mv<MV>(arg[0]).scalar())));
}

// ********************************* ABS *********************************
template <class MV> consoleVariable *cf_abs(consoleScope *s, consoleVariable *arg[])
{
    return new consoleVariable("", MV(fabs(mv<MV>(arg[0]).scalar())));
}

// ********************************* TAN *********************************
template <class MV> consoleVariable *cf_tan(consoleScope *s, consoleVariable *arg[])
{
    return new consoleVariable("", MV(tan(mv<MV>(arg[0]).scalar())));
}

// ********************************* COS *********************************
template <class MV> consoleVariable *cf_cos(consoleScope *s, consoleVariable *arg[])
{
    return new consoleVariable("", MV(cos(mv<MV>(arg[0]).scalar())));
}

// ********************************* SIN *********************************
template <class MV> consoleVariable *cf_sin(consoleScope *s, consoleVariable *arg[])
{
    return new consoleVariable("", MV(sin(mv<MV>(arg[0]).scalar())));
}

// ********************************* COSH *********************************
template <class MV> consoleVariable *cf_cosh(consoleScope *s, consoleVariable *arg[])
{
    return new consoleVariable("", MV(cosh(mv<MV>(arg[0]).scalar())));
}

// ********************************* SINH *********************************
template <class MV> consoleVariable *cf_sinh(consoleScope *s, consoleVariable *arg[])
{
    return new consoleVariable("", MV(sinh(mv<MV>(arg[0]).scalar())));
}

// ********************************* ACOSH *********************************
template <class MV> consoleVariable *cf_acosh(consoleScope *s, consoleVariable *arg[])
{
    double x = mv<MV>(arg[0]).scalar();
    double value = (x >= 1) ? log(x + sqrt(x * x - 1)) : 0.0;
    return new consoleVariable("", MV(value));
}

// ********************************* ASINH *********************************
template <class MV> consoleVariable *cf_asinh(consoleScope *s, consoleVariable *arg[])
{
    MV B = mv<MV>(arg[0]).grade(GRADE2);
    if (B.norm_a() != 0.0)
    {
        double B2 = gp(B, B).scalar();
        if (B2 < 0.0)
        {
            double sqrtMB2 = sqrt(-B2);
            return new consoleVariable("", op(asin(sqrtMB2) / sqrtMB2, B));
        }
        else if (B2 == 0.0)
        {
            return new consoleVariable("", B);
        }
        else
        {
            double sqrtB2 = sqrt(B2);
            return new consoleVariable("", op(asinhScalar(sqrtB2) / sqrtB2, B));
        }
    }
    else
    {
        double x = mv<MV>(arg[0]).scalar();
        double value = asinhScalar(x);
        return new consoleVariable("", MV(value));
    }
}

// ********************************* ATAN *********************************
template <class MV> consoleVariable *cf_atan(consoleScope *s, consoleVariable *arg[])
{
    return new consoleVariable("", MV(atan(mv<MV>(arg[0]).scalar())));
}

// ********************************* ATAN2 *********************************
template <class MV> consoleVariable *cf_atan2(consoleScope *s, consoleVariable *arg[])
{
    return new consoleVariable("", MV(atan2(mv<MV>(arg[0]).scalar(), mv<MV>(arg[1]).scalar())));
}

// ********************************* MAX *********************************
template <class MV> consoleVariable *cf_max(consoleScope *s, consoleVariable *arg[])
{
    GAIM_FLOAT val = (arg[0]->scalar() > arg[1]->scalar()) ? arg[0]->scalar() : arg[1]->scalar();
    return new consoleVariable("", MV(val));
}

// ********************************* MIN *********************************
template <class MV> consoleVariable *cf_min(consoleScope *s, consoleVariable *arg[])
{
    GAIM_FLOAT val = (arg[0]->scalar() < arg[1]->scalar()) ? arg[0]->scalar() : arg[1]->scalar();
    return new consoleVariable("", MV(val));
}

// ********************************* ACOS *********************************
template <class MV> consoleVariable *cf_acos(consoleScope *s, consoleVariable *arg[])
{
    return new consoleVariable("", MV(acos(mv<MV>(arg[0]).scalar())));
}

// ********************************* ASIN *********************************
template <class MV> consoleVariable *cf_asin(consoleScope *s, consoleVariable *arg[])
{
    return new consoleVariable("", MV(asin(mv<MV>(arg[0]).scalar())));
}

// ********************************* SINC *********************************
template <class MV> consoleVariable *cf_sinc(consoleScope *s, consoleVariable *arg[])
{
    double x = mv<MV>(arg[0]).scalar();
    if (x == 0.0)
        return new consoleVariable("", MV(1.0));
    else
        return new consoleVariable("", MV(sin(x) / x));
}

// ********************************* EXP *********************************
template <class MV> consoleVariable *cf_exp(consoleScope *s, consoleVariable *arg[])
{
    return new consoleVariable("", MV(mv<MV>(arg[0]).exp(12)));
}

// ********************************* LOG *********************************
template <class MV> consoleVariable *cf_log(consoleScope *s, consoleVariable *arg[])
{
    double x = mv<MV>(arg[0]).scalar();
    if (x == 0.0)
        return new consoleVariable("", MV(0));
    else if (x > 0.0)
        return new consoleVariable("", MV(log(x)));
    else
        return new consoleVariable("", MV(std::numbers::pi * (MV::e1 ^ MV::e2) + log(-x)));
}

// ********************************* SCALAR_POW *********************************
template <class MV> consoleVariable *cf_scalar_pow(consoleScope *s, consoleVariable *arg[])
{
    return new consoleVariable("", MV(pow(mv<MV>(arg[0]).scalar(), mv<MV>(arg[1]).scalar())));
}

// ********************************* RANDOM SEED *********************************
template <class MV> consoleVariable *cf_randSeed(consoleScope *s, consoleVariable *arg[])
{
    double _seed = mv<MV>(arg[0]).scalar();
    unsigned long seed = (unsigned long)(_seed * 32768.0);
    sgenrand(seed);

    return new consoleVariable("", MV(0.0));
}

// ********************************* RANDOM GAUSS NUMBER *********************************
template <class MV> consoleVariable *cf_randGauss(consoleScope *s, consoleVariable *arg[])
{
    double mean = mv<MV>(arg[0]).scalar();
    double variance = mv<MV>(arg[1]).scalar();
    return new consoleVariable("", MV(gaussNoise(mean, variance)));
}

// ********************************* POW *********************************
template <class MV> consoleVariable *cf_pow(consoleScope *s, consoleVariable *arg[])
{
    int n = (int)(mv<MV>(arg[1]).scalar() + 0.5), i;
    MV r(1.0);
    for (i = 0; i < n; i++)
        r = r * mv<MV>(arg[0]);

    return new consoleVariable("", r);
}

// ********************************* VERSOR PRODUCT *********************************
template <class MV> consoleVariable *cf_versorProduct(consoleScope *s, consoleVariable *arg[])
{
    return new consoleVariable("", MV(mv<MV>(arg[0]) * mv<MV>(arg[1]) * mv<MV>(arg[0]).inverse()));
}

// ********************************* INVERSE VERSOR PRODUCT *********************************
template <class MV> consoleVariable *cf_inverseVersorProduct(consoleScope *s, consoleVariable *arg[])
{
    return new consoleVariable("", MV(mv<MV>(arg[0]).inverse() * mv<MV>(arg[1]) * mv<MV>(arg[0])));
}

// ********************************* mierenneuk VERSOR PRODUCT *********************************
template <class MV> consoleVariable *cf_MNversorProduct(consoleScope *s, consoleVariable *arg[])
{
    MV v = mv<MV>(arg[0]);
    MV o = mv<MV>(arg[1]);

    int vgrade = v.largestGrade() & 1;

    if (vgrade)
        return new consoleVariable("", MV(v * o.gradeInvolution() * v.inverse()));
    else
        return new consoleVariable("", MV(v * o * v.inverse()));
}

// ********************************* mierenneuk INVERSE VERSOR PRODUCT *********************************
template <class MV> consoleVariable *cf_MNinverseVersorProduct(consoleScope *s, consoleVariable *arg[])
{
    MV v = mv<MV>(arg[0]);
    MV o = mv<MV>(arg[1]);

    int vgrade = v.largestGrade() & 1;

    if (vgrade)
        return new consoleVariable("", MV(v.inverse() * o.gradeInvolution() * v));
    else
        return new consoleVariable("", MV(v.inverse() * o * v));
}

// ********************************* PROJECT *********************************
template <class MV> consoleVariable *cf_project(consoleScope *s, consoleVariable *arg[])
{
    MV tmp;
    tmp.project(mv<MV>(arg[1]), mv<MV>(arg[0]));
    return new consoleVariable("", tmp);
}

// ********************************* reject *********************************
template <class MV> consoleVariable *cf_reject(consoleScope *s, consoleVariable *arg[])
{
    MV tmp;
    tmp.reject(mv<MV>(arg[1]), mv<MV>(arg[0]));
    return new consoleVariable("", tmp);
}

// ********************************* factor *********************************
template <class MV> consoleVariable *cf_factor(consoleScope *s, consoleVariable *arg[])
{
    MV tmp[5];
    mv<MV>(arg[0]).factor(tmp);

    int f = (int)(mv<MV>(arg[1]).scalar()) - 1;
    if (f > 4)
        f = 4;
    else if (f < 0)
        f = 0;

    return new consoleVariable("", tmp[f]);
}

// ********************************* set_factor *********************************
template <class MV> consoleVariable *cf_set_factor(consoleScope *s, consoleVariable *arg[])
{
    int idx = (int)arg[0]->scalar();
    consoleAlgebra<MV>::setFactor(mv<MV>(arg[1]), idx);
    return arg[1];
}

// ********************************* equal *********************************
template <class MV> consoleVariable *cf_equal(consoleScope *s, consoleVariable *arg[])
{
    return new consoleVariable("", MV((mv<MV>(arg[0]) - mv<MV>(arg[1])).norm_a() == 0.0));
}

// ********************************* ne *********************************
template <class MV> consoleVariable *cf_ne(consoleScope *s, consoleVariable *arg[])
{
    return new consoleVariable("", MV((mv<MV>(arg[0]) - mv<MV>(arg[1])).norm_a() != 0.0));
}

// ********************************* less *********************************
template <class MV> consoleVariable *cf_less(consoleScope *s, consoleVariable *arg[])
{
    return new consoleVariable("", MV(mv<MV>(arg[0]).scalar() < mv<MV>(arg[1]).scalar()));
}
// ********************************* greater *********************************
template <class MV> consoleVariable *cf_greater(consoleScope *s, consoleVariable *arg[])
{
    return new consoleVariable("", MV(mv<MV>(arg[0]).scalar() > mv<MV>(arg[1]).scalar()));
}
// ********************************* le *********************************
template <class MV> consoleVariable *cf_le(consoleScope *s, consoleVariable *arg[])
{
    return new consoleVariable("", MV(mv<MV>(arg[0]).scalar() <= mv<MV>(arg[1]).scalar()));
}
// ********************************* ge *********************************
template <class MV> consoleVariable *cf_ge(consoleScope *s, consoleVariable *arg[])
{
    return new consoleVariable("", MV(mv<MV>(arg[0]).scalar() >= mv<MV>(arg[1]).scalar()));
}

// ********************************* and *********************************
template <class MV> consoleVariable *cf_and(consoleScope *s, consoleVariable *arg[])
{
    return new consoleVariable("", MV(((mv<MV>(arg[0]).norm_a() != 0.0) && (mv<MV>(arg[1]).norm_a() != 0.0))));
}
// ********************************* or *********************************
template <class MV> consoleVariable *cf_or(consoleScope *s, consoleVariable *arg[])
{
    return new consoleVariable("", MV(((mv<MV>(arg[0]).norm_a() != 0.0) || (mv<MV>(arg[1]).norm_a() != 0.0))));
}

// ********************************* not *********************************
template <class MV> consoleVariable *cf_not(consoleScope *s, consoleVariable *arg[])
{
    return new consoleVariable("", MV(mv<MV>(arg[0]).norm_a() == 0.0));
}

// ********************************* CAST *********************************
// cast_xxx(arg): casts arg (of any model) to the model of MV
template <class MV> consoleVariable *cf_cast(consoleScope *s, consoleVariable *arg[])
{
    return consoleAlgebra<MV>::castTo(arg[0]);
}

// the built-in functions of algebra MV
template <class MV> std::vector<consoleFunc> algebraFunctions()
{
    constexpr int M = consoleAlgebra<MV>::model;
    return {
        {"print", NULL, cf_print<MV>, 1, {CVF_RHS | M}},
        {"print", NULL, cf_print_prec<MV>, 2, {CVF_RHS | M, CVF_RHS | CVF_ANY_MODEL}},
        {"scalar", NULL, cf_scalar<MV>, 1, {CVF_RHS | M}},
        {"dual", NULL, cf_dual<MV>, 1, {CVF_RHS | M}},
        {"dual", NULL, cf_dual2<MV>, 2, {CVF_RHS | M, CVF_RHS | M}},
        {"reverse", NULL, cf_reverse<MV>, 1, {CVF_RHS | M}},
        {"clifford_conjugate", NULL, cf_clifford_conjugate<MV>, 1, {CVF_RHS | M}},
        {"grade_involution", NULL, cf_grade_involution<MV>, 1, {CVF_RHS | M}},
        {"inverse", NULL, cf_inverse<MV>, 1, {CVF_RHS | M}},
        {"general_inverse", NULL, cf_general_inverse<MV>, 1, {CVF_RHS | M}},
        {"negate", NULL, cf_negate<MV>, 1, {CVF_RHS | M}},
        {"assign", NULL, cf_assign<MV>, 2, {CVF_LHS | CVF_ANY_MODEL, CVF_RHS | M}},
        {"gp", NULL, cf_gp<MV>, 2, {CVF_RHS | M, CVF_RHS | M}},
        {"igp", NULL, cf_igp<MV>, 2, {CVF_RHS | M, CVF_RHS | M}},
        {"op", NULL, cf_op<MV>, 2, {CVF_RHS | M, CVF_RHS | M}},
        {"hip", NULL, cf_hip<MV>, 2, {CVF_RHS | M, CVF_RHS | M}},
        {"mhip", NULL, cf_mhip<MV>, 2, {CVF_RHS | M, CVF_RHS | M}},
        {"lcont", NULL, cf_lcont<MV>, 2, {CVF_RHS | M, CVF_RHS | M}},
        {"rcont", NULL, cf_rcont<MV>, 2, {CVF_RHS | M, CVF_RHS | M}},
        {"scp", NULL, cf_scp<MV>, 2, {CVF_RHS | M, CVF_RHS | M}},
        {"gpem", NULL, cf_gpem<MV>, 2, {CVF_RHS | M, CVF_RHS | M}},
        {"lcem", NULL, cf_lcem<MV>, 2, {CVF_RHS | M, CVF_RHS | M}},
        {"meet", NULL, cf_meet<MV>, 2, {CVF_RHS | M, CVF_RHS | M}},
        {"cp", NULL, cf_cp<MV>, 2, {CVF_RHS | M, CVF_RHS | M}},
        {"join", NULL, cf_join<MV>, 2, {CVF_RHS | M, CVF_RHS | M}},
        {"add", NULL, cf_add<MV>, 2, {CVF_RHS | M, CVF_RHS | M}},
        {"sub", NULL, cf_sub<MV>, 2, {CVF_RHS | M, CVF_RHS | M}},

        {"grade", NULL, cf_grade_nb<MV>, 1, {CVF_RHS | M}},
        {"grade", NULL, cf_grade<MV>, 2, {CVF_RHS | M, CVF_RHS | M}},
        {"versor_parity", NULL, cf_versor_parity<MV>, 1, {CVF_RHS | M}},

        {"norm", NULL, cf_norm<MV>, 1, {CVF_RHS | M}},
        {"norm_2", NULL, cf_norm_2<MV>, 1, {CVF_RHS | M}},
        {"norm_r", NULL, cf_norm_r<MV>, 1, {CVF_RHS | M}},

        {"normalize", NULL, cf_normalize<MV>, 1, {CVF_RHS | M}},

        {"abs", NULL, cf_abs<MV>, 1, {CVF_RHS | M}},

        {"sqrt", NULL, cf_sqrt<MV>, 1, {CVF_RHS | M}},
        {"tan", NULL, cf_tan<MV>, 1, {CVF_RHS | M}},
        {"sin", NULL, cf_sin<MV>, 1, {CVF_RHS | M}},
        {"cos", NULL, cf_cos<MV>, 1, {CVF_RHS | M}},
        {"atan", NULL, cf_atan<MV>, 1, {CVF_RHS | M}},
        {"asin", NULL, cf_asin<MV>, 1, {CVF_RHS | M}},
        {"acos", NULL, cf_acos<MV>, 1, {CVF_RHS | M}},
        {"atan2", NULL, cf_atan2<MV>, 2, {CVF_RHS | M, CVF_RHS | M}},
        {"cosh", NULL, cf_cosh<MV>, 1, {CVF_RHS | M}},
        {"sinh", NULL, cf_sinh<MV>, 1, {CVF_RHS | M}},
        {"acosh", NULL, cf_acosh<MV>, 1, {CVF_RHS | M}},
        {"asinh", NULL, cf_asinh<MV>, 1, {CVF_RHS | M}},
        {"sinc", NULL, cf_sinc<MV>, 1, {CVF_RHS | M}},
        {"log", NULL, cf_log<MV>, 1, {CVF_RHS | M}},
        {"exp", NULL, cf_exp<MV>, 1, {CVF_RHS | M}},
        {"pow", NULL, cf_pow<MV>, 2, {CVF_RHS | M, CVF_RHS | M}},
        {"scalar_pow", NULL, cf_scalar_pow<MV>, 2, {CVF_RHS | M, CVF_RHS | M}},
        {"max", NULL, cf_max<MV>, 2, {CVF_RHS | M, CVF_RHS | M}},
        {"min", NULL, cf_min<MV>, 2, {CVF_RHS | M, CVF_RHS | M}},

        //	{"time", NULL, cf_time<MV>, 0, {}},
        {"randSeed", NULL, cf_randSeed<MV>, 1, {CVF_RHS | M}},
        {"randGauss", NULL, cf_randGauss<MV>, 2, {CVF_RHS | M, CVF_RHS | M}},

        {"versor_product", NULL, cf_versorProduct<MV>, 2, {CVF_RHS | M, CVF_RHS | M}},
        {"inverse_versor_product", NULL, cf_inverseVersorProduct<MV>, 2, {CVF_RHS | M, CVF_RHS | M}},
        {"vp", NULL, cf_versorProduct<MV>, 2, {CVF_RHS | M, CVF_RHS | M}},
        {"ivp", NULL, cf_inverseVersorProduct<MV>, 2, {CVF_RHS | M, CVF_RHS | M}},

        // temp mierenneuk version of versor product
        {"mnvp", NULL, cf_MNversorProduct<MV>, 2, {CVF_RHS | M, CVF_RHS | M}},
        {"mnivp", NULL, cf_MNinverseVersorProduct<MV>, 2, {CVF_RHS | M, CVF_RHS | M}},

        {"project", NULL, cf_project<MV>, 2, {CVF_RHS | M, CVF_RHS | M}},
        {"reject", NULL, cf_reject<MV>, 2, {CVF_RHS | M, CVF_RHS | M}},

        {"factor", NULL, cf_factor<MV>, 2, {CVF_RHS | M, CVF_RHS | M}},

        {"set_factor", NULL, cf_set_factor<MV>, 2, {CVF_RHS | CVF_ANY_MODEL, CVF_RHS | M}},

        {"equal", NULL, cf_equal<MV>, 2, {CVF_RHS | M, CVF_RHS | M}},
        {"ne", NULL, cf_ne<MV>, 2, {CVF_RHS | M, CVF_RHS | M}},
        {"less", NULL, cf_less<MV>, 2, {CVF_RHS | M, CVF_RHS | M}},
        {"greater", NULL, cf_greater<MV>, 2, {CVF_RHS | M, CVF_RHS | M}},
        {"le", NULL, cf_le<MV>, 2, {CVF_RHS | M, CVF_RHS | M}},
        {"ge", NULL, cf_ge<MV>, 2, {CVF_RHS | M, CVF_RHS | M}},
        {"and", NULL, cf_and<MV>, 2, {CVF_RHS | M, CVF_RHS | M}},
        {"or", NULL, cf_or<MV>, 2, {CVF_RHS | M, CVF_RHS | M}},
        {"not", NULL, cf_not<MV>, 1, {CVF_RHS | M}},

        /*
        To add:

        To debug general inverse fails after one failure?
        Or after multiple algebra use????
        */

    };
}

// cast_xxx() from algebra MV to every algebra
template <class MV> std::vector<consoleFunc> algebraCastFunctions()
{
    std::vector<consoleFunc> table;
    forEachConsoleAlgebra([&]<class MV2>() {
        table.push_back({consoleAlgebra<MV2>::castName, NULL, cf_cast<MV2>, 1, {CVF_RHS | consoleAlgebra<MV>::model}});
    });
    return table;
}

} // namespace

void appendAlgebraFunctions(std::vector<consoleFunc> &table)
{
    forEachConsoleAlgebra([&]<class MV>() {
        std::vector<consoleFunc> f = algebraFunctions<MV>();
        table.insert(table.end(), f.begin(), f.end());
    });
    forEachConsoleAlgebra([&]<class MV>() {
        std::vector<consoleFunc> f = algebraCastFunctions<MV>();
        table.insert(table.end(), f.begin(), f.end());
    });
}
