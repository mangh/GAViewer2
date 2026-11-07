// Differential test of the Garamon based algebra layer (viewer/src/ga) against
// the Gaigen algebras it replaces: both compute the same operations on the same
// multivectors; the results (as text) are compared with a tolerance.
//
// A Gaigen class (e.g. 'e3ga') and the Garamon namespace of the same name cannot
// be used in the same translation unit: each side is compiled in its own file
// (gaigen_side.cpp, garamon_side.cpp) and provides an "adapter" for
// gadiffRun(), which performs all operations in the same order on both sides.

#ifndef _GADIFF_H_
#define _GADIFF_H_

#include <cstdio>
#include <string>
#include <vector>

using coords = std::vector<double>; // coordinates in display (Gaigen) basis order

// the results of all operations on 'inputs', computed with the Gaigen algebra
// 'algebra' ("e3ga", "p3ga", ...; empty if unknown)
std::vector<std::string> gaigenResults(const std::string &algebra, const std::vector<coords> &inputs);

namespace gadiff
{

inline std::string str(double x)
{
    char buf[64];
    std::snprintf(buf, sizeof(buf), "%.17g", x);
    return buf;
}

inline std::string str(const coords &c)
{
    std::string s;
    for (double x : c)
        s += " " + str(x);
    return s;
}

// Runs all operations; adapter A provides the multivector type A::MV and the
// operations (see gaigen_side.cpp / garamon_side.cpp).
template <class A> std::vector<std::string> run(const std::vector<coords> &inputs)
{
    using MV = typename A::MV;
    std::vector<std::string> out;
    auto put = [&](const std::string &label, const std::string &value) { out.push_back(label + ":" + value); };
    auto mvs = [&](const std::string &label, const MV &x) { put(label, str(A::coordsOf(x))); };

    std::vector<MV> in;
    for (const coords &c : inputs)
        in.push_back(A::make(c));

    for (std::size_t i = 0; i < in.size(); i++)
    {
        const MV &a = in[i];
        std::string l = "[" + std::to_string(i) + "] ";

        put(l + "scalar", " " + str(A::scalar(a)));
        put(l + "gradeUsage", " " + std::to_string(A::gradeUsage(a)));
        put(l + "grade", " " + std::to_string(A::grade(a)));
        put(l + "maxGrade", " " + std::to_string(A::maxGrade(a)));
        put(l + "largestGrade", " " + std::to_string(A::largestGrade(a)));
        MV part;
        put(l + "highestGrade", " " + std::to_string(A::highestGrade(a, part)));
        mvs(l + "highestGrade part", part);
        put(l + "norm_a", " " + str(A::norm_a(a)));
        put(l + "norm_b", " " + str(A::norm_b(a)));
        put(l + "largestCoordinate", " " + str(A::largestCoordinate(a)));
        mvs(l + "reverse", A::reverse(a));
        mvs(l + "gradeInvolution", A::gradeInvolution(a));
        mvs(l + "cliffordConjugate", A::cliffordConjugate(a));
        mvs(l + "negate", A::negate(a));
        mvs(l + "dual", A::dual(a));
        bool ok = false;
        MV vi = A::versorInverse(a, ok);
        mvs(l + "versorInverse", vi);
        put(l + "versorInverse ok", ok ? " 1" : " 0");
        mvs(l + "inverse", A::inverse(a));
        mvs(l + "generalInverse", A::generalInverse(a));
        if (A::norm_a(a) != 0.0)
            mvs(l + "normalize(1)", A::normalize(a, 1));
        if (A::norm_b(a) != 0.0)
            mvs(l + "normalize(2)", A::normalize(a, 2));
        mvs(l + "exp(9)", A::exp(a, 9));
        mvs(l + "exp(12)", A::exp(a, 12));
        mvs(l + "compress(1e-3)", A::compress(a, 1e-3));
        for (unsigned mask : {1u, 2u, 4u, 6u, 9u})
            mvs(l + "takeGrade(" + std::to_string(mask) + ")", A::takeGrade(a, mask));
        int gradeMask = -1;
        int type = A::mvType(a, gradeMask);
        put(l + "mvType", " " + std::to_string(type) + " " + std::to_string(gradeMask));
        std::vector<MV> factors;
        int nf = A::factor(a, factors);
        put(l + "factor", " " + std::to_string(nf));
        for (int f = 0; f < nf; f++)
            mvs(l + "factor " + std::to_string(f), factors[f]);
        put(l + "string", " \"" + A::toString(a, "%2.2f") + "\"");
        put(l + "string %e", " \"" + A::toString(a, "%e") + "\"");
        // parse the text written with full precision, with and without delimiters
        std::string text = A::toString(a, "%.17g");
        MV parsed;
        int n = A::parse(text.c_str(), parsed, false);
        put(l + "parse", " " + std::to_string(n));
        mvs(l + "parse value", parsed);
        std::string delimited = "  [" + text + "] trailing";
        n = A::parse(delimited.c_str(), parsed, true);
        put(l + "parse []", " " + std::to_string(n));
        mvs(l + "parse [] value", parsed);
    }

    // binary operations on all pairs of the first 'nb' inputs
    const std::size_t nb = (in.size() < 40) ? in.size() : 40;
    for (std::size_t i = 0; i < nb; i++)
        for (std::size_t j = 0; j < nb; j++)
        {
            const MV &a = in[i], &b = in[j];
            std::string l = "[" + std::to_string(i) + "," + std::to_string(j) + "] ";
            mvs(l + "gp", A::gp(a, b));
            mvs(l + "op", A::op(a, b));
            mvs(l + "lcont", A::lcont(a, b));
            mvs(l + "rcont", A::rcont(a, b));
            mvs(l + "scp", A::scp(a, b));
            mvs(l + "hip", A::hip(a, b));
            mvs(l + "mhip", A::mhip(a, b));
            mvs(l + "igp", A::igp(a, b));
            mvs(l + "gpem", A::gpem(a, b));
            mvs(l + "lcem", A::lcem(a, b));
            mvs(l + "add", A::add(a, b));
            mvs(l + "sub", A::sub(a, b));
            mvs(l + "project", A::project(a, b));
            mvs(l + "reject", A::reject(a, b));
            // The delta product (and so meet and join) keeps the highest grade with a coordinate
            // above an absolute epsilon (1e-14, as in Gaigen): for large operands (coordinates
            // ~1e3) the rounding noise of a vanishing grade part exceeds it, and the result is
            // noise (differently rounded in Gaigen and Garamon), e.g. meet(B, B) of a large
            // bivector B. These are compared for operands of moderate size only.
            if ((A::norm_a(a) < 1e4) && (A::norm_a(b) < 1e4))
            {
                mvs(l + "meet", A::meet(a, b));
                mvs(l + "join", A::join(a, b));
                MV d;
                put(l + "deltaProduct", " " + std::to_string(A::deltaProduct(a, b, d)));
                mvs(l + "deltaProduct value", d);
            }
        }
    return out;
}

} // namespace gadiff

#endif /* _GADIFF_H_ */
