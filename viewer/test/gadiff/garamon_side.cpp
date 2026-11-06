// gadiff: the Garamon side (viewer/src/ga), the test inputs and the comparison.
//
//   gadiff_e3ga [-v]     (-v: print every result line, not only the mismatches)

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <random>
#include <sstream>
#include <string>
#include <vector>

#include "ga/algorithms.h"
#include "ga/e3ga.h"
#include "ga/format.h"
#include "gadiff.h"

namespace
{

struct garamonE3ga
{
    using MV = e3ga::mv;

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
std::vector<coords> inputs()
{
    using MV = e3ga::mv;
    std::vector<MV> v;

    v.push_back(MV());     // zero
    v.push_back(MV(2.5));  // scalars
    v.push_back(MV(-1.0));
    for (std::size_t i = 1; i < 8; i++)
        v.push_back(ga::basisBlade<MV>(i));
    // ties between the coordinates, and 'e1 + e2^e3' (which Gaigen takes for a versor)
    v.push_back(ga::fromCoordinates<MV>({0, 0, 0, 0, 1, 1, 0, 0}));
    v.push_back(ga::fromCoordinates<MV>({0, 1, 1, 1, 0, 0, 0, 0}));
    v.push_back(ga::fromCoordinates<MV>({0, 1, 0, 0, 1, 0, 0, 0}));

    std::mt19937 rng(20261106);
    std::uniform_real_distribution<double> u(-2.0, 2.0);
    auto vec = [&](double scale) {
        return ga::fromCoordinates<MV>({0, scale * u(rng), scale * u(rng), scale * u(rng), 0, 0, 0, 0});
    };
    for (double scale : {1.0, 1e-6, 1e3})
    {
        v.push_back(vec(scale));                             // vector
        v.push_back(vec(scale) ^ vec(1.0));                  // bivector (blade)
        v.push_back(vec(scale) ^ vec(1.0) ^ vec(1.0));       // trivector
        v.push_back(vec(scale) * vec(1.0));                  // even versor
        v.push_back(vec(scale) * vec(1.0) * vec(1.0));       // odd versor
        v.push_back(ga::exp(MV(vec(1.0) ^ vec(scale)), 12)); // rotor
    }
    for (int k = 0; k < 6; k++) // general multivectors
    {
        coords c(8);
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

bool tokensMatch(const std::string &a, const std::string &b, double tol)
{
    if (a == b)
        return true;
    char *ea, *eb;
    double x = std::strtod(a.c_str(), &ea), y = std::strtod(b.c_str(), &eb);
    if (*ea || *eb) // not both numbers
        return false;
    if (std::isnan(x) || std::isnan(y)) // NaN equals NaN (whatever its sign)
        return std::isnan(x) && std::isnan(y);
    return std::fabs(x - y) <= tol * std::fmax(1.0, std::fmax(std::fabs(x), std::fabs(y)));
}

} // namespace

int main(int argc, char *argv[])
{
    bool verbose = (argc > 1) && (std::string(argv[1]) == "-v");
    const double tol = 1e-9;

    std::vector<coords> in = inputs();
    std::vector<std::string> expected = gaigenResults(in);
    std::vector<std::string> actual = gadiff::run<garamonE3ga>(in);

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
        for (std::size_t k = 0; match && k < te.size(); k++)
            match = tokensMatch(te[k], ta[k], tol);
        if (!match)
        {
            mismatches++;
            std::printf("MISMATCH %s\n  Gaigen: %s\n  Garamon:%s\n", le.c_str(), expected[i].c_str() + le.size() + 1,
                        actual[i].c_str() + la.size() + 1);
        }
        else if (verbose)
            std::printf("%s\n", actual[i].c_str());
    }
    std::printf("%zu inputs, %zu results, %d mismatches\n", in.size(), expected.size(), mismatches);
    return (mismatches == 0) ? 0 : 1;
}
