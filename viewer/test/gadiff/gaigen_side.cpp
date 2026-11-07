// gadiff: the Gaigen side (the algebras GAViewer was built on).

#include <string>
#include <vector>

#include "c3ga/c3ga.h"
#include "e3ga/e3ga.h"
#include "gadiff.h"
#include "p3ga/p3ga.h"

namespace
{

// what the adapter needs to know about a Gaigen algebra
struct e3gaTypes
{
    using MV = e3ga;
    using base = e3gai; // the Gaigen class hides some e3gai overloads (e.g. grade(), versorInverse(a))
    using names = e3ga_ben;
    [[maybe_unused]] static constexpr int nbGrades = 4; // (used by the adapter template)
    static const int *gradeSize()
    {
        return e3gai_gradeSize;
    }
};

struct p3gaTypes
{
    using MV = p3ga;
    using base = p3gai;
    using names = p3ga_ben;
    [[maybe_unused]] static constexpr int nbGrades = 5; // (used by the adapter template)
    static const int *gradeSize()
    {
        return p3gai_gradeSize;
    }
};

struct c3gaTypes
{
    using MV = c3ga;
    using base = c3gai;
    using names = c3ga_ben;
    [[maybe_unused]] static constexpr int nbGrades = 6; // (used by the adapter template)
    static const int *gradeSize()
    {
        return c3gai_gradeSize;
    }
};

template <class T> struct gaigenAdapter
{
    using MV = typename T::MV;
    using base = typename T::base;

    static int gradeStart(int g)
    {
        int s = 0;
        for (int k = 0; k < g; k++)
            s += T::gradeSize()[k];
        return s;
    }
    static int nbCoordinates()
    {
        return gradeStart(T::nbGrades);
    }

    static MV make(const coords &c)
    {
        int usage = 0;
        std::vector<double> packed;
        for (int g = 0; g < T::nbGrades; g++)
        {
            bool used = false;
            for (int k = 0; k < T::gradeSize()[g]; k++)
                used = used || (c[gradeStart(g) + k] != 0.0);
            if (!used)
                continue;
            usage |= 1 << g;
            for (int k = 0; k < T::gradeSize()[g]; k++)
                packed.push_back(c[gradeStart(g) + k]);
        }
        MV x;
        if (usage)
            x.set(usage, packed.data());
        return x;
    }

    static coords coordsOf(const MV &x)
    {
        coords c(nbCoordinates(), 0.0);
        for (int g = 0; g < T::nbGrades; g++)
            if (x.gradeUsage() & (1 << g))
            {
                const double *p = x.coordinates(1 << g);
                for (int k = 0; k < T::gradeSize()[g]; k++)
                    c[gradeStart(g) + k] = p[k];
            }
        return c;
    }

    static double scalar(const MV &a)
    {
        return a.scalar();
    }
    static unsigned gradeUsage(const MV &a)
    {
        return (unsigned)a.gradeUsage();
    }
    static int grade(const MV &a)
    {
        return static_cast<const base &>(a).grade();
    }
    static int maxGrade(const MV &a)
    {
        return a.maxGrade();
    }
    static int largestGrade(const MV &a)
    {
        return a.largestGrade();
    }
    static int highestGrade(const MV &a, MV &part)
    {
        return static_cast<base &>(part).highestGrade(a);
    }
    static double norm_a(const MV &a)
    {
        return a.norm_a();
    }
    static double norm_b(const MV &a)
    {
        return a.norm_b();
    }
    static double largestCoordinate(const MV &a)
    {
        return a.largestCoordinate();
    }
    static MV reverse(const MV &a)
    {
        return a.reverse();
    }
    static MV gradeInvolution(const MV &a)
    {
        return a.gradeInvolution();
    }
    static MV cliffordConjugate(const MV &a)
    {
        return a.cliffordConjugate();
    }
    static MV negate(const MV &a)
    {
        return a.negate();
    }
    static MV dual(const MV &a)
    {
        return a.dual();
    }
    static MV versorInverse(const MV &a, bool &ok)
    {
        MV r;
        ok = (static_cast<base &>(r).versorInverse(a) == 0);
        return r;
    }
    static MV inverse(const MV &a)
    {
        return a.inverse();
    }
    static MV generalInverse(const MV &a)
    {
        MV r;
        static_cast<base &>(r).generalInverse(a);
        return r;
    }
    static MV normalize(const MV &a, int norm)
    {
        return a.normal(norm);
    }
    static MV exp(const MV &a, int order)
    {
        return a.exp(order);
    }
    static MV compress(const MV &a, double epsilon)
    {
        MV r(a);
        r.compress(epsilon);
        return r;
    }
    static MV takeGrade(const MV &a, unsigned mask)
    {
        MV r;
        static_cast<base &>(r).takeGrade(a, (int)mask);
        return r;
    }
    static int mvType(const MV &a, int &gradeMask)
    {
        return a.mvType(&gradeMask);
    }
    static int factor(const MV &a, std::vector<MV> &factors)
    {
        MV f[8];
        int n = a.factor(f);
        factors.assign(f, f + n);
        return n;
    }
    static std::string toString(const MV &a, const char *fp)
    {
        return a.string(fp);
    }
    static int parse(const char *str, MV &result, bool delimiters)
    {
        typename T::names names('[', ']');
        if (!delimiters)
            names.setDelimiters(0, 0);
        return result.parseString(str, &names);
    }

    static MV gp(const MV &a, const MV &b)
    {
        return ::gp(a, b);
    }
    static MV op(const MV &a, const MV &b)
    {
        return ::op(a, b);
    }
    static MV lcont(const MV &a, const MV &b)
    {
        return ::lcont(a, b);
    }
    static MV rcont(const MV &a, const MV &b)
    {
        return ::rcont(a, b);
    }
    static MV scp(const MV &a, const MV &b)
    {
        return ::scp(a, b);
    }
    static MV hip(const MV &a, const MV &b)
    {
        return ::hip(a, b);
    }
    static MV mhip(const MV &a, const MV &b)
    {
        return ::mhip(a, b);
    }
    static MV igp(const MV &a, const MV &b)
    {
        return ::igp(a, b);
    }
    static MV gpem(const MV &a, const MV &b)
    {
        return ::gpem(a, b);
    }
    static MV lcem(const MV &a, const MV &b)
    {
        return ::lcem(a, b);
    }
    static MV add(const MV &a, const MV &b)
    {
        return ::add(a, b);
    }
    static MV sub(const MV &a, const MV &b)
    {
        return ::sub(a, b);
    }
    static MV project(const MV &blade, const MV &a)
    {
        MV r;
        static_cast<base &>(r).project(blade, a);
        return r;
    }
    static MV reject(const MV &blade, const MV &a)
    {
        MV r;
        static_cast<base &>(r).reject(blade, a);
        return r;
    }
    static MV meet(const MV &a, const MV &b)
    {
        return ::meet(a, b);
    }
    static MV join(const MV &a, const MV &b)
    {
        return ::join(a, b);
    }
    static int deltaProduct(const MV &a, const MV &b, MV &d)
    {
        return static_cast<base &>(d).deltaProduct(a, b);
    }
};

} // namespace

std::vector<std::string> gaigenResults(const std::string &algebra, const std::vector<coords> &inputs)
{
    if (algebra == "e3ga")
        return gadiff::run<gaigenAdapter<e3gaTypes>>(inputs);
    if (algebra == "p3ga")
        return gadiff::run<gaigenAdapter<p3gaTypes>>(inputs);
    if (algebra == "c3ga")
        return gadiff::run<gaigenAdapter<c3gaTypes>>(inputs);
    return {};
}
