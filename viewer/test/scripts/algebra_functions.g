// Regression test: the built-in per-algebra console functions and operators,
// evaluated in every model (e3ga, p3ga, c3ga, c5ga, i2ga).
//
// The book figures use only a few of these functions (and hardly any c5ga or
// i2ga), so this script covers the rest. Every result is printed with
// print(x, "1.8e"); the harness compares the console output with the golden
// reference (numbers within a tolerance).
//
// Arguments of the test functions (all of the same model):
//   A: a general multivector (mixed grades)
//   v, w, u: vectors
//   R: a versor (rotor)
//   s: a scalar in (0, 1), t: a scalar > 1

function test_unary(x)
{
    print(scalar_ = scalar(x), "1.8e");
    print(dual_ = dual(x), "1.8e");
    print(reverse_ = reverse(x), "1.8e");
    print(clifford_conjugate_ = clifford_conjugate(x), "1.8e");
    print(grade_involution_ = grade_involution(x), "1.8e");
    print(inverse_ = inverse(x), "1.8e");
    print(general_inverse_ = general_inverse(x), "1.8e");
    print(negate_ = negate(x), "1.8e");
    print(grade_ = grade(x), "1.8e");
    print(grade0_ = grade(x, 0), "1.8e");
    print(grade1_ = grade(x, 1), "1.8e");
    print(grade2_ = grade(x, 2), "1.8e");
    print(grade3_ = grade(x, 3), "1.8e");
    print(versor_parity_ = versor_parity(x), "1.8e");
    print(norm_ = norm(x), "1.8e");
    print(norm_2_ = norm_2(x), "1.8e");
    print(norm_r_ = norm_r(x), "1.8e");
    print(normalize_ = normalize(x), "1.8e");
    print(exp_ = exp(x), "1.8e");
    print(not_ = not(x), "1.8e");
    print(pow_ = pow(x, 3), "1.8e");
    print(minus_ = -x, "1.8e");
    print(inverse_op_ = !x, "1.8e");
    print(reverse_op_ = ~x, "1.8e");
}

function test_binary(x, y)
{
    print(gp_ = gp(x, y), "1.8e");
    print(igp_ = igp(x, y), "1.8e");
    print(op_ = op(x, y), "1.8e");
    print(hip_ = hip(x, y), "1.8e");
    print(mhip_ = mhip(x, y), "1.8e");
    print(lcont_ = lcont(x, y), "1.8e");
    print(rcont_ = rcont(x, y), "1.8e");
    print(scp_ = scp(x, y), "1.8e");
    print(gpem_ = gpem(x, y), "1.8e");
    print(lcem_ = lcem(x, y), "1.8e");
    print(cp_ = cp(x, y), "1.8e");
    print(add_ = add(x, y), "1.8e");
    print(sub_ = sub(x, y), "1.8e");
    print(dual2_ = dual(x, y), "1.8e");
    print(equal_ = equal(x, y), "1.8e");
    print(equal_self_ = equal(x, x), "1.8e");
    print(ne_ = ne(x, y), "1.8e");
    print(and_ = and(x, y), "1.8e");
    print(or_ = or(x, y), "1.8e");
    print(gp_op_ = x y, "1.8e");
    print(igp_op_ = x / y, "1.8e");
    print(op_op_ = x ^ y, "1.8e");
    print(hip_op_ = x . y, "1.8e");
    print(scp_op_ = x * y, "1.8e");
    print(add_op_ = x + y, "1.8e");
    print(sub_op_ = x - y, "1.8e");
    print(equal_op_ = x == y, "1.8e");
    print(ne_op_ = x != y, "1.8e");
    print(and_op_ = x && y, "1.8e");
    print(or_op_ = x || y, "1.8e");
}

function test_versor(R, x)
{
    print(versor_product_ = versor_product(R, x), "1.8e");
    print(inverse_versor_product_ = inverse_versor_product(R, x), "1.8e");
    print(vp_ = vp(R, x), "1.8e");
    print(ivp_ = ivp(R, x), "1.8e");
    print(mnvp_ = mnvp(R, x), "1.8e");
    print(mnivp_ = mnivp(R, x), "1.8e");
}

function test_blades(B, C)
{
    print(meet_ = meet(B, C), "1.8e");
    print(join_ = join(B, C), "1.8e");
    print(meet_op_ = B & C, "1.8e");
    print(join_op_ = B | C, "1.8e");
    print(project_ = project(B, C), "1.8e");
    print(reject_ = reject(B, C), "1.8e");
    print(factor1_ = factor(C, 1), "1.8e");
    print(factor2_ = factor(C, 2), "1.8e");
    print(factor3_ = factor(C, 3), "1.8e");
}

function test_scalar(s, t)
{
    print(abs_ = abs(-s), "1.8e");
    print(sqrt_ = sqrt(t), "1.8e");
    print(tan_ = tan(s), "1.8e");
    print(sin_ = sin(s), "1.8e");
    print(cos_ = cos(s), "1.8e");
    print(atan_ = atan(s), "1.8e");
    print(asin_ = asin(s), "1.8e");
    print(acos_ = acos(s), "1.8e");
    print(atan2_ = atan2(s, t), "1.8e");
    print(cosh_ = cosh(s), "1.8e");
    print(sinh_ = sinh(s), "1.8e");
    print(acosh_ = acosh(t), "1.8e");
    print(asinh_ = asinh(s), "1.8e");
    print(sinc_ = sinc(s), "1.8e");
    print(log_ = log(t), "1.8e");
    print(log0_ = log(0 s), "1.8e");
    print(exp_ = exp(s), "1.8e");
    print(pow_ = pow(t, 4), "1.8e");
    print(scalar_pow_ = scalar_pow(t, s), "1.8e");
    print(max_ = max(s, t), "1.8e");
    print(min_ = min(s, t), "1.8e");
    print(less_ = less(s, t), "1.8e");
    print(greater_ = greater(s, t), "1.8e");
    print(le_ = le(s, s), "1.8e");
    print(ge_ = ge(s, t), "1.8e");
    print(less_op_ = s < t, "1.8e");
    print(greater_op_ = s > t, "1.8e");
    print(le_op_ = s <= t, "1.8e");
    print(ge_op_ = s >= t, "1.8e");
    print(randSeed_ = randSeed(s), "1.8e");
    print(randGauss1_ = randGauss(0 s, t), "1.8e");
    print(randGauss2_ = randGauss(s, t), "1.8e");
}

function test_casts(x)
{
    print(cast_e3ga_ = cast_e3ga(x), "1.8e");
    print(cast_p3ga_ = cast_p3ga(x), "1.8e");
    print(cast_c3ga_ = cast_c3ga(x), "1.8e");
    print(cast_c5ga_ = cast_c5ga(x), "1.8e");
    print(cast_i2ga_ = cast_i2ga(x), "1.8e");
    print(cast_e3ga_op_ = (e3ga)x, "1.8e");
    print(cast_p3ga_op_ = (p3ga)x, "1.8e");
    print(cast_c3ga_op_ = (c3ga)x, "1.8e");
    print(cast_c5ga_op_ = (c5ga)x, "1.8e");
    print(cast_i2ga_op_ = (i2ga)x, "1.8e");
}

function test_model(A, v, w, u, R, s, t)
{
    print(A, "1.8e");
    print(v, "1.8e");
    print(w, "1.8e");
    print(u, "1.8e");
    print(R, "1.8e");
    cprint("--- unary: A");
    test_unary(A);
    cprint("--- unary: v");
    test_unary(v);
    cprint("--- unary: v^w");
    test_unary(v ^ w);
    cprint("--- unary: R");
    test_unary(R);
    cprint("--- binary: v, w");
    test_binary(v, w);
    cprint("--- binary: v^w, u");
    test_binary(v ^ w, u);
    cprint("--- binary: A, R");
    test_binary(A, R);
    cprint("--- versor: R, v");
    test_versor(R, v);
    cprint("--- versor: R, v^w");
    test_versor(R, v ^ w);
    cprint("--- blades: v^w, w^u");
    test_blades(v ^ w, w ^ u);
    cprint("--- blades: v, v^w^u");
    test_blades(v, v ^ w ^ u);
    cprint("--- scalar");
    test_scalar(s, t);
    cprint("--- casts: A");
    test_casts(A);
}

function algebra_functions()
{
    cprint("===== e3ga");
    default_model(e3ga);
    test_model(1.5 + e1 - 0.5 e2 + 0.7 e2^e3 - 0.3 e1^e2 + 0.2 e1^e2^e3,
               e1 + 2 e2 - 0.5 e3, 0.3 e1 - e2 + 1.5 e3, -e1 + 0.4 e2 + 0.8 e3,
               exp(-0.3 e1^e2) exp(0.2 e2^e3), 0.6, 2.5);

    cprint("===== p3ga");
    default_model(p3ga);
    test_model(1.5 + e0 + e1 - 0.5 e2 + 0.7 e2^e3 - 0.3 e0^e1 + 0.2 e0^e1^e2 - 0.1 e0^e1^e2^e3,
               e0 + e1 + 2 e2 - 0.5 e3, e0 + 0.3 e1 - e2 + 1.5 e3, e0 - e1 + 0.4 e2 + 0.8 e3,
               exp(-0.3 e1^e2) exp(0.2 e2^e3), 0.6, 2.5);

    cprint("===== c3ga");
    default_model(c3ga);
    test_model(1.5 + no + e1 - 0.5 e2 + 0.7 e2^ni - 0.3 no^e1 + 0.2 e1^e2^e3 - 0.1 no^e1^e2^ni,
               no + e1 + 2 e2 - 0.5 e3 + 2.625 ni, no + 0.3 e1 - e2 + 1.5 e3 + 1.67 ni,
               -e1 + 0.4 e2 + 0.8 e3 + 0.5 ni,
               exp(-0.3 e1^e2) exp(0.2 e2^e3) exp(-0.5 (e1 + e3)^ni), 0.6, 2.5);

    cprint("===== c5ga");
    default_model(c5ga);
    test_model(1.5 + no + e1 - 0.5 e4 + 0.7 e2^ni - 0.3 e4^e5 + 0.2 e1^e2^e3 - 0.1 no^e1^e5^ni,
               no + e1 + 2 e2 - 0.5 e3 + 0.25 e4 - e5 + 2.75 ni, no + 0.3 e1 - e2 + 1.5 e4 + 1.67 ni,
               -e1 + 0.4 e2 + 0.8 e3 + 0.6 e5 + 0.5 ni,
               exp(-0.3 e1^e4) exp(0.2 e2^e5) exp(-0.5 (e1 + e3)^ni), 0.6, 2.5);

    cprint("===== i2ga");
    default_model(i2ga);
    test_model(1.5 + no + e1 - 0.5 go + 0.7 e2^ni - 0.3 go^gi + 0.2 e1^e2^gi - 0.1 no^e1^go^ni,
               no + e1 + 2 e2 + 0.5 go + 2.5 ni - gi, no + 0.3 e1 - e2 + 1.5 gi + 0.545 ni,
               -e1 + 0.4 e2 + 0.8 go + 0.5 ni,
               exp(-0.3 e1^e2) exp(0.2 e2^go) exp(-0.5 (e1 + gi)^ni), 0.6, 2.5);
}
