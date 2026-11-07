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

#include "osdep.h"
#include "tubedraw.h"
#include "tubegeometry.h"

using namespace tubegeometry;

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

        std::vector<vec3> tubeVertices(m_nbSegments * (endIdx - startIdx));
        std::vector<vec3> tubeDirections(endIdx - startIdx);

        // compute initial vertices (factor the dual of the first direction, or use e1 if l = 0)
        vec3 prevDir;
        {
            // first direction
            vec3 dir = m_vertices[startIdx + 1] - m_vertices[startIdx];
            if (dot(dir, dir) == 0)
                dir = {1.0, 0.0, 0.0};
            else
                dir = normalize(dir);
            prevDir = dir;
            tubeDirections[0] = dir;

            // get factors:
            vec3 f0, f1;
            initialFrame(dir, f0, f1);

            // construct first segments:
            for (int i = 0; i < m_nbSegments; i++)
            {
                double a = (double)i * 2 * M_PI / m_nbSegments;
                tubeVertices[startIdx * m_nbSegments + i] = cos(a) * f0 + sin(a) * f1;
            }
        }

        // then use minimal rotation to get from one to the next
        startIdx++;
        while ((startIdx + 1) < endIdx)
        {
            // compute direction of this segment:
            vec3 newDir = m_vertices[startIdx + 1] - m_vertices[startIdx];
            if (dot(newDir, newDir) == 0)
                newDir = prevDir;
            else
                newDir = normalize(newDir);
            tubeDirections[startIdx] = newDir;

            // the rotation from prev segment to current segment
            const frameRotation R(prevDir, newDir);

            // rotate all vertices of the previous segment:
            for (int i = 0; i < m_nbSegments; i++)
                tubeVertices[startIdx * m_nbSegments + i] = R(tubeVertices[(startIdx - 1) * m_nbSegments + i]);

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
            std::vector<vec3> avgTubeVertices(m_nbSegments * (endIdx - startIdx));
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
                const vec3 &d1 = tubeDirections[i];
                const vec3 &d2 = tubeDirections[prevIdx];

                for (int s = 0; s < m_nbSegments; s++)
                    avgTubeVertices[i * m_nbSegments + s] = edgeJoin(
                        tubeVertices[i * m_nbSegments + s], d1, tubeVertices[prevIdx * m_nbSegments + s], d2);
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

void TubeDraw::drawTubeSegment(int vIdx, const std::vector<vec3> &vertices)
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
        const vec3 &n1 = vertices[vIdx * m_nbSegments + i];
        glNormal3dv(n1.data());
        glVertex3dv((m_vertices[vIdx] + lw * n1).data());

        const vec3 &n2 = vertices[(vIdxp1)*m_nbSegments + i];
        glNormal3dv(n2.data());
        glVertex3dv((m_vertices[vIdxp1] + lw * n2).data());
    }
    glEnd();
}

void TubeDraw::drawTubeCap(int vIdx, const std::vector<vec3> &vertices, bool endCap)
{
    double lw = m_lineWidth / 250.0;

    glBegin(GL_TRIANGLE_FAN);
    // (the normalized dual of the bivector v0 ^ v1: v0 x v1)
    glNormal3dv(normalize(cross(vertices[vIdx * m_nbSegments + 0], vertices[vIdx * m_nbSegments + 1])).data());
    glVertex3dv(m_vertices[vIdx].data());
    for (int _i = 0; _i <= m_nbSegments; _i++)
    {
        int i = (_i) % m_nbSegments;
        if (endCap)
            i = m_nbSegments - 1 - i; // flip order on end cap
        const vec3 &n1 = vertices[vIdx * m_nbSegments + i];
        glVertex3dv((m_vertices[vIdx] + lw * n1).data());
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
        m_vertices.push_back({x, y, z});
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
