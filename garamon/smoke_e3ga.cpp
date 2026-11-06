// Smoke test of the Garamon e3ga algebra generated at build time: it compiles
// against the generated headers and checks a few basic identities.

#include <cmath>
#include <cstdio>

#include <e3ga/Mvec.hpp>

namespace
{
int failures = 0;

void check(bool ok, const char *what)
{
    if (!ok)
    {
        std::printf("FAILED: %s\n", what);
        failures++;
    }
}

bool near(double a, double b)
{
    return std::fabs(a - b) < 1e-12;
}
} // namespace

int main()
{
    using mv = e3ga::Mvec<double>;
    const mv e1 = e3ga::e1<double>(), e2 = e3ga::e2<double>(), e3 = e3ga::e3<double>();

    check(near((e1 * e1)[e3ga::scalar], 1.0), "e1 e1 = 1");
    check(near((e1 * e2)[e3ga::E12], 1.0), "e1 e2 = e12");
    check(near((e2 * e1)[e3ga::E12], -1.0), "e2 e1 = -e12");
    check(near((e1 ^ e2 ^ e3)[e3ga::E123], 1.0), "e1 ^ e2 ^ e3 = e123");
    check((e1 ^ e1).isEmpty(), "e1 ^ e1 = 0");
    check(near((e1 < (e1 ^ e2))[e3ga::E2], 1.0), "e1 _| (e1 ^ e2) = e2");

    const mv v = 2.0 * e1 + 3.0 * e2 - e3;
    check(near((v * v.inv())[e3ga::scalar], 1.0), "v v^-1 = 1");
    check(near(v.norm(), std::sqrt(14.0)), "|v| = sqrt(14)");

    if (failures == 0)
        std::printf("Garamon e3ga: all checks passed\n");
    return (failures == 0) ? 0 : 1;
}
