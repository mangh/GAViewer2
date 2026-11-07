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

#include <cmath>

#include <vector>

#include "gl_includes.h"

#include "gaincludes.h"
#include "osdep.h"
#include "ga/p3ga.h"
#include "tubedraw.h"

e3ga::mv rotorFromVectorToVector(const e3ga::mv &v1, const e3ga::mv &v2)
{
    if (ga::scalar(ga::scp(v1, v2)) < -0.99999f)
    {
        // (near) 180 degree rotation:
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
    else
    {
        GAIM_FLOAT s = (GAIM_FLOAT)sqrt(2.0 * (1.0f + ga::scalar(ga::scp(v2, v1))));
        return (1.0 + v2 * v1) * (1.0f / s);
    }
}

TubeDraw::TubeDraw()
{
    m_what = -1;

    m_lineWidth = 2.0;

    m_nbSegments = 6;

    m_useOpenGL = false;
}

void TubeDraw::begin(int what)
{
    if (m_useOpenGL)
    {
        glLineWidth((float)m_lineWidth);
        glBegin(what);
    }
    else
    {
        m_vertices.clear();
        m_what = what;
    }
}

void TubeDraw::end()
{
    // actually draw stuff
    if (m_useOpenGL)
    {
        glEnd();
    }
    else
    {
        if (m_vertices.size() < 2)
            return;

        if (m_what == GL_LINE_LOOP)
        {
            // if loop: last vertex = first vertex
            m_vertices.push_back(m_vertices[0]);
        }
        else if (m_what == GL_LINE_STRIP)
        {
            // last vertex = extra copy of last vertex
            m_vertices.push_back(m_vertices[m_vertices.size() - 1]);
        }

        // vector of segment vertices (m_nbSegments for each enty in m_vertices)
        unsigned int startIdx = 0;
        unsigned int endIdx = (unsigned int)m_vertices.size();

        std::vector<e3ga::mv> tubeVertices(m_nbSegments * (endIdx - startIdx));
        std::vector<e3ga::mv> tubeDirections(endIdx - startIdx);

        // compute initial vertices (factor the dual of the first direction, or use e1 if l = 0)
        e3ga::mv prevDir;
        {
            // first direction
            e3ga::mv dir = m_vertices[startIdx + 1] - m_vertices[startIdx];
            if (ga::norm_a(dir) == 0)
                dir = e3ga::e1<double>();
            else
                dir = ga::normalize(dir);
            prevDir = dir;
            tubeDirections[0] = dir;

            // get factors:
            e3ga::mv plane = ga::dual(dir);
            std::vector<e3ga::mv> factors;
            ga::factor(plane, factors);

            // construct first segments:
            for (int i = 0; i < m_nbSegments; i++)
            {
                double a = (double)i * 2 * M_PI / m_nbSegments;
                tubeVertices[startIdx * m_nbSegments + i] = factors[0] * cos(a) + factors[1] * sin(a);
            }
        }

        // then use minimal rotation to get from one to the next
        startIdx++;
        while ((startIdx + 1) < endIdx)
        {
            // compute direction of this segment:
            e3ga::mv newDir = m_vertices[startIdx + 1] - m_vertices[startIdx];
            if (ga::norm_a(newDir) == 0)
                newDir = prevDir;
            else
                newDir = ga::normalize(newDir);
            tubeDirections[startIdx] = newDir;

            // compute rotor from prev segment to current segment
            e3ga::mv R = rotorFromVectorToVector(prevDir, newDir);
            e3ga::mv Ri = ga::inverse(R);

            // rotate all vertices of the previous segment:
            for (int i = 0; i < m_nbSegments; i++)
            {
                tubeVertices[startIdx * m_nbSegments + i] =
                    ga::takeGrade(e3ga::mv(R * tubeVertices[(startIdx - 1) * m_nbSegments + i] * Ri), GRADE1);
            }

            prevDir = newDir;
            startIdx++;
        }
        startIdx = 0;

        // DRAW:
        // if GL_LINES, just go ahead, no averaging required
        if (m_what == GL_LINES)
        {
            for (unsigned int i = 0; (i + 1) < m_vertices.size(); i += 2)
            {
                // only change: overwrite all odd segments:
                for (int s = 0; s < m_nbSegments; s++)
                    tubeVertices[(i + 1) * m_nbSegments + s] = tubeVertices[i * m_nbSegments + s];

                drawTubeCap(i, tubeVertices, false);
                drawTubeSegment(i, tubeVertices);
                drawTubeCap(i + 1, tubeVertices, true);
            }
        }
        else
        {
            if (m_what == GL_LINE_STRIP)
            {
                drawTubeCap(0, tubeVertices, false);
                drawTubeCap((int)m_vertices.size() - 2, tubeVertices, true);
            }

            // average segments:
            std::vector<e3ga::mv> avgTubeVertices(m_nbSegments * (endIdx - startIdx));
            int i = 1;
            if (m_what == GL_LINE_LOOP)
                i = 0;
            else
            {
                for (int s = 0; s < m_nbSegments; s++)
                    avgTubeVertices[0 * m_nbSegments + s] = tubeVertices[0 * m_nbSegments + s];
            }

            // test: don't average, but intersect edges!
            for (; i < (int)m_vertices.size(); i++)
            {
                int prevIdx = i - 1;
                if (prevIdx < 0)
                    prevIdx = ((int)m_vertices.size()) - 2;
                const e3ga::mv &td1 = tubeDirections[i];
                const e3ga::mv &td2 = tubeDirections[prevIdx];
                p3ga::mv d1 = p3ga::vector(td1[e3ga::E1], td1[e3ga::E2], td1[e3ga::E3], 0);
                p3ga::mv d2 = p3ga::vector(td2[e3ga::E1], td2[e3ga::E2], td2[e3ga::E3], 0);

                for (int s = 0; s < m_nbSegments; s++)
                {
                    const e3ga::mv &tp1 = tubeVertices[i * m_nbSegments + s];
                    const e3ga::mv &tp2 = tubeVertices[prevIdx * m_nbSegments + s];

                    p3ga::mv p1 = p3ga::vector(tp1[e3ga::E1], tp1[e3ga::E2], tp1[e3ga::E3], 1);
                    p3ga::mv p2 = p3ga::vector(tp2[e3ga::E1], tp2[e3ga::E2], tp2[e3ga::E3], 1);

                    p3ga::mv L1 = p1 ^ d1;
                    p3ga::mv L2 = p2 ^ d2;

                    if (ga::norm_a(p3ga::mv(p1 ^ p2)) < 1e-7)
                    {
                        avgTubeVertices[i * m_nbSegments + s] = tp1;
                    }
                    else
                    {
                        p3ga::mv P = p1 ^ d1 ^ p2;
                        p3ga::mv I = ga::lcont(ga::lcont(L1, P), L2);
                        GAIM_FLOAT Iw = I[p3ga::E0];
                        avgTubeVertices[i * m_nbSegments + s] =
                            e3ga::vector(I[p3ga::E1] / Iw, I[p3ga::E2] / Iw, I[p3ga::E3] / Iw);
                    }
                }
            }

            /*
            // average them:
            for (; i < (int)m_vertices.size(); i++) {
                int prevIdx = i-1;
                if (prevIdx < 0) prevIdx = ((int)m_vertices.size()) - 2;
                for (int s = 0; s < m_nbSegments; s++) {
                    avgTubeVertices[i * m_nbSegments + s] =
                        (tubeVertices[i * m_nbSegments + s] + tubeVertices[prevIdx * m_nbSegments + s]).normal();
                }
            }
            */

            // actually draw
            unsigned int nbVert = (unsigned int)m_vertices.size();
            if (m_what == GL_LINE_STRIP)
            {
                nbVert--;
            }
            for (unsigned int i = 0; (i + 1) < nbVert; i++)
            {
                drawTubeSegment(i, avgTubeVertices);
            }
        }
    }
}

void TubeDraw::drawTubeSegment(int vIdx, const std::vector<e3ga::mv> &vertices)
{
    double lw = m_lineWidth / 250.0;

    int vIdxp1 = vIdx + 1;
    if (m_what == GL_LINE_LOOP)
        if (vIdxp1 > (int)(m_vertices.size() - 2))
            vIdxp1 = 0;
    // use QUAD_STRIP to draw each segment, set normals properly
    glBegin(GL_QUAD_STRIP);
    for (int _i = 0; _i <= m_nbSegments; _i++)
    {
        int i = (_i) % m_nbSegments;
        const e3ga::mv &n1 = vertices[vIdx * m_nbSegments + i];
        glNormal3dv(e3ga::xyz(n1).data());
        glVertex3dv(e3ga::xyz(m_vertices[vIdx] + lw * n1).data());

        const e3ga::mv &n2 = vertices[(vIdxp1)*m_nbSegments + i];
        glNormal3dv(e3ga::xyz(n2).data());
        glVertex3dv(e3ga::xyz(m_vertices[vIdxp1] + lw * n2).data());
    }
    glEnd();
}

void TubeDraw::drawTubeCap(int vIdx, const std::vector<e3ga::mv> &vertices, bool endCap)
{
    double lw = m_lineWidth / 250.0;

    glBegin(GL_TRIANGLE_FAN);
    glNormal3dv(
        e3ga::xyz(ga::normalize(ga::dual(e3ga::mv(vertices[vIdx * m_nbSegments + 0] ^ vertices[vIdx * m_nbSegments + 1]))))
            .data());
    glVertex3dv(e3ga::xyz(m_vertices[vIdx]).data());
    for (int _i = 0; _i <= m_nbSegments; _i++)
    {
        int i = (_i) % m_nbSegments;
        if (endCap)
            i = m_nbSegments - 1 - i; // flip order on end cap
        const e3ga::mv &n1 = vertices[vIdx * m_nbSegments + i];
        glVertex3dv(e3ga::xyz(m_vertices[vIdx] + lw * n1).data());
    }
    glEnd();
}

void TubeDraw::vertex2d(double x, double y)
{
    vertex3d(x, y, 0.0);
}

void TubeDraw::vertex3d(double x, double y, double z)
{
    if (m_useOpenGL)
    {
        glVertex3d(x, y, z);
    }
    else
    {
        m_vertices.push_back(e3ga::vector(x, y, z));
    }
}

void TubeDraw::vertex2dv(const double *c)
{
    vertex3d(c[0], c[1], 0.0);
}

void TubeDraw::vertex3dv(const double *c)
{
    vertex3d(c[0], c[1], c[2]);
}

/// sets line width to 'w'
void TubeDraw::setLineWidth(double w)
{
    m_lineWidth = w;
}

/// enables / disables forwarding to OpenGL
void TubeDraw::setUseOpenGL(bool v)
{
    m_useOpenGL = v;
}

void TubeDraw::setNbSegments(int nb)
{
    m_nbSegments = nb;
}

double TubeDraw::getLineWidth()
{
    return m_lineWidth;
}

bool TubeDraw::getUseOpenGL()
{
    return m_useOpenGL;
}
