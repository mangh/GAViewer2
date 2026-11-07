// gadiff: the Garamon side (viewer/src/ga), the test inputs and the comparison.
//
//   gadiff <algebra> [-v] [-all]
//     algebra: e3ga, p3ga, c3ga, c5ga
//     -v:   print every result line, not only the mismatches
//     -all: all pairs of inputs for the binary operations also in c5ga (by default every 11th
//           pair there: the ~30000 pairs of its ~170 inputs take minutes in a debug build)

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <random>
#include <sstream>
#include <string>
#include <vector>

#include "ga/algorithms.h"
#include "ga/c3ga.h"
#include "ga/c5ga.h"
#include "ga/e3ga.h"
#include "ga/format.h"
#include "ga/p3ga.h"
#include "gadiff.h"

namespace
{

template <class MV_> struct garamonAdapter
{
    using MV = MV_;

    static MV make(const coords &c)
    {
        return ga::fromCoordinates<MV>(c);
    }
    static coords coordsOf(const MV &x)
    {
        return ga::coordinates(x);
    }

    static double scalar(const MV &a)
    {
        return ga::scalar(a);
    }
    static unsigned gradeUsage(const MV &a)
    {
        return ga::gradeUsage(a);
    }
    static int grade(const MV &a)
    {
        return ga::grade(a);
    }
    static int maxGrade(const MV &a)
    {
        return ga::maxGrade(a);
    }
    static int largestGrade(const MV &a)
    {
        return ga::largestGrade(a);
    }
    static int highestGrade(const MV &a, MV &part)
    {
        return ga::highestGrade(a, part);
    }
    static double norm_a(const MV &a)
    {
        return ga::norm_a(a);
    }
    static double norm_b(const MV &a)
    {
        return ga::norm_b(a);
    }
    static double largestCoordinate(const MV &a)
    {
        return ga::largestCoordinate(a);
    }
    static MV reverse(const MV &a)
    {
        return ga::reverse(a);
    }
    static MV gradeInvolution(const MV &a)
    {
        return ga::gradeInvolution(a);
    }
    static MV cliffordConjugate(const MV &a)
    {
        return ga::cliffordConjugate(a);
    }
    static MV negate(const MV &a)
    {
        return ga::negate(a);
    }
    static MV dual(const MV &a)
    {
        return ga::dual(a);
    }
    static MV versorInverse(const MV &a, bool &ok)
    {
        return ga::versorInverse(a, &ok);
    }
    static MV inverse(const MV &a)
    {
        return ga::inverse(a);
    }
    static MV generalInverse(const MV &a)
    {
        return ga::generalInverse(a);
    }
    static MV normalize(const MV &a, int norm)
    {
        return ga::normalize(a, norm);
    }
    static MV exp(const MV &a, int order)
    {
        return ga::exp(a, order);
    }
    static MV compress(const MV &a, double epsilon)
    {
        return ga::compress(a, epsilon);
    }
    static MV takeGrade(const MV &a, unsigned mask)
    {
        return ga::takeGrade(a, mask);
    }
    static int mvType(const MV &a, int &gradeMask)
    {
        return ga::mvType(a, &gradeMask);
    }
    static int factor(const MV &a, std::vector<MV> &factors)
    {
        return ga::factor(a, factors);
    }
    static std::string toString(const MV &a, const char *fp)
    {
        return ga::toString(a, fp);
    }
    static int parse(const char *str, MV &result, bool delimiters)
    {
        ga::basisNames<MV> names(delimiters ? '[' : 0, delimiters ? ']' : 0);
        return ga::parse(str, result, names);
    }

    static MV gp(const MV &a, const MV &b)
    {
        return ga::gp(a, b);
    }
    static MV op(const MV &a, const MV &b)
    {
        return ga::op(a, b);
    }
    static MV lcont(const MV &a, const MV &b)
    {
        return ga::lcont(a, b);
    }
    static MV rcont(const MV &a, const MV &b)
    {
        return ga::rcont(a, b);
    }
    static MV scp(const MV &a, const MV &b)
    {
        return ga::scp(a, b);
    }
    static MV hip(const MV &a, const MV &b)
    {
        return ga::hip(a, b);
    }
    static MV mhip(const MV &a, const MV &b)
    {
        return ga::mhip(a, b);
    }
    static MV igp(const MV &a, const MV &b)
    {
        return ga::igp(a, b);
    }
    static MV gpem(const MV &a, const MV &b)
    {
        return ga::gpem(a, b);
    }
    static MV lcem(const MV &a, const MV &b)
    {
        return ga::lcem(a, b);
    }
    static MV add(const MV &a, const MV &b)
    {
        return a + b;
    }
    static MV sub(const MV &a, const MV &b)
    {
        return a - b;
    }
    static MV project(const MV &blade, const MV &a)
    {
        return ga::project(blade, a);
    }
    static MV reject(const MV &blade, const MV &a)
    {
        return ga::reject(blade, a);
    }
    static MV meet(const MV &a, const MV &b)
    {
        return ga::meet(a, b);
    }
    static MV join(const MV &a, const MV &b)
    {
        return ga::join(a, b);
    }
    static int deltaProduct(const MV &a, const MV &b, MV &d)
    {
        return ga::deltaProduct(a, b, d);
    }
};

// The test multivectors (deterministic): special values, basis blades, blades,
// versors and general multivectors of several magnitudes.
template <class MV> std::vector<coords> inputs()
{
    constexpr int n = ga::algebraTraits<MV>::dimension;
    constexpr std::size_t N = std::size_t(1) << n;
    const auto &basis = ga::algebraTraits<MV>::displayBasis;
    std::vector<MV> v;

    v.push_back(MV());    // zero
    v.push_back(MV(2.5)); // scalars
    v.push_back(MV(-1.0));
    for (std::size_t i = 1; i < N; i++)
        v.push_back(ga::basisBlade<MV>(i));

    // ties between the coordinates (the first two bivectors, all vectors), and
    // 'e1 + <first bivector>' (e.g. 'e1 + e2^e3', which Gaigen takes for a versor)
    std::vector<std::size_t> grade1, grade2;
    for (std::size_t i = 0; i < N; i++)
    {
        if (ga::bitmapGrade(basis[i].bitmap) == 1)
            grade1.push_back(i);
        if (ga::bitmapGrade(basis[i].bitmap) == 2)
            grade2.push_back(i);
    }
    coords c(N, 0.0);
    c[grade2[0]] = c[grade2[1]] = 1;
    v.push_back(ga::fromCoordinates<MV>(c));
    c.assign(N, 0.0);
    for (std::size_t i : grade1)
        c[i] = 1;
    v.push_back(ga::fromCoordinates<MV>(c));
    c.assign(N, 0.0);
    c[grade1[0]] = c[grade2[0]] = 1;
    v.push_back(ga::fromCoordinates<MV>(c));

    std::mt19937 rng(20261106);
    std::uniform_real_distribution<double> u(-2.0, 2.0);
    auto vec = [&](double scale) {
        MV r;
        for (int i = 0; i < n; i++)
            r += ga::basisVector<MV>(i) * (scale * u(rng));
        return r;
    };
    for (double scale : {1.0, 1e-6, 1e3})
    {
        MV blade = vec(scale);
        v.push_back(blade); // vector
        for (int g = 2; g <= n; g++)
        {
            blade = blade ^ vec(1.0);
            v.push_back(blade); // blades of grade 2 ... n
        }
        v.push_back(vec(scale) * vec(1.0));                  // even versor
        v.push_back(vec(scale) * vec(1.0) * vec(1.0));       // odd versor
        v.push_back(ga::exp(MV(vec(1.0) ^ vec(scale)), 12)); // rotor
    }
    for (int k = 0; k < 6; k++) // general multivectors
    {
        c.assign(N, 0.0);
        for (double &x : c)
            x = u(rng);
        v.push_back(ga::fromCoordinates<MV>(c));
    }

    std::vector<coords> result;
    for (const MV &x : v)
        result.push_back(ga::coordinates(x));
    return result;
}

// splits "label: tokens" into the label and the tokens
void splitLine(const std::string &line, std::string &label, std::vector<std::string> &tokens)
{
    std::size_t colon = line.find(':');
    label = line.substr(0, colon);
    tokens.clear();
    std::istringstream in(line.substr(colon + 1));
    if (line.find('"', colon) != std::string::npos) // a string: compared as a whole
    {
        tokens.push_back(line.substr(colon + 1));
        return;
    }
    for (std::string t; in >> t;)
        tokens.push_back(t);
}

// the largest absolute value of the numbers among 'tokens' (at least 1); a scale hint
// "~<number>" (see gadiff::run()) counts as such a number
double tokensScale(const std::vector<std::string> &tokens)
{
    double scale = 1.0;
    for (const std::string &t : tokens)
    {
        char *e;
        double x = std::strtod(t.c_str() + ((!t.empty() && (t[0] == '~')) ? 1 : 0), &e);
        if (!*e && std::isfinite(x))
            scale = std::fmax(scale, std::fabs(x));
    }
    return scale;
}

// Two tokens match if they are equal, or both numbers that differ by at most tol * scale,
// 'scale' being the magnitude of the whole result (the largest coordinate of a multivector):
// rounding noise in a coordinate that should be 0 is relative to the result, not to itself.
bool tokensMatch(const std::string &a, const std::string &b, double tol, double scale)
{
    if (a == b)
        return true;
    if (!a.empty() && !b.empty() && (a[0] == '~') && (b[0] == '~')) // scale hints (not compared)
        return true;
    char *ea, *eb;
    double x = std::strtod(a.c_str(), &ea), y = std::strtod(b.c_str(), &eb);
    if (*ea || *eb) // not both numbers
        return false;
    if (std::isnan(x) || std::isnan(y)) // NaN equals NaN (whatever its sign)
        return std::isnan(x) && std::isnan(y);
    return std::fabs(x - y) <= tol * scale;
}

} // namespace

int main(int argc, char *argv[])
{
    const std::string algebra = (argc > 1) ? argv[1] : "";
    bool verbose = false, all = false;
    for (int k = 2; k < argc; k++)
    {
        verbose = verbose || (std::string(argv[k]) == "-v");
        all = all || (std::string(argv[k]) == "-all");
    }
    std::size_t pairStride = 1;
    const double tol = 1e-9;

    std::vector<coords> in;
    std::vector<std::string> actual;
    if (algebra == "e3ga")
    {
        in = inputs<e3ga::mv>();
        actual = gadiff::run<garamonAdapter<e3ga::mv>>(in);
    }
    else if (algebra == "p3ga")
    {
        in = inputs<p3ga::mv>();
        actual = gadiff::run<garamonAdapter<p3ga::mv>>(in);
    }
    else if (algebra == "c3ga")
    {
        in = inputs<c3ga::mv>();
        actual = gadiff::run<garamonAdapter<c3ga::mv>>(in);
    }
    else if (algebra == "c5ga")
    {
        in = inputs<c5ga::mv>();
        pairStride = all ? 1 : 11;
        actual = gadiff::run<garamonAdapter<c5ga::mv>>(in, pairStride);
    }
    else
    {
        std::fprintf(stderr, "usage: gadiff <algebra> [-v] [-all]   (algebra: e3ga, p3ga, c3ga, c5ga)\n");
        return 2;
    }
    std::vector<std::string> expected = gaigenResults(algebra, in, pairStride);

    if (expected.size() != actual.size())
    {
        std::printf("different number of results: %zu (Gaigen) vs %zu (Garamon)\n", expected.size(),
                    actual.size());
        return 1;
    }

    int mismatches = 0;
    for (std::size_t i = 0; i < expected.size(); i++)
    {
        std::string le, la;
        std::vector<std::string> te, ta;
        splitLine(expected[i], le, te);
        splitLine(actual[i], la, ta);
        bool match = (le == la) && (te.size() == ta.size());
        const double scale = std::fmax(tokensScale(te), tokensScale(ta));
        for (std::size_t k = 0; match && k < te.size(); k++)
            match = tokensMatch(te[k], ta[k], tol, scale);
        if (!match)
        {
            mismatches++;
            std::printf("MISMATCH %s\n  Gaigen: %s\n  Garamon:%s\n", le.c_str(), expected[i].c_str() + le.size() + 1,
                        actual[i].c_str() + la.size() + 1);
        }
        else if (verbose)
            std::printf("%s\n", actual[i].c_str());
    }
    std::printf("%s: %zu inputs, %zu results, %d mismatches\n", algebra.c_str(), in.size(), expected.size(),
                mismatches);
    return (mismatches == 0) ? 0 : 1;
}
