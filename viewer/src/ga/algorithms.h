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

// The algorithms are ported from the Gaigen 1 generated algebras (e3ga.cpp, ...),
// Copyright 2002-2009, Daniel Fontijne, University of Amsterdam.

#ifndef _GA_ALGORITHMS_H_
#define _GA_ALGORITHMS_H_

/*
Geometric algebra operations that GAViewer needs and Garamon does not
provide (or provides with another definition), written once for every
Garamon multivector type MV (with ga::algebraTraits<MV>, see traits.h).

They reproduce the behaviour of the Gaigen 1 algebras GAViewer was built on,
including their conventions:

- in algebras with a non-Euclidean metric (c3ga, ...), some algorithms use the
  Euclidean metric versions of the products (gpem, lcem, scpem, projectEM,
  versorInverseEM: all basis vectors taken as orthonormal), exactly where Gaigen
  does: the delta product (and so meet and join), factor and mvType,

- dual(A) = A I^-1 (Garamon's Mvec::dual() is A _| I, of opposite sign in e3ga),
- inverse(A) = versorInverse(A) = ~A / <A ~A>, zero if <A ~A> = 0,
- norm_a(A) = sum of the squared coordinates (no square root!),
  norm_b(A) = <A ~A>,
- a 'grade usage' is a bitmask of grades (bit k = grade k, as GRADEk in Gaigen),
- hip() is the Hestenes inner product (0 if an operand is a scalar),
  mhip() the modified one (scalars included),
- 'epsilon' defaults to 1e-14, as in Gaigen.

Algorithms that depend on the order of the coordinates (e.g. finding the
largest one) iterate in the display basis order (the Gaigen basis order).
*/

#include <cmath>
#include <cstddef>
#include <utility>
#include <vector>

#include "traits.h"

namespace ga
{

inline constexpr double defaultEpsilon = 1e-14;

// the results of mvType() (the values of GA_BLADE, ... in the Gaigen algebras)
enum mvTypes
{
    mvTypeBlade = 1,       // a versor of a single grade
    mvTypeVersor = 2,      // an invertible product of vectors
    mvTypeMultivector = 3, // anything else
};

template <garamonMV MV> constexpr int dimensionOf()
{
    return algebraTraits<MV>::dimension;
}

// ---------------------------------------------------------------------------
// coordinates, grades
// ---------------------------------------------------------------------------

// the coordinate of display basis blade 'i' (see algebraTraits<MV>::displayBasis)
template <garamonMV MV> double coordinate(const MV &a, std::size_t i)
{
    const displayBlade &b = algebraTraits<MV>::displayBasis[i];
    return b.sign * a[b.bitmap];
}

// sets the coordinate of display basis blade 'i'
template <garamonMV MV> void setCoordinate(MV &a, std::size_t i, double value)
{
    const displayBlade &b = algebraTraits<MV>::displayBasis[i];
    a[b.bitmap] = b.sign * value;
}

// the multivector with the given coordinates (in display basis order)
template <garamonMV MV> MV fromCoordinates(const std::vector<double> &c)
{
    MV r;
    for (std::size_t i = 0; i < c.size(); i++)
        if (c[i] != 0.0)
            setCoordinate(r, i, c[i]);
    return r;
}

// the coordinates (in display basis order)
template <garamonMV MV> std::vector<double> coordinates(const MV &a)
{
    constexpr std::size_t n = std::size_t(1) << dimensionOf<MV>();
    std::vector<double> c(n);
    for (std::size_t i = 0; i < n; i++)
        c[i] = coordinate(a, i);
    return c;
}

// the basis blade with display index 'i' (with its display orientation)
template <garamonMV MV> MV basisBlade(std::size_t i)
{
    MV r;
    setCoordinate(r, i, 1.0);
    return r;
}

// true if 'a' has a part of grade 'k' (zero has none; Garamon's isGrade(0)
// is true for zero, Gaigen's grade usage of zero is empty)
template <garamonMV MV> bool hasGrade(const MV &a, int k)
{
    return !a.isEmpty() && a.isGrade(k);
}

// basis vector 'i' (in display basis order)
template <garamonMV MV> MV basisVector(int i)
{
    for (std::size_t k = 0; k < algebraTraits<MV>::displayBasis.size(); k++)
        if ((bitmapGrade(algebraTraits<MV>::displayBasis[k].bitmap) == 1) && (i-- == 0))
            return basisBlade<MV>(k);
    return MV();
}

// the scalar part
template <garamonMV MV> double scalar(const MV &a)
{
    return a[0];
}

// bitmask of the grades present in 'a' (bit k: grade k)
template <garamonMV MV> unsigned gradeUsage(const MV &a)
{
    unsigned usage = 0;
    for (int k = 0; k <= dimensionOf<MV>(); k++)
        if (hasGrade(a, k))
            usage |= 1u << k;
    return usage;
}

// the parts of 'a' of the grades in 'gradeMask' (bit k: grade k)
template <garamonMV MV> MV takeGrade(const MV &a, unsigned gradeMask)
{
    MV r;
    for (int k = 0; k <= dimensionOf<MV>(); k++)
        if ((gradeMask & (1u << k)) && hasGrade(a, k))
            r += a.grade(k);
    return r;
}

// the sum of the squares of the coordinates of grade 'k'
template <garamonMV MV> double gradeNorm_a(const MV &a, int k)
{
    double n = 0.0;
    for (std::size_t i = 0; i < algebraTraits<MV>::displayBasis.size(); i++)
        if (bitmapGrade(algebraTraits<MV>::displayBasis[i].bitmap) == k)
        {
            double c = a[algebraTraits<MV>::displayBasis[i].bitmap];
            n += c * c;
        }
    return n;
}

// true if a coordinate of grade 'k' has an absolute value > 'epsilon'
template <garamonMV MV> bool gradeAbove(const MV &a, int k, double epsilon)
{
    for (const displayBlade &b : algebraTraits<MV>::displayBasis)
        if ((bitmapGrade(b.bitmap) == k) && (std::fabs(a[b.bitmap]) > epsilon))
            return true;
    return false;
}

// the grade of 'a' if it is homogeneous, -1 if not (0 for a scalar or zero)
template <garamonMV MV> int grade(const MV &a)
{
    unsigned usage = gradeUsage(a);
    for (int g = dimensionOf<MV>(); g > 0; g--)
        if ((usage & (1u << g)) && gradeAbove(a, g, 0.0))
            return (usage ^ (1u << g)) ? -1 : g;
    return 0;
}

// the highest grade with a non-zero coordinate (-1 if none above grade 0)
template <garamonMV MV> int maxGrade(const MV &a)
{
    unsigned usage = gradeUsage(a);
    for (int g = dimensionOf<MV>(); g > 0; g--)
        if ((usage & (1u << g)) && gradeAbove(a, g, 0.0))
            return g;
    return -1;
}

// the grade whose coordinates have the largest sum of squares (the lowest one if equal)
template <garamonMV MV> int largestGrade(const MV &a)
{
    unsigned usage = gradeUsage(a);
    double largest = -1.0;
    int lg = 0;
    for (int g = 0; g <= dimensionOf<MV>(); g++)
        if (usage & (1u << g))
        {
            double n = gradeNorm_a(a, g);
            if (n > largest)
            {
                largest = n;
                lg = g;
            }
        }
    return lg;
}

// the highest grade of 'a' with a coordinate > epsilon; 'part' is set to that grade part of 'a'
// (to zero, and 0 is returned, if there is none)
template <garamonMV MV> int highestGrade(const MV &a, MV &part, double epsilon = defaultEpsilon)
{
    unsigned usage = gradeUsage(a);
    for (int g = dimensionOf<MV>(); g >= 0; g--)
        if ((usage & (1u << g)) && gradeAbove(a, g, epsilon))
        {
            part = a.grade(g);
            return g;
        }
    part = MV();
    return 0;
}

// removes the grades whose coordinates are all <= epsilon (in absolute value)
template <garamonMV MV> MV compress(const MV &a, double epsilon = defaultEpsilon)
{
    MV r(a);
    for (int g = 0; g <= dimensionOf<MV>(); g++)
        if (hasGrade(r, g) && !gradeAbove(r, g, epsilon))
            r.clear(g);
    return r;
}

// sum of the squares of the coordinates
template <garamonMV MV> double norm_a(const MV &a)
{
    double n = 0.0;
    for (const displayBlade &b : algebraTraits<MV>::displayBasis)
        n += a[b.bitmap] * a[b.bitmap];
    return n;
}

// the largest absolute value of a coordinate
template <garamonMV MV> double largestCoordinate(const MV &a)
{
    double m = 0.0;
    for (const displayBlade &b : algebraTraits<MV>::displayBasis)
        m = std::fmax(m, std::fabs(a[b.bitmap]));
    return m;
}

// ---------------------------------------------------------------------------
// involutions, dual
// ---------------------------------------------------------------------------

template <garamonMV MV> MV reverse(const MV &a)
{
    return a.reverse();
}

template <garamonMV MV> MV gradeInvolution(const MV &a)
{
    MV r;
    for (int g = 0; g <= dimensionOf<MV>(); g++)
        if (hasGrade(a, g))
            r += (g & 1) ? MV(-a.grade(g)) : a.grade(g);
    return r;
}

template <garamonMV MV> MV cliffordConjugate(const MV &a)
{
    return reverse(gradeInvolution(a));
}

template <garamonMV MV> MV negate(const MV &a)
{
    return -a;
}

// A I^-1
template <garamonMV MV> MV dual(const MV &a)
{
    return a * algebraTraits<MV>::Iinv();
}

// ---------------------------------------------------------------------------
// products
// ---------------------------------------------------------------------------

template <garamonMV MV> MV gp(const MV &a, const MV &b)
{
    return a * b;
}

template <garamonMV MV> MV op(const MV &a, const MV &b)
{
    return a ^ b;
}

template <garamonMV MV> MV lcont(const MV &a, const MV &b)
{
    return a < b;
}

template <garamonMV MV> MV rcont(const MV &a, const MV &b)
{
    return a > b;
}

// scalar product <a b>
template <garamonMV MV> MV scp(const MV &a, const MV &b)
{
    return (a * b).grade(0);
}

namespace detail
{
// sum over the grade parts a_r, b_s of <a_r b_s>_|r-s|, with or without the scalars
template <garamonMV MV> MV innerProduct(const MV &a, const MV &b, bool withScalars)
{
    MV r;
    for (int ga = 0; ga <= dimensionOf<MV>(); ga++)
    {
        if (!hasGrade(a, ga) || (!withScalars && ga == 0))
            continue;
        for (int gb = 0; gb <= dimensionOf<MV>(); gb++)
        {
            if (!hasGrade(b, gb) || (!withScalars && gb == 0))
                continue;
            r += (a.grade(ga) * b.grade(gb)).grade(std::abs(ga - gb));
        }
    }
    return r;
}
} // namespace detail

// Hestenes inner product (0 if an operand is a scalar)
template <garamonMV MV> MV hip(const MV &a, const MV &b)
{
    return detail::innerProduct(a, b, false);
}

// modified Hestenes inner product (scalars included)
template <garamonMV MV> MV mhip(const MV &a, const MV &b)
{
    return detail::innerProduct(a, b, true);
}

namespace detail
{
// the sign of the product of the basis blades with bitmaps a and b (the number of
// swaps that bring the factors of a b into the canonical order, i.e. ascending bits)
constexpr double reorderingSign(unsigned a, unsigned b)
{
    a >>= 1;
    int swaps = 0;
    while (a)
    {
        swaps += bitmapGrade(a & b);
        a >>= 1;
    }
    return (swaps & 1) ? -1.0 : 1.0;
}

// The geometric product (leftContraction = false) or the left contraction (true) of a
// and b with the Euclidean metric (all basis vectors orthonormal), computed on the
// basis blades of the Garamon algebra.
template <garamonMV MV> MV euclideanProduct(const MV &a, const MV &b, bool leftContraction)
{
    constexpr unsigned N = 1u << dimensionOf<MV>();
    MV r;
    for (unsigned i = 0; i < N; i++)
    {
        const double ca = a[i];
        if (ca == 0.0)
            continue;
        for (unsigned j = 0; j < N; j++)
        {
            const double cb = b[j];
            if ((cb == 0.0) || (leftContraction && ((i & ~j) != 0))) // A _| B: the factors of A must be in B
                continue;
            r[i ^ j] += reorderingSign(i, j) * ca * cb;
        }
    }
    return compress(r, 0.0); // (no null grade parts)
}
} // namespace detail

// The products with the Euclidean metric: all basis vectors are taken as orthonormal
// (e.g. no.no = 1, no.ni = 0 in c3ga), as Gaigen's gpem and lcem; in a Euclidean
// algebra the same as gp and lcont.
template <garamonMV MV> MV gpem(const MV &a, const MV &b)
{
    if constexpr (algebraTraits<MV>::euclideanMetric)
        return gp(a, b);
    else
        return detail::euclideanProduct(a, b, false);
}

template <garamonMV MV> MV lcem(const MV &a, const MV &b)
{
    if constexpr (algebraTraits<MV>::euclideanMetric)
        return lcont(a, b);
    else
        return detail::euclideanProduct(a, b, true);
}

// the scalar product with the Euclidean metric
template <garamonMV MV> MV scpem(const MV &a, const MV &b)
{
    return gpem(a, b).grade(0);
}

// ---------------------------------------------------------------------------
// inverses, norms
// ---------------------------------------------------------------------------

// ~a / <a ~a>; 'ok' is set to false (and zero is returned) if <a ~a> == 0
template <garamonMV MV> MV versorInverse(const MV &a, bool *ok = nullptr)
{
    MV r = reverse(a);
    double s = scalar(scp(a, r));
    if (ok)
        *ok = (s != 0.0);
    if (s == 0.0)
        return MV();
    return r * (1.0 / s);
}

// ~a / <a ~a> with the Euclidean metric; zero ('ok' false) if <a ~a> == 0
template <garamonMV MV> MV versorInverseEM(const MV &a, bool *ok = nullptr)
{
    MV r = reverse(a);
    double s = scalar(scpem(a, r));
    if (ok)
        *ok = (s != 0.0);
    if (s == 0.0)
        return MV();
    return r * (1.0 / s);
}

template <garamonMV MV> MV inverse(const MV &a)
{
    return versorInverse(a);
}

// a * b^-1
template <garamonMV MV> MV igp(const MV &a, const MV &b)
{
    return gp(a, inverse(b));
}

// the inverse of any invertible multivector (solves a x = 1); zero if 'a' is singular
template <garamonMV MV> MV generalInverse(const MV &a)
{
    constexpr std::size_t n = std::size_t(1) << dimensionOf<MV>();
    // m[row][col]: column j = coordinates of a * (basis blade j)
    std::vector<std::vector<double>> m(n, std::vector<double>(n));
    for (std::size_t j = 0; j < n; j++)
    {
        std::vector<double> c = coordinates(MV(a * basisBlade<MV>(j)));
        for (std::size_t i = 0; i < n; i++)
            m[i][j] = c[i];
    }
    std::vector<double> x(n, 0.0);
    x[0] = 1.0; // right-hand side: the scalar 1

    // Gauss-Jordan elimination with partial pivoting
    for (std::size_t i = 0; i < n; i++)
    {
        std::size_t pi = i;
        for (std::size_t j = i + 1; j < n; j++)
            if (std::fabs(m[j][i]) > std::fabs(m[pi][i]))
                pi = j;
        std::swap(m[pi], m[i]);
        std::swap(x[pi], x[i]);
        double pivot = m[i][i];
        if (pivot == 0.0) // singular
            return MV();
        for (std::size_t k = 0; k < n; k++)
        {
            if (k == i)
                continue;
            double mult = -m[k][i] / pivot;
            for (std::size_t j = i + 1; j < n; j++)
                m[k][j] += m[i][j] * mult;
            x[k] += x[i] * mult;
        }
        for (std::size_t j = i + 1; j < n; j++)
            m[i][j] /= pivot;
        x[i] /= pivot;
    }
    return fromCoordinates<MV>(x);
}

// <a ~a>
template <garamonMV MV> double norm_b(const MV &a)
{
    return scalar(scp(a, reverse(a)));
}

// a / sqrt(norm_a(a)) (norm = 1) or a / sqrt(|norm_b(a)|) (norm = 2)
template <garamonMV MV> MV normalize(const MV &a, int norm = 1)
{
    double s = (norm == 2) ? 1.0 / std::sqrt(std::fabs(norm_b(a))) : 1.0 / std::sqrt(norm_a(a));
    return a * s;
}

// exp(a), Taylor series of 'order' terms, with scaling and squaring
template <garamonMV MV> MV exp(const MV &a, int order = 9)
{
    MV result(1.0);
    if (order == 0)
        return result;

    // scale by a power of 2 so that the norm is < 1
    unsigned long max = (unsigned long)largestCoordinate(a);
    unsigned long scale = 1;
    if (max > 1)
        scale <<= 1;
    while (max)
    {
        max >>= 1;
        scale <<= 1;
    }
    MV scaled = a * (1.0 / (double)scale);

    // Taylor approximation
    MV term(1.0);
    for (int i = 1; i < order; i++)
    {
        term = term * scaled * (1.0 / (double)i);
        result += term;
    }

    // undo the scaling
    while (scale > 1)
    {
        result = result * result;
        scale >>= 1;
    }
    return result;
}

// ---------------------------------------------------------------------------
// blades, versors
// ---------------------------------------------------------------------------

// blade or versor? Returns mvTypeBlade, mvTypeVersor or mvTypeMultivector; 'gradeMask'
// is set to the grade (as a bitmask: 1 << grade) of a blade (to 0 otherwise)
template <garamonMV MV> int mvType(const MV &a, int *gradeMask = nullptr, double epsilon = defaultEpsilon)
{
    // (this test fails on 'e1 + (e2 ^ e3)': it says versor, while it is not; as in Gaigen)
    if (gradeMask)
        *gradeMask = 0;

    // versor inverse
    MV r = reverse(a);
    double s = scalar(scpem(a, r));
    if (s == 0.0)
        return mvTypeMultivector;
    MV ai = r * (1.0 / s);

    auto isOne = [epsilon](const MV &x) {
        unsigned usage = gradeUsage(x);
        if (usage > 1u)
            return false;
        if (usage == 0)
            return false;
        double c = scalar(x);
        return (c >= 1.0 - epsilon) && (c <= 1.0 + epsilon);
    };
    // check if ai a = 1 and a ai = 1
    if (!isOne(compress(gpem(ai, a), epsilon)) || !isOne(compress(gpem(a, ai), epsilon)))
        return mvTypeMultivector;

    // test if ai x a is of the same grade as x for all basis vectors x
    for (const displayBlade &b : algebraTraits<MV>::displayBasis)
    {
        if (bitmapGrade(b.bitmap) != 1)
            continue;
        MV x;
        x[b.bitmap] = 1.0;
        if (gradeUsage(compress(gpem(gpem(ai, x), a), epsilon)) & ~2u)
            return mvTypeMultivector;
    }

    // normalize 'a' (to make sure 'small' (< epsilon) versors are not taken for blades)
    MV an = compress(MV(a * std::sqrt(std::fabs(1.0 / s))), epsilon);

    // test if 'a' is of a single grade
    unsigned usage = gradeUsage(an);
    int count = 0;
    for (int g = 0; usage; g++, usage >>= 1)
        if (usage & 1)
        {
            if (gradeMask)
                *gradeMask = 1 << g;
            count++;
        }
    return (count <= 1) ? mvTypeBlade : mvTypeVersor;
}

// (a _| blade) _| blade^-1
template <garamonMV MV> MV project(const MV &blade, const MV &a)
{
    return lcont(lcont(a, blade), versorInverse(blade));
}

// (a _| blade) _| blade^-1 with the Euclidean metric
template <garamonMV MV> MV projectEM(const MV &blade, const MV &a)
{
    return lcem(lcem(a, blade), versorInverseEM(blade));
}

// blade^-1 _| (blade ^ a)
template <garamonMV MV> MV reject(const MV &blade, const MV &a)
{
    return lcont(versorInverse(blade), op(blade, a));
}

// Factors the blade (or versor, if 'versor' is true) 'a' into vectors: their outer
// (geometric) product is 'a'. Returns the number of factors, stored in 'factors'
// (the factors are scaled to be of about the same size).
template <garamonMV MV> int factor(const MV &a, std::vector<MV> &factors, bool versor = false)
{
    factors.clear();
    int ga = versor ? maxGrade(a) : grade(a);
    if (ga <= 0)
        return 0;

    // the multiplication factor that makes all factors of the same order of magnitude
    double mul = std::pow(std::sqrt(norm_a(a)), 1.0 / (double)ga);

    // the basis blade of grade 'ga' with the largest coordinate (the first one, in display order)
    const auto &basis = algebraTraits<MV>::displayBasis;
    unsigned largest = 0;
    double maxC = -1.0;
    for (const displayBlade &b : basis)
        if ((bitmapGrade(b.bitmap) == ga) && (std::fabs(a[b.bitmap]) > maxC))
        {
            maxC = std::fabs(a[b.bitmap]);
            largest = b.bitmap;
        }

    // the basis vectors of that blade, in display order (except the first basis vector
    // of the display basis, which Gaigen always skips)
    std::vector<MV> vectors;
    bool first = true;
    for (const displayBlade &b : basis)
    {
        if (bitmapGrade(b.bitmap) != 1)
            continue;
        if (first)
        {
            first = false;
            continue;
        }
        if (largest & b.bitmap)
        {
            MV v;
            v[b.bitmap] = b.sign;
            vectors.push_back(v);
        }
    }

    MV ca(a);
    for (std::size_t i = 0; ga > 1; ga--, i++)
    {
        MV f = projectEM(ca, vectors[i]) * mul;
        factors.push_back(f);
        // remove the factor from the blade
        MV fi = versorInverseEM(f);
        ca = versor ? gpem(fi, ca) : lcem(fi, ca);
    }
    factors.push_back(takeGrade(ca, 2u)); // the last factor is the remaining vector
    return (int)factors.size();
}

// the highest grade part of a b ('d'); returns its grade
template <garamonMV MV> int deltaProduct(const MV &a, const MV &b, MV &d)
{
    return highestGrade(gpem(a, b), d);
}

namespace detail
{
// the part of 'a' of its grade, or of its grade part with the largest norm if it is not homogeneous
template <garamonMV MV> MV homogeneousPart(const MV &a, int &g)
{
    g = grade(a);
    if (g >= 0)
        return a;
    g = largestGrade(a);
    return takeGrade(a, 1u << g);
}
} // namespace detail

// the join of two blades
template <garamonMV MV> MV join(const MV &a, const MV &b)
{
    int ga, gb;
    MV ca = detail::homogeneousPart(a, ga), cb = detail::homogeneousPart(b, gb);

    if (ga == 0) // the join is b
        return op(ca, cb);
    if (gb == 0) // the join is a
        return op(cb, ca);

    MV d;
    int gd = deltaProduct(ca, cb, d);
    int gj = (ga + gb + gd) >> 1;

    if (gj == dimensionOf<MV>()) // the pseudoscalar
        return algebraTraits<MV>::I();

    // start with the blade of the highest grade (b1), project all basis blades of the
    // required grade (rg, possibly 0) onto the other one (b2), wedge them to b1 and keep
    // the largest result
    const MV &b1 = (ga > gb) ? ca : cb;
    const MV &b2 = (ga > gb) ? cb : ca;
    int rg = gj - ((ga > gb) ? ga : gb);

    MV result;
    double largestNorm = -1.0;
    for (const displayBlade &bb : algebraTraits<MV>::displayBasis)
    {
        if (bitmapGrade(bb.bitmap) != rg)
            continue;
        MV be;
        be[bb.bitmap] = bb.sign;
        MV t = op(b1, projectEM(b2, be));
        double nm = norm_a(t);
        if (nm > largestNorm)
        {
            largestNorm = nm;
            result = t;
        }
    }
    return result;
}

// the meet of two blades
template <garamonMV MV> MV meet(const MV &a, const MV &b)
{
    int ga, gb;
    MV ca = detail::homogeneousPart(a, ga), cb = detail::homogeneousPart(b, gb);

    MV d;
    deltaProduct(ca, cb, d);
    bool ok;
    MV jinv = versorInverse(join(ca, cb), &ok);
    if (!ok)
        return MV();
    return lcem(d, jinv);
}

} // namespace ga

#endif /* _GA_ALGORITHMS_H_ */
