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

#ifndef _TUBE_GEOMETRY_H_
#define _TUBE_GEOMETRY_H_

/*
Plain 3D vector arithmetic for the drawing code that runs on every repaint
(TubeDraw draws every line as a tube, drawVector() the arrow heads).

These computations used to be written with multivectors (e3ga rotors, the
factorization of a bivector, a p3ga line intersection). A Garamon multivector
operation allocates memory and is far slower than the Gaigen one was (about
60 times in an MSVC Debug build), which made drawing slow; the functions
below compute exactly the same geometry with plain 3D vectors (each one notes
the multivector expression it replaces; viewer/test/tubegeometry checks them
against those expressions).
*/

#include <array>
#include <cmath>

namespace tubegeometry
{

using vec3 = std::array<double, 3>;

inline vec3 operator+(const vec3 &a, const vec3 &b)
{
    return {a[0] + b[0], a[1] + b[1], a[2] + b[2]};
}
inline vec3 operator-(const vec3 &a, const vec3 &b)
{
    return {a[0] - b[0], a[1] - b[1], a[2] - b[2]};
}
inline vec3 operator*(double s, const vec3 &a)
{
    return {s * a[0], s * a[1], s * a[2]};
}
inline double dot(const vec3 &a, const vec3 &b)
{
    return a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
}
inline vec3 cross(const vec3 &a, const vec3 &b)
{
    return {a[1] * b[2] - a[2] * b[1], a[2] * b[0] - a[0] * b[2], a[0] * b[1] - a[1] * b[0]};
}
// a / |a| (a / sqrt(norm_a(a)), as Gaigen's normal())
inline vec3 normalize(const vec3 &a)
{
    return (1.0 / std::sqrt(dot(a, a))) * a;
}

/*
The rotation of the tube cross-section from one segment direction (unit vector
a) to the next one (unit vector b): the rotor R with R a R^-1 = b, applied as
v -> R v R^-1 (formerly rotorFromVectorToVector() and an e3ga sandwich product).

- Usually R = (1 + b a) / |1 + b a| = b n with n = (a + b) / |a + b|, i.e. two
  reflections: v -> n v n = 2 (n.v) n - v, then -> b (.) b.
- When a and b are (nearly) opposite (a.b < -0.99999), R is the unit bivector
  of a plane containing a (spanned by a and the part of b perpendicular to a,
  or else e1 or e2): a rotation over 180 degrees in that plane, which negates
  the components of v in the plane.
*/
class frameRotation
{
  public:
    frameRotation(const vec3 &a, const vec3 &b)
    {
        if (dot(a, b) < -0.99999)
        {
            m_halfTurn = true;
            vec3 t = dot(a, a) * b - dot(a, b) * a; // a _| (a ^ b)
            if (dot(t, t) == 0.0)
            {
                const vec3 e1 = {1.0, 0.0, 0.0}, e2 = {0.0, 1.0, 0.0};
                const vec3 ae1 = cross(a, e1), ae2 = cross(a, e2); // (norm_a(a ^ e1) = |a x e1|^2)
                t = (dot(ae1, ae1) > dot(ae2, ae2)) ? e1 : e2;
            }
            // an orthonormal basis of the plane of a and t
            m_e = normalize(a);
            m_f = normalize(t - dot(t, m_e) * m_e);
        }
        else
        {
            m_halfTurn = false;
            m_n = normalize(a + b);
            m_b = b;
        }
    }

    vec3 operator()(const vec3 &v) const
    {
        if (m_halfTurn)
            return v - (2.0 * dot(v, m_e)) * m_e - (2.0 * dot(v, m_f)) * m_f;
        const vec3 u = (2.0 * dot(m_n, v)) * m_n - v;
        return (2.0 * dot(m_b, u)) * m_b - u;
    }

  private:
    bool m_halfTurn;
    vec3 m_n, m_b; // the reflections
    vec3 m_e, m_f; // the plane of the half turn
};

/*
The initial cross-section frame of a tube with direction 'd': the two factors
f0, f1 of the bivector dual(d) = d I^-1 as computed by Gaigen's factor() (and
ga::factor): the basis vector e_k of the basis blade with the largest
coordinate of dual(d) (e2 for e2^e3, e3 for e3^e1, e2 for e1^e2: e1 is always
skipped) projected onto the plane perpendicular to d and multiplied by
|d|^(1/2), and then f1 = f0^-1 _| dual(d) = (f0 x d) / |f0|^2.
*/
inline void initialFrame(const vec3 &d, vec3 &f0, vec3 &f1)
{
    // dual(d) = -(d0 e2^e3 + d1 e3^e1 + d2 e1^e2): its largest coordinate (the first one if equal)
    int largest = 0;
    double maxC = -1.0;
    for (int k = 0; k < 3; k++)
        if (std::fabs(d[k]) > maxC)
        {
            maxC = std::fabs(d[k]);
            largest = k;
        }
    vec3 v = {0.0, 0.0, 0.0};
    v[(largest == 1) ? 2 : 1] = 1.0; // e2^e3 -> e2, e3^e1 -> e3, e1^e2 -> e2

    const double mul = std::pow(std::sqrt(dot(d, d)), 0.5);
    f0 = mul * (v - (dot(v, d) / dot(d, d)) * d);
    f1 = (1.0 / dot(f0, f0)) * cross(f0, d);
}

/*
Where tube edge 2 (through p2, direction d2) meets tube edge 1 (through p1,
direction d1): the p3ga expression I = (L1 _| P) _| L2 with the lines
L1 = p1 ^ d1, L2 = p2 ^ d2 and the plane P = L1 ^ p2 (points with e0 = 1,
directions with e0 = 0, Euclidean metric in 4D), I / I.e0. L1 _| P is (up to a
factor) q, the part of p2 perpendicular (in 4D) to p1 and d1, so the result
is p2 - ((q.p2) / (q.d2)) d2. If p1 and p2 (nearly) coincide, i.e.
norm_a(p1 ^ p2) = |p1 x p2|^2 + |p1 - p2|^2 < 1e-7, the result is p1.
*/
inline vec3 edgeJoin(const vec3 &p1, const vec3 &d1, const vec3 &p2, const vec3 &d2)
{
    const vec3 c = cross(p1, p2), diff = p1 - p2;
    if (dot(c, c) + dot(diff, diff) < 1e-7)
        return p1;

    using vec4 = std::array<double, 4>;
    auto dot4 = [](const vec4 &a, const vec4 &b) { return a[0] * b[0] + a[1] * b[1] + a[2] * b[2] + a[3] * b[3]; };
    auto sub4 = [](const vec4 &a, double s, const vec4 &b) {
        return vec4{a[0] - s * b[0], a[1] - s * b[1], a[2] - s * b[2], a[3] - s * b[3]};
    };
    const vec4 a = {p1[0], p1[1], p1[2], 1.0}, b = {d1[0], d1[1], d1[2], 0.0};
    const vec4 P2 = {p2[0], p2[1], p2[2], 1.0}, D2 = {d2[0], d2[1], d2[2], 0.0};
    // Gram-Schmidt: an orthonormal basis (u, w) of span(a, b), then q = P2 minus its projection
    const vec4 u = sub4(a, 0.0, a);
    const double uu = dot4(u, u);
    const vec4 w = sub4(b, dot4(b, u) / uu, u);
    const double ww = dot4(w, w);
    vec4 q = sub4(P2, dot4(P2, u) / uu, u);
    if (ww != 0.0)
        q = sub4(q, dot4(P2, w) / ww, w);
    const double lambda = -dot4(q, P2) / dot4(q, D2);
    return p2 + lambda * d2;
}

} // namespace tubegeometry

#endif /* _TUBE_GEOMETRY_H_ */
