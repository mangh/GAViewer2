// Checks the plain 3D drawing geometry (viewer/src/tubegeometry.h) against the
// multivector expressions it replaces (evaluated with the ga layer, which the
// gadiff tests check against the Gaigen algebras).

#include <cmath>
#include <cstdio>
#include <random>
#include <vector>

#include "ga/algorithms.h"
#include "ga/e3ga.h"
#include "ga/p3ga.h"
#include "tubegeometry.h"

namespace
{
using tubegeometry::vec3;
using tubegeometry::operator+; // (std::array: not found by argument dependent lookup)
using tubegeometry::operator*;

int failures = 0, checks = 0;

void check(const char *what, const vec3 &expected, const vec3 &actual, double tol = 1e-9)
{
    checks++;
    double scale = 1.0;
    for (int k = 0; k < 3; k++)
        scale = std::fmax(scale, std::fabs(expected[k]));
    for (int k = 0; k < 3; k++)
    {
        const bool bothNan = std::isnan(expected[k]) && std::isnan(actual[k]);
        if (!bothNan && !(std::fabs(expected[k] - actual[k]) <= tol * scale))
        {
            if (failures++ < 20)
                std::printf("FAILED %s: expected %.17g %.17g %.17g, got %.17g %.17g %.17g\n", what, expected[0],
                            expected[1], expected[2], actual[0], actual[1], actual[2]);
            return;
        }
    }
}

e3ga::mv mv(const vec3 &v)
{
    return e3ga::vector(v[0], v[1], v[2]);
}
vec3 v3(const e3ga::mv &v)
{
    return e3ga::xyz(v);
}

// the former rotorFromVectorToVector() (tubedraw.cpp, before the plain 3D version)
e3ga::mv rotorFromVectorToVector(const e3ga::mv &v1, const e3ga::mv &v2)
{
    if (ga::scalar(ga::scp(v1, v2)) < -0.99999f)
    {
        e3ga::mv tmp = ga::lcont(v1, e3ga::mv(v1 ^ v2));
        double n2 = ga::norm_a(tmp);
        if (n2 != 0.0f)
            tmp = ga::normalize(tmp);
        else
            tmp = (ga::norm_a(e3ga::mv(v1 ^ e3ga::e1<double>())) > ga::norm_a(e3ga::mv(v1 ^ e3ga::e2<double>())))
                      ? e3ga::e1<double>()
                      : e3ga::e2<double>();
        return ga::normalize(e3ga::mv(v1 ^ tmp));
    }
    double s = std::sqrt(2.0 * (1.0f + ga::scalar(ga::scp(v2, v1))));
    return (1.0 + v2 * v1) * (1.0f / s);
}

void checkRotation(const vec3 &a, const vec3 &b, const std::vector<vec3> &vs)
{
    const e3ga::mv R = rotorFromVectorToVector(mv(a), mv(b)), Ri = ga::inverse(R);
    const tubegeometry::frameRotation rot(a, b);
    for (const vec3 &v : vs)
        check("frameRotation", v3(ga::takeGrade(e3ga::mv(R * mv(v) * Ri), 2u)), rot(v));
}

void checkInitialFrame(const vec3 &d)
{
    std::vector<e3ga::mv> factors;
    ga::factor(ga::dual(mv(d)), factors);
    vec3 f0, f1;
    tubegeometry::initialFrame(d, f0, f1);
    check("initialFrame f0", v3(factors[0]), f0);
    check("initialFrame f1", v3(factors[1]), f1);
}

void checkEdgeJoin(const vec3 &tp1, const vec3 &td1, const vec3 &tp2, const vec3 &td2)
{
    // the former p3ga code (tubedraw.cpp)
    p3ga::mv d1 = p3ga::vector(td1[0], td1[1], td1[2], 0), d2 = p3ga::vector(td2[0], td2[1], td2[2], 0);
    p3ga::mv p1 = p3ga::vector(tp1[0], tp1[1], tp1[2], 1), p2 = p3ga::vector(tp2[0], tp2[1], tp2[2], 1);
    vec3 expected;
    if (ga::norm_a(p3ga::mv(p1 ^ p2)) < 1e-7)
        expected = tp1;
    else
    {
        p3ga::mv L1 = p1 ^ d1, L2 = p2 ^ d2, P = p1 ^ d1 ^ p2;
        p3ga::mv I = ga::lcont(ga::lcont(L1, P), L2);
        double Iw = I[p3ga::E0];
        expected = {I[p3ga::E1] / Iw, I[p3ga::E2] / Iw, I[p3ga::E3] / Iw};
    }
    check("edgeJoin", expected, tubegeometry::edgeJoin(tp1, td1, tp2, td2), 1e-7);
}

} // namespace

int main()
{
    std::mt19937 rng(20261107);
    std::uniform_real_distribution<double> u(-2.0, 2.0);
    auto rnd = [&]() { return vec3{u(rng), u(rng), u(rng)}; };
    auto unit = [&]() { return tubegeometry::normalize(rnd()); };

    std::vector<vec3> vs;
    for (int k = 0; k < 6; k++)
        vs.push_back(rnd());

    for (int k = 0; k < 2000; k++)
    {
        const vec3 a = unit(), b = unit();
        checkRotation(a, b, vs);
        checkRotation(a, tubegeometry::normalize(-1.0 * a + 1e-4 * b), vs); // nearly opposite
        checkInitialFrame(a);
        checkInitialFrame(3.7 * a); // not unit
        const vec3 p1 = rnd(), p2 = rnd();
        checkEdgeJoin(p1, a, p2, b);
        checkEdgeJoin(p1, a, p1 + 1e-5 * b, b); // (nearly) coinciding points
    }
    // exactly opposite: the e1 / e2 fallback of the half turn
    for (const vec3 &a : {vec3{1, 0, 0}, vec3{0, 1, 0}, vec3{0, 0, 1}, tubegeometry::normalize(vec3{1, 1, 0})})
        checkRotation(a, -1.0 * a, vs);
    // the axes (e1: the direction of a zero length segment) and ties between the coordinates of dual(d)
    for (const vec3 &d : {vec3{1, 0, 0}, vec3{0, 1, 0}, vec3{0, 0, 1}, vec3{1, 1, 0}, vec3{0, 1, 1}, vec3{1, 0, 1},
                          vec3{1, 1, 1}, vec3{-1, 1, -1}})
        checkInitialFrame(tubegeometry::normalize(d));

    // the normal of a triangle / tube cap: normalize(dual(a ^ b)) = normalize(a x b)
    for (int k = 0; k < 2000; k++)
    {
        const vec3 a = rnd(), b = rnd();
        check("dual(a ^ b)", v3(ga::normalize(ga::dual(e3ga::mv(mv(a) ^ mv(b))))),
              tubegeometry::normalize(tubegeometry::cross(a, b)));
    }
    // the arrow head normals of drawVector(): e1 rotated k times by exp(-0.5 * rotStep * e1^e2)
    {
        const double rotStep = 2 * 3.14159265358979323846 / 32;
        const e3ga::mv rt = ga::exp(e3ga::mv(-0.5 * rotStep * e3ga::e1<double>() ^ e3ga::e2<double>()));
        const e3ga::mv rti = ga::inverse(rt);
        e3ga::mv n = e3ga::e1<double>();
        double z = 0;
        for (int k = 0; k <= 32; k++, z += rotStep)
        {
            check("arrow head normal", v3(n), vec3{std::cos(z), std::sin(z), 0.0}, 1e-12);
            n = ga::takeGrade(e3ga::mv(rt * n * rti), 2u);
        }
    }

    std::printf("tubegeometry: %d checks, %d failures\n", checks, failures);
    return (failures == 0) ? 0 : 1;
}
