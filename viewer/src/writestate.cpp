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

#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <set>
#include <string>

#include <vector>

#include "console/consolecontrol.h"
#include "console/consolescope.h"
#include "console/consolestatement.h"
#include "console/consolevariable.h"
#include "gaincludes.h"
#include "geosphere.h"
#include "glwindow.h"
#include "mainwindow.h"
#include "models.h"
#include "object.h"
#include "state.h"
#include "uistate.h"
#include "util.h"

// A multivector as text for a .geo file: exactly (17 significant digits of the double), unlike the
// console, which prints the coordinates as floats (as Gaigen did).
template <class MV> static std::string geoText(const MV &mv)
{
    return ga::toString(mv, "%.17e", false);
}

static inline void printColor(FILE *F, const char *colName, float c[4])
{
    fprintf(F, "%s %f %f %f %f\n", colName, c[0], c[1], c[2], c[3]);
}

int state::writeStateToGeoFile(const char *filename)
{
    int i;
    state::OI oi;

    FILE *F = fopen(filename, "w+");
    if (F == NULL)
    {
        cprintf("state::writeStateToGeoFile(): can not open '%s' for writing.\n", filename);
        return -1;
    }

    fprintf(F, "clear\n");

    for (i = 0; i < 3; i++)
        fprintf(F, "factor e3ga %d [%s]\n", i + 1, geoText(m_e3gaFactor[i]).c_str());
    for (i = 0; i < 4; i++)
        fprintf(F, "factor p3ga %d [%s]\n", i + 1, geoText(m_p3gaFactor[i]).c_str());
    for (i = 0; i < 5; i++)
        fprintf(F, "factor c3ga %d [%s]\n", i + 1, geoText(m_c3gaFactor[i]).c_str());

    // first output every object
    for (oi = g_state->m_objects.begin(); oi != g_state->m_objects.end(); oi++)
    {
        //		printf("Writing %s\n", m_object[i]->m_name);
        oi->second->writeToGeoFile(F);
    }
    // output current global state

    if (gui_state) // (no GUI: e.g. the headless regression harness)
    {
        // without the prefix "GAViewer: " of the window label, which the title entry adds again
        // (mainWindow::setTitle())
        const char *title = gui_state->m_mainWindow->label();
        if (title == NULL)
            title = "";
        if (!strncmp(title, "GAViewer: ", strlen("GAViewer: ")))
            title += strlen("GAViewer: ");
        else if (!strcmp(title, "GAViewer"))
            title = "";
        fprintf(F, "title \"%s\"\n", title);
    }
    printColor(F, "fgcolor", m_fgColor);
    printColor(F, "bgcolor", m_bgColor);
    printColor(F, "olcolor", m_olColor);
    printColor(F, "cvcolor", m_cvColor);

    fprintf(F, "fontsize %f\n", m_fontSize);
    fprintf(F, "clip %f\n", m_clipDistance);

    // float m_poFactor, m_poUnits; // polygon offset todo???

    // todo: size of window / console???

    fprintf(F, "tsmode %s\n", m_tsMode);
    fprintf(F, "tsfont %s\n", m_tsFont);

    // the drawing settings (set by the figures with set_point_size(), set_gl_lines(), ...; not
    // reset when a file is opened, so they are always written)
    if (gui_state)
    {
        fprintf(F, "console set_point_size(%.17g);\n", gui_state->m_pointSize);
        fprintf(F, "console set_line_width(%.17g);\n", gui_state->m_tubeDraw.getLineWidth());
        fprintf(F, "console set_gl_lines(%d);\n", gui_state->m_tubeDraw.getUseOpenGL() ? 1 : 0);
        fprintf(F, "console set_text_scale(%.17g);\n", gui_state->m_textScale);
        fprintf(F, "console set_vector_head_size(%.17g);\n", gui_state->m_vectorHeadSize);
    }
    if (g_pointSphere)
        fprintf(F, "console set_point_sub_div(%d);\n", g_pointSphere->depth);
    if (g_sphereSphere)
        fprintf(F, "console set_sphere_sub_div(%d);\n", g_sphereSphere->depth);

    if (gui_state)
    {
        fprintf(F, "eyepos [%s]\n", geoText(gui_state->m_glWindow->getCamPos()).c_str());
        e3ga::mv B;
        rotor2Bivector(B, gui_state->m_glWindow->getCamOri());
        fprintf(F, "eyeori [%s]\n", geoText(e3ga::mv(-B)).c_str());
    }

    // the default model and the inner product of the console (they affect how the constants,
    // functions and dynamic statements below evaluate)
    const char *defaultModel = modelName(m_globalScope->getDefaultModel());
    fprintf(F, "console default_model(%s);\n", defaultModel ? defaultModel : "");
    if (!m_globalScope->innerProduct().empty())
        fprintf(F, "console inner_product(%s);\n", m_globalScope->innerProduct().c_str());

    // constants (before the functions and dynamic statements that may use them)
    m_globalScope->writeConstantsToGeoFile(F);

    // the user functions the dynamic statements call (otherwise they would be unknown when the
    // file is opened in a new session, or after the 'clear' above)
    std::set<std::string> called;
    for (i = 0; i < (int)m_dynStmt.size(); i++)
        m_dynStmt[i].ds()->calledFunctions(called);
    m_globalScope->writeFunctionsToGeoFile(F, called);

    writeConsoleControlsToGeoFile(F);

    for (i = 0; i < (int)m_dynStmt.size(); i++)
    {
        char *str = m_dynStmt[i].ds()->string(0, 0);
        if (str)
        {
            if (m_dynStmt[i].name().size())
                fprintf(F, "console dynamic{%s : %s}\n", m_dynStmt[i].name().c_str(), str);
            else
                fprintf(F, "console dynamic{%s}\n", str);
            free(str);
        }
    }

    // todo: store animation state??

    fclose(F);

    return 0;
}

int consoleGlobalScope::writeFunctionsToGeoFile(FILE *F, const std::set<std::string> &names)
{
    // the user functions (all overloads) of the names, and of the names they call
    std::set<std::string> todo(names), done;
    std::vector<consoleFuncStmt *> functions; // (in the order of m_functions: sorted)
    while (!todo.empty())
    {
        const std::string name = *todo.begin();
        todo.erase(todo.begin());
        if (!done.insert(name).second)
            continue;
        for (const auto &[key, func] : m_functions)
            if (func->funcStmt && (name == func->name))
            {
                functions.push_back(func->funcStmt);
                std::set<std::string> called;
                func->funcStmt->calledFunctions(called);
                for (const std::string &c : called)
                    if (!done.count(c))
                        todo.insert(c);
            }
    }

    for (consoleFuncStmt *f : functions)
    {
        char *args = f->argSpecList() ? f->argSpecList()->string(0, 0) : strdup("");
        fprintf(F, "console %s %s(%s);\n", f->function() ? "function" : "batch", f->name(), args);
        free(args);
    }
    for (consoleFuncStmt *f : functions)
    {
        char *str = f->string(0, 0);
        fprintf(F, "console %s\n", str);
        free(str);
    }
    return 0;
}

int consoleGlobalScope::writeConstantsToGeoFile(FILE *F)
{
    for (UCI I = m_userConstants.begin(); I != m_userConstants.end(); I++)
    {
        // (the value exactly, see geoText())
        const std::string value =
            I->second.visitMV([](const auto &mv) { return geoText(mv); }, std::string("0"));
        fprintf(F, "console add_const(%s = %s);\n", I->first.c_str(), value.c_str());
    }
    return 0;
}

int consoleControlBool::writeToGeoFile(FILE *F)
{
    // (the name quoted: it may be any text, e.g. "  pan/tilt control ?")
    fprintf(F, "console ctrl_bool(\"%s\" = %d);\n", name(), lookupValueBool());

    return 0;
}

int consoleControlRange::writeToGeoFile(FILE *F)
{
    if (m_valueSlider)
    {
        fprintf(F, "console ctrl_range(\"%s\" = %.17g, %.17g, %.17g, %.17g);\n", name(), lookupValue(), m_valueSlider->minimum(),
                m_valueSlider->maximum(), m_valueSlider->step());
    }

    return 0;
}

int consoleControlSelection::writeToGeoFile(FILE *F)
{
    fprintf(F, "console ctrl_select(%s = %.17g", name(), lookupValue());

    int i = 0;
    while (m_menuItems[i].text)
    {
        fprintf(F, ", %s = %.17g", m_menuItems[i].text, m_menuValues[i]);
        i++;
    }

    fprintf(F, ");\n");

    return 0;
}

int consoleControl::writeToGeoFileMemFun(FILE *F)
{
    return writeToGeoFile(F);
}

int state::writeConsoleControlsToGeoFile(FILE *F)
{
    for (std::vector<consoleControl *>::const_iterator I = m_consoleControls.begin(); I != m_consoleControls.end(); I++)
    {
        (*I)->writeToGeoFileMemFun(F);
        // the control assigns its variable, which shows or hides it depending on the context (e.g.
        // shown if a dynamic statement exists): restore its visibility explicitly
        if (object *o = getObjectPtrByName((*I)->name()))
            fprintf(F, "%s \"%s\"\n", (o->m_drawMode & OD_HIDE) ? "hide" : "show", (*I)->name());
    }

    /*
    // keep? or is this not portable or just MS???? Fuck the STL!

    std::for_each(m_consoleControls.begin(), m_consoleControls.end(),
        std::bind2nd(std::mem_fun1(&consoleControl::writeToGeoFileMemFun), F) );
*/
    return 0;
}

int object::writeToGeoFile(FILE *F)
{

    printColor(F, "fgcolor", m_fgColor);
    printColor(F, "bgcolor", m_bgColor);
    printColor(F, "olcolor", m_olColor);

    fprintf(F, "fontsize %f\n", m_fontSize);

    return 0;
}

int object::writeToGeoFileProperties(FILE *F)
{
    // m_dmMenuIdx
    // m_drawMode

    if (m_drawMode & OD_HIDE)
        fprintf(F, "hide ");
    if (m_drawMode & OD_STIPPLE)
        fprintf(F, "stipple ");
    if (m_drawMode & OD_ORI)
        fprintf(F, "orientation ");
    if (m_drawMode & OD_WIREFRAME)
        fprintf(F, "wireframe ");
    if (m_drawMode & OD_MAGNITUDE)
        fprintf(F, "magnitude ");
    if (m_drawMode & OD_SHADE)
        fprintf(F, "shade ");
    if (m_drawMode & OD_OUTLINE)
        fprintf(F, "outline ");
    if (m_drawMode & OD_HIDE_ALWAYS)
        fprintf(F, "hide_always ");

    if (m_dmMenuIdx >= 0)
        fprintf(F, "dm%d", m_dmMenuIdx + 1);

    /*
    else if (!strcmpnc(str[i], "versor")) creationFlags |= OC_VERSOR;
    else if (!strcmpnc(str[i], "blade")) creationFlags |= OC_BLADE;
    */

    return 0;
}

int e3gaObject::writeToGeoFile(FILE *F)
{
    object::writeToGeoFile(F);

    fprintf(F, "e3ga \"%s\" [%s] ", m_name.c_str(), geoText(m_mv).c_str());
    writeToGeoFileProperties(F);
    fprintf(F, "\n");
    return 0;
}

int p3gaObject::writeToGeoFile(FILE *F)
{
    object::writeToGeoFile(F);

    fprintf(F, "p3ga \"%s\" [%s] ", m_name.c_str(), geoText(m_mv).c_str());
    writeToGeoFileProperties(F);
    fprintf(F, "\n");
    return 0;
}

int c3gaObject::writeToGeoFile(FILE *F)
{
    object::writeToGeoFile(F);

    fprintf(F, "c3ga \"%s\" [%s] ", m_name.c_str(), geoText(m_mv).c_str());
    writeToGeoFileProperties(F);
    fprintf(F, "\n");
    return 0;
}

int c5gaObject::writeToGeoFile(FILE *F)
{
    object::writeToGeoFile(F);

    fprintf(F, "c5ga \"%s\" [%s] ", m_name.c_str(), geoText(m_mv).c_str());
    writeToGeoFileProperties(F);
    fprintf(F, "\n");
    return 0;
}

int i2gaObject::writeToGeoFile(FILE *F)
{
    object::writeToGeoFile(F);

    fprintf(F, "i2ga \"%s\" [%s] ", m_name.c_str(), geoText(m_mv).c_str());
    writeToGeoFileProperties(F);
    fprintf(F, "\n");
    return 0;
}

int textObject::writeToGeoFile(FILE *F)
{
    object::writeToGeoFile(F);
    char labelPosName[1024];

    // fix saving for labels.. remember image possibilyt

    // print out typeset mode & font
    fprintf(F, "tsmode %s\n", m_tsMode);
    fprintf(F, "tsfont %s\n", m_tsFont);

    // get label position
    if (!(m_flags & LABEL_DYNAMIC))
        sprintf(labelPosName, "[%s]", geoText(m_pos).c_str());
    else
        sprintf(labelPosName, "\"%s\"", m_pointName.c_str());

    // KNOWN LIMITATION: a '"' in the text is written as it is, and GAfile::readString() ends the
    // text there (e.g. 'say "hi"' is read back as 'say '); see GAfile::readString()
    fprintf(F, "label \"%s\" %s \"%s\" ", m_name.c_str(), labelPosName, m_text);

    if (m_flags & LABEL_2D)
        fprintf(F, "flag_2d ");
    if (m_flags & LABEL_3D)
        fprintf(F, "flag_3d ");
    if (m_flags & LABEL_CX)
        fprintf(F, "flag_cx ");
    if (m_flags & LABEL_CY)
        fprintf(F, "flag_cy ");
    if (m_flags & LABEL_PX)
        fprintf(F, "flag_px ");
    if (m_flags & LABEL_PY)
        fprintf(F, "flag_py ");
    if (m_flags & LABEL_NX)
        fprintf(F, "flag_nx ");
    if (m_flags & LABEL_NY)
        fprintf(F, "flag_ny ");
    if (m_flags & LABEL_ACX)
        fprintf(F, "flag_acx ");
    if (m_flags & LABEL_DYNAMIC)
        fprintf(F, "flag_dynamic ");
    if (m_flags & LABEL_IMAGE)
        fprintf(F, "flag_image ");
    if (m_flags & LABEL_FULLSCREEN)
        fprintf(F, "flag_fullscreen ");

    writeToGeoFileProperties(F);
    fprintf(F, "\n");
    return 0;
}

int polygonObject::writeToGeoFile(FILE *F)
{
    object::writeToGeoFile(F);
    int i;

    // todo: maybe make temp vertices indices a global var? increment on each use, reset on new save?

    // write vertices
    for (i = 0; i < m_nbVertices; i++)
    {
        fprintf(F, "e3ga \"%s vertex %d\" [%s] hide \n", m_name.c_str(), i,
                geoText(e3ga::vector(m_vertex + i * 3)).c_str());
    }

    fprintf(F, "%s \"%s\" %d ", (m_simplex) ? "simplex" : "polygon", m_name.c_str(), m_nbVertices);

    for (i = 0; i < m_nbVertices; i++)
    {
        if (m_mode & OC_DYNAMIC)
        {
            fprintf(F, "\"%s\" ", m_vertexName[i]);
        }
        else
        {
            fprintf(F, "\"%s vertex %d\" ", m_name.c_str(), i);
        }
    }

    if (m_mode & OC_DYNAMIC)
        fprintf(F, "dynamic ");

    writeToGeoFileProperties(F);
    fprintf(F, "\n");
    return 0;
}

int meshObject::writeToGeoFile(FILE *F)
{
    int i, j;
    object::writeToGeoFile(F);

    fprintf(F, "mesh \"%s\" ", m_name.c_str());

    if (m_mode & OC_DYNAMIC)
        fprintf(F, "dynamic ");

    if (m_mode & OC_COMPUTE_NORMALS_FLAT)
        fprintf(F, "compute_normals_flat ");
    else if (m_mode & OC_COMPUTE_NORMALS_GOURAUD)
        fprintf(F, "compute_normals_gouraud ");
    else if (m_mode & OC_SPECIFY_NORMALS)
        fprintf(F, "specify_normals ");

    writeToGeoFileProperties(F);
    fprintf(F, "\n");

    for (i = 0; i < m_nbVertices; i++)
    {
        if (m_vertex[i].posName)
            fprintf(F, "meshvertex \"%s\" %d \"%s\"\n", m_name.c_str(), i, m_vertex[i].posName);
        else
            fprintf(F, "meshvertex \"%s\" %d [%s]\n", m_name.c_str(), i, geoText(e3ga::vector(m_vertex[i].pos)).c_str());

        if (m_vertex[i].nlName)
            fprintf(F, "meshnormal \"%s\" %d \"%s\"\n", m_name.c_str(), i, m_vertex[i].nlName);
        else
            fprintf(F, "meshnormal \"%s\" %d [%s]\n", m_name.c_str(), i, geoText(e3ga::vector(m_vertex[i].nl)).c_str());
    }

    for (i = 0; i < m_nbFaces; i++)
    {
        fprintf(F, "meshface \"%s\" ", m_name.c_str());

        for (j = 0; j < m_face[i].nbVtx; j++)
            fprintf(F, "%d ", m_face[i].vtxIdx[j]);

        fprintf(F, "\n");
    }

    return 0;
}
