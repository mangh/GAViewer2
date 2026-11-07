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

/*
The general built-in console functions (points, transformations, draw
properties, bit-wise operations, system functions and help), and the table of
all built-in functions: these general functions followed by the functions of
every algebra (see consolealgebrafunctions.cpp).

(Up to 2026 this was the 'generated' file consolegeneratedfunctions.cpp,
produced by the 'codegen' tool from consolecodetemplates.txt.)
*/

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <array>
#include <string>
#include <utility>
#include <vector>

#include "../gaincludes.h"
#include "../geosphere.h"
#include "../glwindow.h"
#include "../mainwindow.h"
#include "../mt19937-2.h"
#include "../mvint.h"
#include "../object.h"
#include "../osdep.h"
#include "../state.h"
#include "../uistate.h"
#include "../util.h"
#include "console.h"
#include "consolefunction.h"
#include "consolegafile.h"
#include "consolescope.h"
#include "consolevariable.h"

consoleVariable *cf_p3ga_point3(consoleScope *s, consoleVariable *arg[])
{
    p3ga::mv pt(ga::scalar(arg[0]->e()) * p3ga::e1<double>() + ga::scalar(arg[1]->e()) * p3ga::e2<double>() + ga::scalar(arg[2]->e()) * p3ga::e3<double>() +
            p3ga::e0<double>());
    return new consoleVariable("", pt);
}

consoleVariable *cf_p3ga_point1(consoleScope *s, consoleVariable *arg[])
{
    const std::array<double, 3> c = e3ga::xyz(arg[0]->e());
    p3ga::mv pt(c[0] * p3ga::e1<double>() + c[1] * p3ga::e2<double>() + c[2] * p3ga::e3<double>() + p3ga::e0<double>());
    return new consoleVariable("", pt);
}

consoleVariable *cf_c3ga_point3(consoleScope *s, consoleVariable *arg[])
{
    double e1c = ga::scalar(arg[0]->e()), e2c = ga::scalar(arg[1]->e()), e3c = ga::scalar(arg[2]->e());
    c3ga::mv pt = c3ga::vector(e1c, e2c, e3c, 1.0, 0.5 * (e1c * e1c + e2c * e2c + e3c * e3c));
    return new consoleVariable("", pt);
}

consoleVariable *cf_c3ga_point1(consoleScope *s, consoleVariable *arg[])
{
    const std::array<double, 3> c = e3ga::xyz(arg[0]->e());
    double e1c = c[0], e2c = c[1], e3c = c[2];
    c3ga::mv pt = c3ga::vector(e1c, e2c, e3c, 1.0, 0.5 * (e1c * e1c + e2c * e2c + e3c * e3c));
    return new consoleVariable("", pt);
}

consoleVariable *cf_c3ga_pointP1(consoleScope *s, consoleVariable *arg[])
{
    const std::array<double, 4> c = p3ga::xyzw(arg[0]->p());
    double e0c = c[3];
    double e1c, e2c, e3c;
    if (e0c != 0.0)
    {
        e1c = c[0] / e0c;
        e2c = c[1] / e0c;
        e3c = c[2] / e0c;
    }
    else
    {
        e1c = c[0];
        e2c = c[1];
        e3c = c[2];
    }

    c3ga::mv pt = c3ga::vector(e1c, e2c, e3c, 1.0, 0.5 * (e1c * e1c + e2c * e2c + e3c * e3c));
    return new consoleVariable("", pt);
}

consoleVariable *cf_translation_versor3(consoleScope *s, consoleVariable *arg[])
{
    double e1c = ga::scalar(arg[0]->e()), e2c = ga::scalar(arg[1]->e()), e3c = ga::scalar(arg[2]->e());
    c3ga::mv t = c3ga::vector(e1c, e2c, e3c, 0.0, 0.0);
    c3ga::mv tv = 1 - 0.5 * (t ^ c3ga::ei<double>());
    return new consoleVariable("", tv);
}

consoleVariable *cf_translation_versor1(consoleScope *s, consoleVariable *arg[])
{
    const std::array<double, 3> c = e3ga::xyz(arg[0]->e());
    double e1c = c[0], e2c = c[1], e3c = c[2];
    c3ga::mv t = c3ga::vector(e1c, e2c, e3c, 0.0, 0.0);
    c3ga::mv tv = 1 - 0.5 * (t ^ c3ga::ei<double>());
    return new consoleVariable("", tv);
}

// ********************************* CLF *********************************
consoleVariable *cf_clf(consoleScope *s, consoleVariable *arg[])
{
    g_state->removeAllObjects();
    g_state->setDefaultGlobalScopeVariables();

    if (gui_state)
        gui_state->setStatus("");
    return new consoleVariable("", e3ga::mv(0.0));
}

// ********************************* CLC *********************************
consoleVariable *cf_clc(consoleScope *s, consoleVariable *arg[])
{
    if (gui_state && (gui_state->m_console))
        gui_state->m_console->clear();
    if (g_state)
        g_state->removeAllConsoleControls();

    return new consoleVariable("", e3ga::mv(0.0));
}

// ********************************* RED .. GRAY *********************************

consoleVariable *cf_set_object_X_color4(const std::string &objectName, int X, float R, float G, float B, float A)
{
    e3ga::mv tmp(0.0);
    object *O = g_state->getObjectPtrByName(objectName);
    if (O == NULL)
    {
        return new consoleVariable("", tmp);
    }
    else
    {
        float C[4] = {R, G, B, A};
        if (X == 0)
            O->setFgColor(C);
        else if (X == 1)
            O->setBgColor(C);
        else if (X == 2)
            O->setOlColor(C);
    }

    return new consoleVariable("", tmp);
}

consoleVariable *cf_set_object_X_color3(const std::string &objectName, int X, float R, float G, float B)
{
    e3ga::mv tmp(0.0);
    object *O = g_state->getObjectPtrByName(objectName);
    if (O == NULL)
    {
        return new consoleVariable("", tmp);
    }
    else
    {
        float A = 1.0;
        if (X == 0)
            A = O->fgColor(3);
        else if (X == 1)
            A = O->bgColor(3);
        else if (X == 2)
            A = O->olColor(3);
        return cf_set_object_X_color4(objectName, X, R, G, B, A);
    }
}

consoleVariable *cf_set_object_fg_color_vector(consoleScope *s, consoleVariable *arg[])
{
    const std::string &objectName = arg[0]->name();
    return cf_set_object_X_color3(objectName, 0, (float)arg[1]->e()[e3ga::E1], (float)arg[1]->e()[e3ga::E2],
                                  (float)arg[1]->e()[e3ga::E3]);
}

consoleVariable *cf_set_object_fg_color_RGB(consoleScope *s, consoleVariable *arg[])
{
    const std::string &objectName = arg[0]->name();
    return cf_set_object_X_color3(objectName, 0, (float)ga::scalar(arg[1]->e()), (float)ga::scalar(arg[2]->e()),
                                  (float)ga::scalar(arg[3]->e()));
}
consoleVariable *cf_set_object_fg_color_RGBA(consoleScope *s, consoleVariable *arg[])
{
    const std::string &objectName = arg[0]->name();
    return cf_set_object_X_color4(objectName, 0, (float)ga::scalar(arg[1]->e()), (float)ga::scalar(arg[2]->e()),
                                  (float)ga::scalar(arg[3]->e()), (float)ga::scalar(arg[4]->e()));
}

static consoleVariable *cf_set_color3_internal(consoleScope *s, consoleVariable *arg[], float c[3])
{
    arg[0]->color3(c);
    arg[0]->modifiedColor();
    return arg[0];
}

consoleVariable *cf_red(consoleScope *s, consoleVariable *arg[])
{
    float c[4];
    ::getColor("red", c);
    return cf_set_color3_internal(s, arg, c);
}

consoleVariable *cf_green(consoleScope *s, consoleVariable *arg[])
{
    float c[4];
    ::getColor("green", c);
    return cf_set_color3_internal(s, arg, c);
}

consoleVariable *cf_blue(consoleScope *s, consoleVariable *arg[])
{
    float c[4];
    ::getColor("blue", c);
    return cf_set_color3_internal(s, arg, c);
}

consoleVariable *cf_white(consoleScope *s, consoleVariable *arg[])
{
    float c[4];
    ::getColor("white", c);
    return cf_set_color3_internal(s, arg, c);
}

consoleVariable *cf_magenta(consoleScope *s, consoleVariable *arg[])
{
    float c[4];
    ::getColor("magenta", c);
    return cf_set_color3_internal(s, arg, c);
}

consoleVariable *cf_yellow(consoleScope *s, consoleVariable *arg[])
{
    float c[4];
    ::getColor("yellow", c);
    return cf_set_color3_internal(s, arg, c);
}

consoleVariable *cf_cyan(consoleScope *s, consoleVariable *arg[])
{
    float c[4];
    ::getColor("cyan", c);
    return cf_set_color3_internal(s, arg, c);
}

consoleVariable *cf_black(consoleScope *s, consoleVariable *arg[])
{
    float c[4];
    ::getColor("black", c);
    return cf_set_color3_internal(s, arg, c);
}

consoleVariable *cf_grey(consoleScope *s, consoleVariable *arg[])
{
    float c[4];
    ::getColor("grey", c);
    return cf_set_color3_internal(s, arg, c);
}

consoleVariable *cf_color1(consoleScope *s, consoleVariable *arg[])
{
    const std::array<double, 3> c = e3ga::xyz(arg[1]->e());
    arg[0]->color3((float)c[0], (float)c[1], (float)c[2]);
    arg[0]->modifiedColor();
    return arg[0];
}

consoleVariable *cf_color3(consoleScope *s, consoleVariable *arg[])
{
    arg[0]->color3((float)ga::scalar(arg[1]->e()), (float)ga::scalar(arg[2]->e()), (float)ga::scalar(arg[3]->e()));
    arg[0]->modifiedColor();
    return arg[0];
}

consoleVariable *cf_color4(consoleScope *s, consoleVariable *arg[])
{
    arg[0]->color4((float)ga::scalar(arg[1]->e()), (float)ga::scalar(arg[2]->e()), (float)ga::scalar(arg[3]->e()),
                   (float)ga::scalar(arg[4]->e()));
    arg[0]->modifiedColor();
    arg[0]->modifiedAlpha();
    return arg[0];
}

static consoleVariable *cf_alpha_internal(consoleVariable *arg0, float arg1)
{
    arg0->alpha(arg1);
    arg0->modifiedAlpha();
    return arg0;
}

consoleVariable *cf_alpha(consoleScope *s, consoleVariable *arg[])
{
    return cf_alpha_internal(arg[0], (float)ga::scalar(arg[1]->e()));
}

consoleVariable *cf_versor(consoleScope *s, consoleVariable *arg[])
{
    arg[0]->creationFlags(arg[0]->creationFlags() | OC_VERSOR);
    return arg[0];
}

consoleVariable *cf_blade(consoleScope *s, consoleVariable *arg[])
{
    arg[0]->creationFlags(arg[0]->creationFlags() | OC_BLADE);
    return arg[0];
}

consoleVariable *cf_dm1(consoleScope *s, consoleVariable *arg[])
{
    arg[0]->creationFlags((arg[0]->creationFlags() ^ (arg[0]->creationFlags() & OC_DM7)) | OC_DM1);
    arg[0]->modifiedDrawMethod();
    return arg[0];
}
consoleVariable *cf_dm2(consoleScope *s, consoleVariable *arg[])
{
    arg[0]->creationFlags((arg[0]->creationFlags() ^ (arg[0]->creationFlags() & OC_DM7)) | OC_DM2);
    arg[0]->modifiedDrawMethod();
    return arg[0];
}
consoleVariable *cf_dm3(consoleScope *s, consoleVariable *arg[])
{
    arg[0]->creationFlags((arg[0]->creationFlags() ^ (arg[0]->creationFlags() & OC_DM7)) | OC_DM3);
    arg[0]->modifiedDrawMethod();
    return arg[0];
}
consoleVariable *cf_dm4(consoleScope *s, consoleVariable *arg[])
{
    arg[0]->creationFlags((arg[0]->creationFlags() ^ (arg[0]->creationFlags() & OC_DM7)) | OC_DM4);
    arg[0]->modifiedDrawMethod();
    return arg[0];
}
consoleVariable *cf_dm5(consoleScope *s, consoleVariable *arg[])
{
    arg[0]->creationFlags((arg[0]->creationFlags() ^ (arg[0]->creationFlags() & OC_DM7)) | OC_DM5);
    arg[0]->modifiedDrawMethod();
    return arg[0];
}
consoleVariable *cf_dm6(consoleScope *s, consoleVariable *arg[])
{
    arg[0]->creationFlags((arg[0]->creationFlags() ^ (arg[0]->creationFlags() & OC_DM7)) | OC_DM6);
    arg[0]->modifiedDrawMethod();
    return arg[0];
}
consoleVariable *cf_dm7(consoleScope *s, consoleVariable *arg[])
{
    arg[0]->creationFlags((arg[0]->creationFlags() ^ (arg[0]->creationFlags() & OC_DM7)) | OC_DM7);
    arg[0]->modifiedDrawMethod();
    return arg[0];
}

consoleVariable *cf_stipple(consoleScope *s, consoleVariable *arg[])
{
    arg[0]->drawMode(arg[0]->drawMode() | OD_STIPPLE);
    arg[0]->modifiedStipple();
    return arg[0];
}

consoleVariable *cf_no_stipple(consoleScope *s, consoleVariable *arg[])
{
    arg[0]->drawMode(arg[0]->drawMode() ^ (arg[0]->drawMode() & OD_STIPPLE));
    arg[0]->modifiedStipple();
    return arg[0];
}

consoleVariable *cf_ori(consoleScope *s, consoleVariable *arg[])
{
    arg[0]->drawMode(arg[0]->drawMode() | OD_ORI);
    arg[0]->modifiedOri();
    return arg[0];
}

consoleVariable *cf_no_ori(consoleScope *s, consoleVariable *arg[])
{
    arg[0]->drawMode(arg[0]->drawMode() ^ (arg[0]->drawMode() & OD_ORI));
    arg[0]->modifiedOri();
    return arg[0];
}

consoleVariable *cf_mute(consoleScope *s, consoleVariable *arg[])
{
    arg[0]->drawMode(arg[0]->drawMode() | OD_MUTE);
    arg[0]->modifiedMute();
    return arg[0];
}

consoleVariable *cf_unmute(consoleScope *s, consoleVariable *arg[])
{
    arg[0]->drawMode(arg[0]->drawMode() ^ (arg[0]->drawMode() & OD_MUTE));
    arg[0]->modifiedMute();
    return arg[0];
}

consoleVariable *cf_hide(consoleScope *s, consoleVariable *arg[])
{
    arg[0]->drawMode(arg[0]->drawMode() | OD_HIDE);
    arg[0]->modifiedHide();
    return arg[0];
}

consoleVariable *cf_show(consoleScope *s, consoleVariable *arg[])
{
    arg[0]->drawMode(arg[0]->drawMode() ^ (arg[0]->drawMode() & OD_HIDE));
    arg[0]->modifiedHide();
    return arg[0];
}

consoleVariable *cf_wireframe(consoleScope *s, consoleVariable *arg[])
{
    arg[0]->drawMode(arg[0]->drawMode() | OD_WIREFRAME);
    arg[0]->modifiedWireframe();
    return arg[0];
}

consoleVariable *cf_no_wireframe(consoleScope *s, consoleVariable *arg[])
{
    arg[0]->drawMode(arg[0]->drawMode() ^ (arg[0]->drawMode() & OD_WIREFRAME));
    arg[0]->modifiedWireframe();
    return arg[0];
}

consoleVariable *cf_shade(consoleScope *s, consoleVariable *arg[])
{
    arg[0]->drawMode(arg[0]->drawMode() | OD_SHADE);
    arg[0]->modifiedShade();
    return arg[0];
}

consoleVariable *cf_no_shade(consoleScope *s, consoleVariable *arg[])
{
    arg[0]->drawMode(arg[0]->drawMode() ^ (arg[0]->drawMode() & OD_SHADE));
    arg[0]->modifiedShade();
    return arg[0];
}

consoleVariable *cf_outline(consoleScope *s, consoleVariable *arg[])
{
    arg[0]->drawMode(arg[0]->drawMode() | OD_OUTLINE);
    arg[0]->modifiedOutline();
    return arg[0];
}

consoleVariable *cf_no_outline(consoleScope *s, consoleVariable *arg[])
{
    arg[0]->drawMode(arg[0]->drawMode() ^ (arg[0]->drawMode() & OD_OUTLINE));
    arg[0]->modifiedOutline();
    return arg[0];
}

consoleVariable *cf_no_weight(consoleScope *s, consoleVariable *arg[])
{
    arg[0]->drawMode(arg[0]->drawMode() ^ (arg[0]->drawMode() & OD_MAGNITUDE));
    arg[0]->modifiedMagnitude();
    return arg[0];
}

consoleVariable *cf_weight(consoleScope *s, consoleVariable *arg[])
{
    arg[0]->drawMode(arg[0]->drawMode() | OD_MAGNITUDE);
    arg[0]->modifiedMagnitude();
    return arg[0];
}

consoleVariable *cf_get_color(consoleScope *s, consoleVariable *arg[])
{
    const float *c = arg[0]->color();
    return new consoleVariable("", e3ga::vector(c[0], c[1], c[2]));
}

consoleVariable *cf_get_alpha(consoleScope *s, consoleVariable *arg[])
{
    return new consoleVariable("", e3ga::mv((double)arg[0]->alpha()));
}

consoleVariable *cf_get_stipple(consoleScope *s, consoleVariable *arg[])
{
    return new consoleVariable("", e3ga::mv((double)((arg[0]->drawMode() & OD_STIPPLE) != 0)));
}
consoleVariable *cf_get_wireframe(consoleScope *s, consoleVariable *arg[])
{
    return new consoleVariable("", e3ga::mv((double)((arg[0]->drawMode() & OD_WIREFRAME) != 0)));
}
consoleVariable *cf_get_outline(consoleScope *s, consoleVariable *arg[])
{
    return new consoleVariable("", e3ga::mv((double)((arg[0]->drawMode() & OD_OUTLINE) != 0)));
}
consoleVariable *cf_get_weight(consoleScope *s, consoleVariable *arg[])
{
    return new consoleVariable("", e3ga::mv((double)((arg[0]->drawMode() & OD_MAGNITUDE) != 0)));
}
consoleVariable *cf_get_ori(consoleScope *s, consoleVariable *arg[])
{
    return new consoleVariable("", e3ga::mv((double)((arg[0]->drawMode() & OD_ORI) != 0)));
}
consoleVariable *cf_get_hide(consoleScope *s, consoleVariable *arg[])
{
    return new consoleVariable("", e3ga::mv((double)((arg[0]->drawMode() & OD_HIDE) != 0)));
}

consoleVariable *cf_bit_not(consoleScope *s, consoleVariable *arg[])
{
    double s1 = arg[0]->scalar();
    s1 += (s1 >= 0.0) ? 0.5 : -0.5;
    unsigned int i1 = (unsigned int)(s1 + ((s1 >= 0.0) ? 0.5 : -0.5));
    i1 = (unsigned int)0xFFFFFFFF ^ i1;
    return new consoleVariable("", e3ga::mv((double)i1));
}

consoleVariable *cf_bit_and(consoleScope *s, consoleVariable *arg[])
{
    double s1 = arg[0]->scalar();
    unsigned int i1 = (unsigned int)(s1 + ((s1 >= 0.0) ? 0.5 : -0.5));

    double s2 = arg[1]->scalar();
    unsigned int i2 = (unsigned int)(s2 + ((s2 >= 0.0) ? 0.5 : -0.5));

    return new consoleVariable("", e3ga::mv((double)(i1 & i2)));
}

consoleVariable *cf_bit_or(consoleScope *s, consoleVariable *arg[])
{
    double s1 = arg[0]->scalar();
    unsigned int i1 = (unsigned int)(s1 + ((s1 >= 0.0) ? 0.5 : -0.5));

    double s2 = arg[1]->scalar();
    unsigned int i2 = (unsigned int)(s2 + ((s2 >= 0.0) ? 0.5 : -0.5));

    return new consoleVariable("", e3ga::mv((double)(i1 | i2)));
}

consoleVariable *cf_bit_xor(consoleScope *s, consoleVariable *arg[])
{
    double s1 = arg[0]->scalar();
    unsigned int i1 = (unsigned int)(s1 + ((s1 >= 0.0) ? 0.5 : -0.5));

    double s2 = arg[1]->scalar();
    unsigned int i2 = (unsigned int)(s2 + ((s2 >= 0.0) ? 0.5 : -0.5));

    return new consoleVariable("", e3ga::mv((double)(i1 ^ i2)));
}

consoleVariable *cf_bit_shift(consoleScope *s, consoleVariable *arg[])
{
    double s1 = arg[0]->scalar();
    unsigned int i1 = (unsigned int)(s1 + ((s1 >= 0.0) ? 0.5 : -0.5));

    double s2 = arg[1]->scalar();
    int i2 = (int)(s2 + ((s2 >= 0.0) ? 0.5 : -0.5));

    if (i2 < 0)
        return new consoleVariable("", e3ga::mv((double)(i1 >> (-i2))));
    else
        return new consoleVariable("", e3ga::mv((double)(i1 << (i2))));
}

// ********************************* CREATE POLYGON  *********************************

consoleVariable *cf_create_polygon(consoleScope *s, consoleVariable *arg[], int N)
{
    const std::string &polyName = arg[0]->name();
    const std::string &fillMode = arg[1]->name();

    //	printf("Fillmode = '%s'\n", fillMode.c_str());
    int creationFlags = OC_DM1;
    if (fillMode == "strip")
        creationFlags = OC_DM2;
    else if (fillMode == "loop")
        creationFlags = OC_DM3;
    int drawMode = 0;

    polygonObject *PO = new polygonObject(polyName, drawMode, creationFlags);

    for (int i = 0; i < N; i++)
    {
        PO->addVertex(arg[2 + i]->name().c_str());
    }

    g_state->addObject(PO);

    e3ga::mv tmp(0.0);
    return new consoleVariable("", tmp);
}

consoleVariable *cf_create_polygon_3(consoleScope *s, consoleVariable *arg[])
{
    return cf_create_polygon(s, arg, 3);
}
consoleVariable *cf_create_polygon_4(consoleScope *s, consoleVariable *arg[])
{
    return cf_create_polygon(s, arg, 4);
}
consoleVariable *cf_create_polygon_5(consoleScope *s, consoleVariable *arg[])
{
    return cf_create_polygon(s, arg, 5);
}
consoleVariable *cf_create_polygon_6(consoleScope *s, consoleVariable *arg[])
{
    return cf_create_polygon(s, arg, 6);
}

consoleVariable *cf_create_arc(consoleScope *s, consoleVariable *arg[])
{
    // arg 0: name
    // arg 1: versor
    // arg 2: point
    // arg 3: nb of segments
    const std::string &arcName = arg[0]->name();
    c3ga::mv V = arg[1]->c();
    c3ga::mv p = arg[2]->c();
    int nbSegments = arg[3]->scalar();

    int polyDrawMode = 0;
    polygonObject *PO = new polygonObject(arcName, polyDrawMode, OC_DM2); // OC_DM2 = strip

    c3ga::mv Vi = ga::inverse(V);
    for (int i = 0; i <= nbSegments; i++)
    {
        char pointName[1024];
        sprintf(pointName, "%s_point_%d", arcName.c_str(), i);
        g_state->addC3gaObject(p, pointName);
        PO->addVertex(pointName);
        g_state->removeObject(pointName);
        p = V * p * Vi;
    }

    g_state->addObject(PO);

    e3ga::mv tmp(0.0);
    return new consoleVariable("", tmp);
}

// ********************************* SCREENSHOT  *********************************
consoleVariable *cf_screenshot(consoleScope *s, consoleVariable *arg[])
{
    int width = (int)(arg[1]->scalar() + 0.5);
    int height = (int)(arg[2]->scalar() + 0.5);
    bool wol = arg[3]->scalar() != 0.0;

    if (gui_state)
        gui_state->setupScreenshot(arg[0]->name(), width, height, wol);

    return new consoleVariable("", e3ga::mv(0.0));
}

// ********************************* SET WINDOW TITLE  *********************************
consoleVariable *cf_set_window_title(consoleScope *s, consoleVariable *arg[])
{
    static char title[1024];
    strcpy(title, (std::string("GAViewer: ") + arg[0]->name()).c_str());
    //	strcpy(title, arg[0]->name().c_str());
    if (gui_state && gui_state->m_mainWindow)
        gui_state->m_mainWindow->label(title);

    return new consoleVariable("", e3ga::mv(0.0));
}

// ********************************* ADD NET PORT  *********************************
consoleVariable *cf_add_net_port(consoleScope *s, consoleVariable *arg[])
{
    char buf[16];
    sprintf(buf, "%d", (int)arg[0]->scalar());
    g_state->openNetServer(buf);

    e3ga::mv tmp(0.0);
    return new consoleVariable("", tmp);
}

// ********************************* REMOVE NET PORT  *********************************
consoleVariable *cf_remove_net_port(consoleScope *s, consoleVariable *arg[])
{
    char buf[16];
    sprintf(buf, "%d", (int)arg[0]->scalar());
    g_state->closeNetServer(buf);

    e3ga::mv tmp(0.0);
    return new consoleVariable("", tmp);
}

// ********************************* DISPLAY NET STATUS *********************************
consoleVariable *cf_net_status(consoleScope *s, consoleVariable *arg[])
{
    g_state->displayNetStatus();

    e3ga::mv tmp(0.0);
    return new consoleVariable("", tmp);
}

// ********************************* CLOSES ALL NETWORK STUFF *********************************
consoleVariable *cf_net_close(consoleScope *s, consoleVariable *arg[])
{
    g_state->closeNetServers();

    e3ga::mv tmp(0.0);
    return new consoleVariable("", tmp);
}

// ********************************* MOUSE INTERACTION *********************************
consoleVariable *cf_disable_mouse_interaction(consoleScope *s, consoleVariable *arg[])
{
    if (gui_state)
        gui_state->setInteractiveMouse(false);
    e3ga::mv tmp(0.0);
    return new consoleVariable("", tmp);
}

consoleVariable *cf_enable_mouse_interaction(consoleScope *s, consoleVariable *arg[])
{
    if (gui_state)
        gui_state->setInteractiveMouse(true);
    e3ga::mv tmp(0.0);
    return new consoleVariable("", tmp);
}

// ********************************* SET POINT SIZE  *********************************
consoleVariable *cf_set_point_size(consoleScope *s, consoleVariable *arg[])
{
    if (gui_state)
        gui_state->setPointSize(arg[0]->scalar());
    e3ga::mv tmp(0.0);
    return new consoleVariable("", tmp);
}

// ********************************* SET NEAR FAR CLIP PLANES*********************************
consoleVariable *cf_set_near_far_clip_planes(consoleScope *s, consoleVariable *arg[])
{
    if (gui_state)
        gui_state->m_glWindow->setNearFarClipPlanes(arg[0]->scalar(), arg[1]->scalar());
    e3ga::mv tmp(0.0);
    return new consoleVariable("", tmp);
}

// ********************************* SET TEXT SCALE *********************************
consoleVariable *cf_set_text_scale(consoleScope *s, consoleVariable *arg[])
{
    if (gui_state)
        gui_state->setTextScale(fabs(arg[0]->scalar()));
    e3ga::mv tmp(0.0);
    return new consoleVariable("", tmp);
}

// ********************************* SET LINE WIDTH *********************************
consoleVariable *cf_set_line_width(consoleScope *s, consoleVariable *arg[])
{
    if (gui_state)
        gui_state->setLineWidth(fabs(arg[0]->scalar()));
    e3ga::mv tmp(0.0);
    return new consoleVariable("", tmp);
}

// ********************************* SET GL LINES *********************************
consoleVariable *cf_set_gl_lines(consoleScope *s, consoleVariable *arg[])
{
    // first set to opposite of requested:
    if (gui_state)
    {
        gui_state->m_tubeDraw.setUseOpenGL(arg[0]->scalar() == 0.0);
        // then call menu callback (which toggles the value):
        ui_menuCallback(NULL, (void *)MI_GLLINES);
    }

    e3ga::mv tmp(0.0);
    return new consoleVariable("", tmp);
}

// ********************************* SET VECTOR HEAD SIZE *********************************
consoleVariable *cf_set_vector_head_size(consoleScope *s, consoleVariable *arg[])
{
    if (gui_state)
        gui_state->setVectorHeadSize(arg[0]->scalar());
    e3ga::mv tmp(0.0);
    return new consoleVariable("", tmp);
}

// ********************************* SET POINT SUB DIV *********************************
consoleVariable *cf_set_point_sub_div(consoleScope *s, consoleVariable *arg[])
{
    int subDiv = (int)(arg[0]->scalar() + 0.5);
    if (subDiv < 1)
        subDiv = 1;
    if (subDiv > 5)
        subDiv = 5;

    gsFree(&g_pointSphere);
    gsCompute(&g_pointSphere, subDiv);

    e3ga::mv tmp(0.0);
    return new consoleVariable("", tmp);
}

// ********************************* SET SPHERE SUB DIV *********************************
consoleVariable *cf_set_sphere_sub_div(consoleScope *s, consoleVariable *arg[])
{
    int subDiv = (int)(arg[0]->scalar() + 0.5);
    if (subDiv < 1)
        subDiv = 1;
    if (subDiv > 5)
        subDiv = 5;

    gsFree(&g_sphereSphere);
    gsCompute(&g_sphereSphere, subDiv);

    e3ga::mv tmp(0.0);
    return new consoleVariable("", tmp);
}

consoleVariable *cf_cmd(consoleScope *s, consoleVariable *arg[])
{
    e3ga::mv tmp(0.0);

    // cprintf("cmd: '%s'\n", arg[0]->name().c_str());
    g_state->m_consoleGAfile->read(g_state, arg[0]->name().c_str());

    return new consoleVariable("", tmp);
}

// ********************************* RANDOM NUMBER *********************************
consoleVariable *cf_rand(consoleScope *s, consoleVariable *arg[])
{
    return new consoleVariable("", e3ga::mv(genrand()));
    //	return new consoleVariable("", i2ga(genrand()));
}

// *********************************  TIME *********************************
consoleVariable *cf_time(consoleScope *s, consoleVariable *arg[])
{
    return new consoleVariable("", e3ga::mv(u_timeGet()));
    //	return new consoleVariable("", i2ga(u_timeGet()));
}

// ********************************* RANDOM GAUSS NUMBER *********************************
consoleVariable *cf_randGaussStd(consoleScope *s, consoleVariable *arg[])
{
    double mean = 0.0;
    double variance = 1.0;
    return new consoleVariable("", e3ga::mv(gaussNoise(mean, variance)));
    // return new consoleVariable("", i2ga(gaussNoise(mean, variance)));
}

// ********************************* CLD *********************************
consoleVariable *cf_cld(consoleScope *s, consoleVariable *arg[])
{
    g_state->removeAllDynStmt();
    return new consoleVariable("", e3ga::mv(0.0));
}

// ********************************* CLD *********************************
consoleVariable *cf_cld_1(consoleScope *s, consoleVariable *arg[])
{
    g_state->removeDynStmt(arg[0]->name());
    return new consoleVariable("", e3ga::mv(0.0));
}

// ********************************* START ANIMATION *********************************
consoleVariable *cf_start_animation(consoleScope *s, consoleVariable *arg[])
{
    g_state->startAnimation();
    return new consoleVariable("", e3ga::mv(0.0));
}

// ********************************* STOP ANIMATION *********************************
consoleVariable *cf_stop_animation(consoleScope *s, consoleVariable *arg[])
{
    g_state->stopAnimation();
    return new consoleVariable("", e3ga::mv(0.0));
}

// ********************************* PAUSE ANIMATION *********************************
consoleVariable *cf_pause_animation(consoleScope *s, consoleVariable *arg[])
{
    g_state->pauseAnimation();
    return new consoleVariable("", e3ga::mv(0.0));
}

// ********************************* RESET *********************************
consoleVariable *cf_reset(consoleScope *s, consoleVariable *arg[])
{
    if (!s->globalScope())
    {
        cprintf("Error: reset() can only be called in global scope\n");
    }
    else
        s->resetRequested(1);
    return new consoleVariable("", e3ga::mv(0.0));
}

// ********************************* CPRINT *********************************
consoleVariable *cf_cprint(consoleScope *s, consoleVariable *arg[])
{
    e3ga::mv tmp(0.0);

    cprintf("%s\n", arg[0]->name().c_str());

    return new consoleVariable("", tmp);
}

// ********************************* PROMPT *********************************
consoleVariable *cf_prompt(consoleScope *s, consoleVariable *arg[])
{
    if (gui_state && gui_state->m_console)
        gui_state->m_console->prompt(arg[0]->name().c_str());
    return new consoleVariable("", e3ga::mv(0.0));
}

// ********************************* PROMPT_DEFAULT *********************************
consoleVariable *cf_prompt_default(consoleScope *s, consoleVariable *arg[])
{
    if (gui_state && gui_state->m_console)
        gui_state->m_console->prompt(">> ");
    return new consoleVariable("", e3ga::mv(0.0));
}

// ********************************* MUTE WARNINGS *********************************
consoleVariable *cf_mute_warnings(consoleScope *s, consoleVariable *arg[])
{
    if (g_state)
        g_state->m_globalScope->supressWarnings(1);
    return new consoleVariable("", e3ga::mv(0.0));
}

// ********************************* UNMUTE WARNINGS *********************************
consoleVariable *cf_unmute_warnings(consoleScope *s, consoleVariable *arg[])
{
    if (g_state)
        g_state->m_globalScope->supressWarnings(0);
    return new consoleVariable("", e3ga::mv(0.0));
}

// ********************************* DIARY *********************************
consoleVariable *cf_diary(consoleScope *s, consoleVariable *arg[])
{
    const char *filename = NULL;
    if (gui_state)
        gui_state->saveDiary(filename);
    return new consoleVariable("", e3ga::mv(0.0));
}

// ********************************* DIARY *********************************
consoleVariable *cf_diary_filename(consoleScope *s, consoleVariable *arg[])
{
    const char *filename = arg[0]->name().c_str();
    if (gui_state)
        gui_state->saveDiary(filename);
    return new consoleVariable("", e3ga::mv(0.0));
}

// ********************************* SELECT *********************************
consoleVariable *cf_select(consoleScope *s, consoleVariable *arg[])
{
    // todo: if select was issued, don't override it in consoleScope:showassignmentlist
    if (gui_state)
    {
        gui_state->setCurrentObjectByName(arg[0]->name());
    }
    return arg[0];
}

// ********************************* REMOVE *********************************
consoleVariable *cf_remove(consoleScope *s, consoleVariable *arg[])
{
    if (g_state->removeObject(arg[0]->name()) < 0)
    {
        cprintf("Error: '%s' does not exist in global scope\n", arg[0]->name().c_str());
    }
    return new consoleVariable("", e3ga::mv(0.0));
}

// ********************************* ADD CONST *********************************
consoleVariable *cf_add_const(consoleScope *s, consoleVariable *arg[])
{
    if (!s->globalScope())
    {
        cprintf("add_const() can only be called in the global scope!");
    }
    else
    {
        ((consoleGlobalScope *)s)->addConstant(arg[0]->name(), arg[0]);
    }
    return new consoleVariable("", e3ga::mv(0.0));
}

// ********************************* RENAME BUILTIN CONST *********************************
consoleVariable *cf_rename_builtin_const(consoleScope *s, consoleVariable *arg[])
{
    if (!s->globalScope())
    {
        cprintf("rename_builtin_const() can only be called in the global scope!");
    }
    else
    {
        ((consoleGlobalScope *)s)->renameBuiltinConstant(arg[0]->name(), arg[1]->name());
    }
    return new consoleVariable("", e3ga::mv(0.0));
}

// ********************************* REMOVE CONST *********************************
consoleVariable *cf_remove_const(consoleScope *s, consoleVariable *arg[])
{
    if (!s->globalScope())
    {
        cprintf("remove_const() can only be called in the global scope!");
    }
    else
    {
        ((consoleGlobalScope *)s)->removeConstant(arg[0]->name());
    }
    return new consoleVariable("", e3ga::mv(0.0));
}

// ********************************* LABEL *********************************
consoleVariable *cf_label(consoleScope *s, consoleVariable *arg[])
{
    g_state->addTextObject(arg[0]->name().c_str(), arg[0]->name(), arg[0]->name() + "_label", LABEL_DYNAMIC | LABEL_3D,
                           -1);
    return new consoleVariable("", e3ga::mv(0.0));
}

// ********************************* LABEL 2 *********************************
consoleVariable *cf_label2(consoleScope *s, consoleVariable *arg[])
{
    g_state->addTextObject(arg[1]->name().c_str(), arg[0]->name(), arg[0]->name() + "_label", LABEL_DYNAMIC | LABEL_3D,
                           -1);
    return new consoleVariable("", e3ga::mv(0.0));
}

// ********************************* CAMROTFUNC *********************************
consoleVariable *cf_camrotfunc(consoleScope *s, consoleVariable *arg[])
{
    g_state->setRot(arg[0]->e(), arg[1]->scalar());
    /*	if (g_state->removeObject(arg[0]->name()) < 0) {
            cprintf("Error: '%s' does not exist in global scope\n", arg[0]->name().c_str());
        }*/
    return new consoleVariable("", e3ga::mv(0.0));
}

// ********************************* GET DRAW FLAGS *********************************
consoleVariable *cf_get_draw_flags(consoleScope *s, consoleVariable *arg[])
{
    return new consoleVariable("", e3ga::mv((double)arg[0]->forceFlags()));
}

// ********************************* GET INTERPRETATION *********************************
consoleVariable *cf_get_interpretation(consoleScope *s, consoleVariable *arg[])
{
    mvInt *mvi = arg[0]->interpretation();
    int interpretation = (mvi && mvi->m_valid) ? mvi->m_type : 0;
    if (mvi)
        delete mvi;
    return new consoleVariable("", e3ga::mv((double)interpretation));
}

// ********************************* AUTO COLOR *********************************

/*function sign_radiussquared(mv,interpretation)
// interpretation is get_interpretation result minus MVI_C3GA
// compute the sign of the radius squared of a blade
// the answer is 0 for objects without a sensible radius
{
    MVI_BLADE = 256;
    MVI_DUAL = 2048;
    MVI_ROUND = MVI_BLADE + 128;
    if (bit_and(interpretation, MVI_ROUND) ) {
        nimv = ni.mv;
        alphasize = mv ~mv/ (nimv ~nimv);
        if (bit_and(interpretation, MVI_DUAL) ) {
                if (alphasize > 0) return(1); else return(-1);
        } else {
                if (alphasize < 0) return(1); else return(-1);
        }
    } else {
        return(0);
    }
}*/

consoleVariable *cf_autocolorfunc(consoleScope *s, consoleVariable *arg[])
{
    int drawFlags = arg[0]->forceFlags();

    mvInt *mvi = arg[0]->interpretation();
    int interpretation = (mvi && mvi->m_valid) ? mvi->m_type : 0;

    int gr = arg[0]->grade();

    // default grade-related colors
    if (!((drawFlags & OFF_COLOR)))
    {
        switch (gr)
        {
        case 0:
            cf_black(s, arg);
            break;
        case 1:
            cf_red(s, arg);
            break;
        case 2:
            cf_blue(s, arg);
            break;
        case 3:
            cf_green(s, arg);
            break;
        case 4:
            cf_yellow(s, arg);
            break;
        case 5:
            cf_white(s, arg);
            break;
        }
    }

    if (!((drawFlags & OFF_ALPHA)))
    {
        if (interpretation == MVI_E3GA + MVI_TRIVECTOR)
            cf_alpha_internal(arg[0], 0.5f);
        else if (interpretation == MVI_C3GA + MVI_SPHERE)
            cf_alpha_internal(arg[0], 0.5f);
        else if (interpretation == MVI_C3GA + MVI_SPHERE + MVI_DUAL)
            cf_alpha_internal(arg[0], 0.5f);
        else if (interpretation == MVI_C3GA + MVI_PLANE)
            cf_alpha_internal(arg[0], 0.5f);
        else if (interpretation == MVI_C3GA + MVI_PLANE + MVI_DUAL)
            cf_alpha_internal(arg[0], 0.5f);
        else
            cf_alpha_internal(arg[0], 1.0f); // default: solid
    }

    if (!((drawFlags & OFF_STIPPLE)))
    {
        if ((interpretation & MVI_MODEL_MASK) == MVI_C3GA)
        {
            cf_no_stipple(s, arg);
            //			if ((interpretation & MVI_FREE_FLAT) == MVI_FREE_FLAT)
            if ((interpretation & MVI_BLADE_TYPE_MASK) == MVI_FREE_FLAT)
                cf_stipple(s, arg);
            else if (mvi && ((interpretation & MVI_BLADE_TYPE_MASK) == MVI_ROUND))
                if (mvi->m_scalar[0] < 0)
                    cf_stipple(s, arg);
        }
    }

    if (!((drawFlags & OFF_MAGNITUDE)))
    {
        if ((interpretation & MVI_MODEL_MASK) == MVI_C3GA)
        {
            if (((interpretation & MVI_BLADE_TYPE_MASK) == MVI_ROUND) ||
                ((interpretation & MVI_BLADE_TYPE_MASK) == MVI_BOUND_FLAT) ||
                ((interpretation & (MVI_BLADE_TYPE_MASK | 15)) == MVI_POINT) ||
                ((interpretation & (MVI_BLADE_TYPE_MASK | 15)) == MVI_FREE_SCALAR))
            {
                cf_no_weight(s, arg);
            }
        }
        else if ((interpretation & MVI_MODEL_MASK) == MVI_P3GA)
        {
            if ((interpretation & MVI_BLADE_TYPE_MASK) == MVI_BOUND_FLAT)
                cf_no_weight(s, arg);
        }
    }

    if (mvi)
        delete mvi;

    return arg[0];
}

consoleVariable *cf_console_ctrl_remove(consoleScope *s, consoleVariable *arg[])
{
    g_state->removeConsoleControl(arg[0]->name());

    return new consoleVariable("", e3ga::mv(0.0));
}

consoleVariable *cf_console_ctrl_bool(consoleScope *s, consoleVariable *arg[])
{
    g_state->addConsoleControlBool(arg[0]->name(), arg[0]->isTrue());
    return new consoleVariable("", e3ga::mv(0.0));
}

consoleVariable *cf_console_ctrl_bool_with_callback(consoleScope *s, consoleVariable *arg[])
{
    g_state->addConsoleControlBool(arg[0]->name(), arg[0]->isTrue(), arg[1]->name());
    return new consoleVariable("", e3ga::mv(0.0));
}

consoleVariable *cf_console_ctrl_range_cb(consoleScope *s, consoleVariable *arg[], bool callback)
{
    double minValue = arg[1]->scalar();
    double maxValue = arg[2]->scalar();

    double step = (maxValue - minValue) / 200;
    // 'round' step
    double rStep = 10e-40;
    while ((rStep * 10) < fabs(step))
        rStep *= 10;
    if (step < 0)
        rStep = -rStep;

    std::string callbackName = (callback) ? arg[3]->name() : "";

    g_state->addConsoleControlRange(arg[0]->name(), arg[0]->scalar(), minValue, maxValue, rStep, callbackName);

    return new consoleVariable("", e3ga::mv(0.0));
}

consoleVariable *cf_console_ctrl_range(consoleScope *s, consoleVariable *arg[])
{
    return cf_console_ctrl_range_cb(s, arg, false);
}

consoleVariable *cf_console_ctrl_range_with_callback(consoleScope *s, consoleVariable *arg[])
{
    return cf_console_ctrl_range_cb(s, arg, true);
}

consoleVariable *cf_console_ctrl_range_step_cb(consoleScope *s, consoleVariable *arg[], bool callback)
{
    double minValue = arg[1]->scalar();
    double maxValue = arg[2]->scalar();
    double step = arg[3]->scalar();

    std::string callbackName = (callback) ? arg[4]->name() : "";

    g_state->addConsoleControlRange(arg[0]->name(), arg[0]->scalar(), minValue, maxValue, step, callbackName);

    return new consoleVariable("", e3ga::mv(0.0));
}

consoleVariable *cf_console_ctrl_range_step(consoleScope *s, consoleVariable *arg[])
{
    return cf_console_ctrl_range_step_cb(s, arg, false);
}

consoleVariable *cf_console_ctrl_range_step_with_callback(consoleScope *s, consoleVariable *arg[])
{
    return cf_console_ctrl_range_step_cb(s, arg, true);
}

consoleVariable *cf_console_ctrl_selectionN_cb(consoleScope *s, consoleVariable *arg[], int nbSelection, bool callback)
{
    std::vector<std::pair<std::string, double>> options;
    int i;

    for (i = 1; i <= nbSelection; i++)
        options.push_back(std::make_pair(arg[i]->name(), arg[i]->scalar()));

    std::string callbackName = (callback) ? arg[nbSelection + 1]->name() : "";

    g_state->addConsoleControlSelection(arg[0]->name(), arg[0]->scalar(), options, callbackName);

    return new consoleVariable("", e3ga::mv(0.0));
}

consoleVariable *cf_console_ctrl_selection1(consoleScope *s, consoleVariable *arg[])
{
    return cf_console_ctrl_selectionN_cb(s, arg, 1, false);
}
consoleVariable *cf_console_ctrl_selection2(consoleScope *s, consoleVariable *arg[])
{
    return cf_console_ctrl_selectionN_cb(s, arg, 2, false);
}
consoleVariable *cf_console_ctrl_selection3(consoleScope *s, consoleVariable *arg[])
{
    return cf_console_ctrl_selectionN_cb(s, arg, 3, false);
}
consoleVariable *cf_console_ctrl_selection4(consoleScope *s, consoleVariable *arg[])
{
    return cf_console_ctrl_selectionN_cb(s, arg, 4, false);
}
consoleVariable *cf_console_ctrl_selection5(consoleScope *s, consoleVariable *arg[])
{
    return cf_console_ctrl_selectionN_cb(s, arg, 5, false);
}
consoleVariable *cf_console_ctrl_selection6(consoleScope *s, consoleVariable *arg[])
{
    return cf_console_ctrl_selectionN_cb(s, arg, 6, false);
}
consoleVariable *cf_console_ctrl_selection7(consoleScope *s, consoleVariable *arg[])
{
    return cf_console_ctrl_selectionN_cb(s, arg, 7, false);
}

consoleVariable *cf_console_ctrl_selection1_with_callback(consoleScope *s, consoleVariable *arg[])
{
    return cf_console_ctrl_selectionN_cb(s, arg, 1, true);
}
consoleVariable *cf_console_ctrl_selection2_with_callback(consoleScope *s, consoleVariable *arg[])
{
    return cf_console_ctrl_selectionN_cb(s, arg, 2, true);
}
consoleVariable *cf_console_ctrl_selection3_with_callback(consoleScope *s, consoleVariable *arg[])
{
    return cf_console_ctrl_selectionN_cb(s, arg, 3, true);
}
consoleVariable *cf_console_ctrl_selection4_with_callback(consoleScope *s, consoleVariable *arg[])
{
    return cf_console_ctrl_selectionN_cb(s, arg, 4, true);
}
consoleVariable *cf_console_ctrl_selection5_with_callback(consoleScope *s, consoleVariable *arg[])
{
    return cf_console_ctrl_selectionN_cb(s, arg, 5, true);
}
consoleVariable *cf_console_ctrl_selection6_with_callback(consoleScope *s, consoleVariable *arg[])
{
    return cf_console_ctrl_selectionN_cb(s, arg, 6, true);
}

consoleVariable *cf_help_general(consoleScope *s, consoleVariable *arg[])
{
    e3ga::mv tmp(0.0);

    cprintf("\nHelp is available on the following topics:\n"
            "help(system)\n"
            "help(constants)\n"
            "help(operators)\n"
            "help(products)\n"
            "help(functions)\n"
            "help(boolean)\n"
            "help(projective)\n"
            "help(conformal)\n"
            "help(cast)\n"
            "help(display)\n"
            "help(controls)\n"
            "help(all) -- lists all help\n");

    cprintf("\n\n");

    return new consoleVariable("", tmp);
}

consoleVariable *cf_help(consoleScope *s, consoleVariable *arg[])
{
    e3ga::mv tmp(0.0);
    int all = arg[0]->name() == "all";

    if (all || (arg[0]->name() == "system"))
    {
        cprintf("\nSystem commands:\n"
                "cprint(arg1): prints the string 'arg1' to the console(NO printf like formatting!)\n"
                "cmd(arg1): executes the command 'arg1' as if it had been read from a .geo file\n"
                "prompt(): sets the default console prompt\n"
                "prompt(arg1): sets the console prompt to arg1\n"
                "mute_warnings(): disables the printing of warnings to the console\n"
                "unmute_warnings(): enables the printing of warnings to the console\n"
                "diary(): saves command and output history to file\n"
                "select(arg1): selects arg1 (arg1 in global scope)\n"
                "remove(arg1): removes arg1 (arg1 in global scope)\n"
                "dynamic{arg1}: adds 'arg1' to the list of dynamic statements\n"
                "cld(): clears list of dynamic statements\n"
                "cld(name): removes the dynamic statement named 'name'\n"
                "start_animation(): starts the animation of dynamics depending on 'atime'\n"
                "stop_animation(): stops animation\n"
                "pause_animation(): pauses animation\n"
                "resume_animation(): resumes animation (synonym of start_animation())\n"
                "reset(): resets the entire viewer (console, dynamics, variables, etc)\n"
                "add_const(arg1): adds a constant. E.g.: add_const(I3 = e1 ^ e2 ^ e3);\n"
                "remove_const(arg1): removes a constant. E.g.: remove_const(I3);\n"
                "rename_builtin_const(arg1, arg2): renames builtin constant 'arg1' to 'arg2'.\n"
                "default_model([e3ga|p3ga|c3ga|c5ga|i2ga]): sets default model of scalars, e1, e2, e3 to the argument\n"
                "default_model(): sets model to the default\n"
                "inner_product(functionName): sets the function used to evaluate the inner product to 'functionName' "
                "(hip, mhip, lcont and rcont and the recommended functions)\n"
                "get_interpretation(arg1): returns an integer that gives information about how 'arg1' is interpreted "
                "(useful to make your own autocolor)\n"
                "get_draw_flags(arg1): returns an integer that gives information about how what drawing properties of "
                "'arg1' have been explicitly set (useful to make your own autocolor)\n"
                "label(arg1): draws a label at the position of arg1, the text of the label will be 'arg1'\n"
                "label(arg1, arg2): draws a label at the position of arg1, then text will be 'arg2'\n"
                "add_net_port(arg1): opens a new TCP network server on port 'arg1'\n"
                "remove_net_port(arg1): remove the TCP network server listening on port 'arg1'\n"
                "net_status(): gives a summary of network ports and connections\n"
                "net_close(): immediatelly shuts down all network ports and connections\n"
                "screenshot(filename, width, height, alsoWithoutLabels): renders screenshot\n"
                "set_window_title(title): sets the main window title to 'title'\n"
                "disable_mouse_interaction(): disables editing multivectors using the ctrl-right-mouse.\n"
                "enable_mouse_interaction(): enables editing multivectors using the ctrl-right-mouse.\n");
    }

    if (all || (arg[0]->name() == "constants"))
    {
        cprintf("\nAvailable constants:\n-All scalar numbers\n"
                "-e1, e2, e3: Euclidean basisvector\n"
                "-e0: Projective origin basisvector\n"
                "-ni, no: Conformal infinity and origin\n"
                "-gi, go: Grayscale infinity and origin\n"
                "-einf: Synonym for conformal infinity\n"
                "-pi: 3.1415926535897932384626433832795\n"
                "-e_: 2.7182818284590452353602874713527\n");
    }

    if (all || (arg[0]->name() == "operators"))
    {
        cprintf("\nAvailable unary operators:\n~ : reversion\n"
                "-  negation\n"
                "!  inversion\n"
                "Example of unary operator use: a = -e1\n");

        cprintf("\nAvailable binary operators (in order of precedence):\n"
                "^  outer product (precedence level 9)\n"
                "|  join (precedence level 8)\n"
                "&  meet (precedence level 8)\n"
                ".  inner product (precedence level 7)\n"
                "'space' geometric product (precedence level 6)\n"
                "*  synonym for geometric product (precedence level 6)\n"
                "/  inverse geometric product (precedence level 6)\n"
                "+  addition (precedence level 5)\n"
                "-  subtraction (precedence level 5)\n"
                "< less (precedence level 4)\n"
                "> greater (precedence level 4)\n"
                "<= less or equal (precedence level 4)\n"
                ">= greater or equal (precedence level 4)\n"
                "== equal (precedence level 3)\n"
                "!= not equal (precedence level 3)\n"
                "&& and (precedence level 2)\n"
                "|| or (precedence level 1)\n"
                "=  assignment (precedence level 0, right associative)\n"
                "All operators are left associative, except assignment (which isn't really an operator)\n"
                "Example of binary operator use: a = (e1 ^ e2) . c\n");
    }

    if (all || (arg[0]->name() == "products"))
    {
        cprintf("\nAvailable products:\n"
                "gp(arg1, arg2): returns the geometric product of arg1 and arg2\n"
                "igp(arg1, arg2): returns the inverse geometric product of arg1 and arg2\n"
                "op(arg1, arg2): returns the outer product of arg1 and arg2\n"
                "hip(arg1, arg2): returns the Hestenes inner product of arg1 and arg2\n"
                "mhip(arg1, arg2): returns the modifiedHestenes inner product of arg1 and arg2\n"
                "lcont(arg1, arg2): returns the left contraction of arg1 and arg2\n"
                "rcont(arg1, arg2): returns the right contraction of arg1 and arg2\n"
                "scp(arg1, arg2): returns the scalar product of arg1 and arg2\n"
                "gpem(arg1, arg2): returns the Euclidean Metric geometric product of arg1 and arg2\n"
                "lcem(arg1, arg2): returns the Euclidean Metric left contraction of arg1 and arg2\n"
                "meet(arg1, arg2): returns the meet of arg1 and arg2\n"
                "join(arg1, arg2): returns the join of arg1 and arg2\n"
                "cp(arg1, arg2): returns the commutator product of arg1 and arg2\n");
    }

    if (all || (arg[0]->name() == "functions"))
    {
        cprintf("\nAvailable functions:\n"
                "print(arg1): prints coordinates of arg1 to the console, returns 0\n"
                "print(arg1, arg2): prints coordinates of arg1 to the console with precision arg2 (e.g., arg2 = "
                "\"e\"), returns 0\n"
                "assign(arg1, arg2): assigns the value of arg2 to arg1, returns arg2\n"
                "add(arg1, arg2): returns arg1 + arg2\n"
                "sub(arg1, arg2): returns arg1 - arg2\n"
                "scalar(arg1): returns the scalar part of arg1\n"
                "dual(arg1): returns the dual of arg1 with respect to the full space\n"
                "dual(arg1, arg2): returns the dual of arg1 with respect to arg2\n"
                "reverse(arg1): returns the reverse of arg1\n"
                "clifford_conjugate(arg1): returns the clifford conjugate of arg1\n"
                "grade_involution(arg1): returns the grade involution of arg1\n"
                "inverse(arg1): returns the (versor) inverse of arg1\n"
                "general_inverse(arg1): returns the inverse of arg1 even if it is not a versor (returns 0 if inverse "
                "does not exist)\n"
                "negate(arg1): returns the negation of arg1\n");

        cprintf("\n"
                "grade(arg1, arg2): returns the grade 'arg2' part of arg1, e.g. grade(a, 2)\n"
                "grade(arg1): if arg1 is a blade, returns the grade of arg1, otherwise returns -1\n"
                "versor_parity(arg1): if arg1 is an even versor returns: 0, odd versor: 1; not a versor : -1.\n"
                "norm_2(arg1): returns the sum of the square of all coordinates of arg1\n"
                "norm_r(arg1): returns scp(arg1, reverse(arg1))\n"
                "norm(arg1): returns sqrt(abs(norm_r(arg1))) * sign(norm_r(arg1))\n"
                "normalize(arg1): returns arg1 / abs(norm_r(arg1))\n");

        cprintf("\n"
                "sqrt(arg1): returns sqrt(scalar(arg1))\n"
                "abs(arg1): returns abs(scalar(arg1))\n"
                "max(arg1, arg2): returns max(scalar(arg1), scalar(arg2))\n"
                "min(arg1, arg2): returns min(scalar(arg1), scalar(arg2))\n"
                "tan(arg1): returns tan(scalar(arg1)) -- [radians]\n"
                "sin(arg1): returns sin(scalar(arg1)) -- [radians]\n"
                "cos(arg1): returns cos(scalar(arg1)) -- [radians]\n"
                "atan(arg1): returns atan(scalar(arg1)) -- [radians]\n"
                "asin(arg1): returns asin(scalar(arg1)) -- [radians]\n"
                "acos(arg1): returns acos(scalar(arg1)) -- [radians]\n"
                "atan2(arg1): returns atan2(scalar(arg1), scalar(arg2)) -- [radians]\n"
                "sinh(arg1): returns sinh(scalar(arg1)) -- [radians]\n"
                "cosh(arg1): returns cosh(scalar(arg1)) -- [radians]\n"
                "asinh(arg1): returns asinh(scalar(arg1)) -- [radians]\n"
                "acosh(arg1): returns acosh(scalar(arg1)) -- [radians]\n"
                "sinc(arg1): returns sin(scalar(arg1)) / scalar(arg1) -- [radians]\n"
                "log(arg1): returns natural logarithm of scalar(arg1) (also of negative numbers)\n"
                "exp(arg1): returns exp(arg1) (inaccurate series expansion)\n"
                "pow(arg1, arg2): returns arg1 multiplied arg2 times with itself (arg2: integer >= 0))\n"
                "scalar_pow(arg1, arg2): arg1 raised to the power of arg2\n");

        cprintf("\n"
                "rand(): returns random number (uniform distribution between 0.0 and 1.0)\n"
                "randSeed(arg1): sets the random seed to arg1\n"
                "time(): returns current time, millisecond resolution. May be used for randSeed()\n"
                "randGauss(arg1, arg2): returns random number (gaussion distribution with specified mean; arg1 = mean, "
                "arg2 = variance)\n"
                "randGaussStd(): returns random number (gaussion distribution with mean = 0.0; variance = 1.0)\n");

        cprintf("\n"
                "versor_product(arg1, arg2): returns (arg1 arg2 inverse(arg1))\n"
                "vp(arg1, arg2): synonym of versor_product(arg1, arg2)\n"
                "inverse_versor_product(arg1, arg2): returns (inverse(arg1) arg2 arg1)\n"
                "ivp(arg1, arg2): synonym of inverse_versor_product(arg1, arg2)\n"
                "project(arg1, arg2): returns the projection of arg1 onto arg2\n"
                "reject(arg1, arg2): returns the rejection of arg1 from arg2\n"
                "factor(arg1, arg2): returns factor 'arg2' of arg1 (arg2 = integer from 1 to the grade of arg1)\n");

        cprintf("\n"
                "create_polygon(poly_name, [fill|loop|strip], vertex1, vertex2, ..., vertexN): creates a new polygon "
                "(max. 6 vertices)\n"

        );
        cprintf("\n"
                "create_arc(arc_name, versor, point, nbSegments): creates a new arc of 'nbSegments' segments by "
                "applying 'versor' to 'point' repeatedly\n");
    }

    if (all || (arg[0]->name() == "boolean"))
    {
        cprintf("\nBoolean functions (false = 0.0, true = any other value):\n"
                "equal(arg1, arg2): returns true if (arg1 - arg2) equals 0\n"
                "ne(arg1, arg2): returns true if (arg1 - arg2) does not equal 0\n"
                "less(arg1, arg2): returns true if scalar(arg1) < scalar(arg2)\n"
                "greater(arg1, arg2): returns true if scalar(arg1) > scalar(arg2)\n"
                "le(arg1, arg2): (less or equal) returns true if scalar(arg1) <= scalar(arg2)\n"
                "ge(arg1, arg2): (greater or equal) returns true if scalar(arg1) >= scalar(arg2)\n"
                "and(arg1, arg2): returns true if arg1 is true and arg2 is true\n"
                "or(arg1, arg2): returns true if arg1 is true or arg2 is true\n"
                "not(arg1): returns false if arg1 is true, and true if arg1 is false\n"
                "\n"
                "Bitwise functions (performed on 32 bit unsigned integers):\n"
                "bit_not(arg1): returns the bitwise 'not' of int(scalar(arg1))\n"
                "bit_and(arg1, arg2): returns the bitwise 'and' of int(scalar(arg1)) and int(scalar(arg2))\n"
                "bit_or(arg1, arg2): returns the bitwise 'or' of int(scalar(arg1)) and int(scalar(arg2))\n"
                "bit_xor(arg1, arg2): returns the bitwise 'xor' of int(scalar(arg1)) and int(scalar(arg2))\n"
                "bit_shift(arg1, arg2): returns bitwise shift left of int(scalar(arg1)) by int(scalar(arg2)) (can be "
                "negative for right shift)\n");
    }

    if (all || (arg[0]->name() == "projective"))
    {
        cprintf("\nSpecial functions for the projective model\n"
                "p3ga_point(arg1, arg2, arg3): returns the projective point constructed from the vector [arg1 e1 + "
                "arg2 e2 + arg3 e3]\n"
                "p3ga_point(arg1): returns the projective point constructed from the vector arg1\n");
    }

    if (all || (arg[0]->name() == "conformal"))
    {
        cprintf("\nSpecial functions for the conformal model\n"
                "c3ga_point(arg1, arg2, arg3): returns the conformal point constructed from the vector [arg1 e1 + arg2 "
                "e2 + arg3 e3]\n"
                "c3ga_point(arg1): returns the conformal point constructed from the vector arg1\n"
                "c3ga_point(arg1): returns the conformal point constructed from the projective point arg1\n"
                "translation_versor(arg1): returns a conformal versor that translates over vector arg1\n"
                "tv(arg1): synonym of translation_versor(arg1)\n"
                "translation_versor(arg1, arg2, arg3): returns a conformal versor that translates over vector [arg1 e1 "
                "+ arg2 e2 + arg3 e3]\n"
                "tv(arg1, arg2, arg3): synonym of translation_versor(arg1, arg2, arg3)\n");
    }

    if (all || (arg[0]->name() == "cast"))
    {
        cprintf("\nCasting functions between models:\n"
                "C-like casting:\n"
                "(e3ga)arg1: casts arg1 to euclidean model\n"
                "(p3ga)arg1: casts arg1 to projective model\n"
                "(c3ga)arg1: casts arg1 to conformal model of 3-D Euclidean space\n"
                "(c5ga)arg1: casts arg1 to conformal model of 5-D Euclidean space\n"
                "(i2ga)arg1: casts arg1 to image algebra \n"
                /*		"\nFunction-like casting:\n"
                        "cast_e3ga(arg1): casts arg1 to euclidean model\n"
                        "cast_p3ga(arg1): casts arg1 to projective model\n"
                        "cast_c3ga(arg1): casts arg1 to conformal model\n"	*/
        );
    }

    if (all || (arg[0]->name() == "display"))
    {
        cprintf("\nSpecial functions for controlling display & drawing:\n"
                "clf(): clears the display (removes all variables/objects)\n"
                "clc(): clears the console\n");

        /*		cprintf("\n"
                "set_object_color(arg1, colorName): sets color of 'arg1' to colorName (red, green, blue, white, magenta,
           yellow, cyan, black, gray, grey)\n" "set_object_color(arg1, R, G, B): sets color of 'arg1' to RGB\n"
                "set_object_color(arg1, R, G, B, A): sets color of 'arg1' to RGBA\n");*/

        cprintf("\n"
                "red(arg1), green(arg1), blue(arg1), ...\n"
                "white(arg1), magenta(arg1), yellow(arg1), ...\n"
                "cyan(arg1), black(arg1), grey(arg1), ...\n"
                "gray(arg1): (DEPRECATED) sets the color of arg1\n");

        cprintf("\n"
                "color(arg1, arg2, arg3, arg4): sets the color of arg1 to rgb value (arg2, arg3, arg4)\n"
                "color(arg1, arg2, arg3, arg4, arg5): sets the color of arg1 to rgba value (arg2, arg3, arg4, arg5)\n"
                "alpha(arg1, arg2): sets the alpha (opacity) of arg1 to arg2\n");

        cprintf("\n"
                "dm1(arg1), dm2(arg1), dm3(arg1), dm4(arg1), ...\n"
                "dm5(arg1), dm6(arg1), dm7(arg1): sets the 'draw method' of arg1\n");

        cprintf("\n"
                "stipple(arg1): draws arg1 stippled\n"
                "no_stipple(arg1): draws arg1 not stippled\n"
                "wireframe(arg1): draws arg1 in wireframe\n"
                "no_wireframe(arg1): draws arg1 without wireframe\n"
                "outline(arg1): outlines arg1\n"
                "no_outline(arg1): does not outline arg1\n"
                "weight(arg1): draws the weight of arg1\n"
                "no_weight(arg1): does not draw the weight of arg1\n"
                "ori(arg1): draws the orientation of arg1\n"
                "no_ori(arg1): does not draw the orientation of arg1\n"
                "mute(arg1): disables printing of arg1 to console\n"
                "unmute(arg1): enables printing of arg1 to console\n"
                "hide(arg1): hides arg1\n"
                "show(arg1): shows arg1\n");

        cprintf("\n"
                "get_color(arg1): returns a vector with rgb color of arg1\n"
                "get_alpha(arg1): returns a scalar with alpha of arg1\n"
                "get_stipple(arg1): returns a boolean scalar with flag stipple of arg1\n"
                "get_wireframe(arg1): returns a boolean scalar with flag wireframe of arg1\n"
                "get_outline(arg1): returns a boolean scalar with flag outline of arg1\n"
                "get_weight(arg1): returns a boolean scalar with flag weight of arg1\n"
                "get_ori(arg1): returns a boolean scalar with flag ori of arg1\n"
                "get_hide(arg1): returns a boolean scalar with flag hide of arg1\n");

        cprintf("\n"
                "versor(arg1): forces versor interpretation of arg1\n"
                "blade(arg1): forces blade interpretation of arg1\n");

        cprintf(
            "\n"
            "set_point_size(arg): sets the (drawing) size of points to 'arg' (default = 0.1)\n"
            "set_text_scale(arg): sets the scaling of text and labels to 'arg' (default = 1.0)\n"
            "set_line_width(arg): sets the width of most lines to 'arg' (default = 2.0)\n"
            "set_gl_lines(arg): sets the use of regular OpenGL lines (arg != 0), or 'tubes' (arg == 0).\n"
            "set_vector_head_size(arg): sets the size of vector 'heads' to 'arg' (default = -1.0 = auto size)\n"
            "set_point_sub_div(arg): sets the amount of subdivision of the geosphere used for rendering points "
            "(default = 2)\n"
            "set_sphere_sub_div(arg): sets the amount of subdivision of the geosphere used for rendering spheres "
            "(default = 3)\n"
            "set_near_far_clip_planes(near, far): sets the closest and furthest distance at which objects get drawn. "
            "Passing invalid (e.g. negative) values causes a reset to the default (default near=0.95, far=100.0).\n");
    }

    if (all || (arg[0]->name() == "controls"))
    {
        cprintf("\nFunctions for creating and removing console controls:\n"
                "ctrl_bool(name = value): creates a boolean control with 'name', set to value\n"
                "ctrl_range(name = value, min_value, max_value): creates a slider control with 'name', set to value, "
                "limited to min and max values\n"
                "ctrl_range(name = value, min_value, max_value, step): creates a slider control with 'name', set to "
                "value, limited to min and max values, with 'step'\n"
                "ctrl_select(name = value, option1 = value1, ..., optionN = valueN): creates a selection menu with "
                "'name', set to value. A maximum op 7 options can be specified, 'value' must be one of the options\n"
                "ctrl_bool_with_callback(name = value, \"callbackName\"): like ctrl_bool(), but calls 'callbackName()' "
                "when value changes.\n"
                "ctrl_range_with_callback(name = value, min_value, max_value, \"callbackName\"): like ctrl_range(), "
                "but calls 'callbackName()' when value changes.\n"
                "ctrl_range_with_callback(name = value, min_value, max_value, step, \"callbackName\"): like "
                "ctrl_range(), but calls 'callbackName()' when value changes.\n"
                "ctrl_select_with_callback(name = value, option1 = value1, ..., optionN = valueN, \"callbackName\"): "
                "like ctrl_select(), but calls 'callbackName()' when value changes.\n"
                "ctrl_remove(name): removes any control with name 'name'\n");
    }

    cprintf("\n\n");

    return new consoleVariable("", tmp);
}


namespace
{
const consoleFunc c_generalConsoleFunc[] = {
    {"add_net_port", NULL, cf_add_net_port, 1, {CVF_LHS | CVF_RHS | CVF_ANY_MODEL}},
    {"remove_net_port", NULL, cf_remove_net_port, 1, {CVF_LHS | CVF_RHS | CVF_ANY_MODEL}},
    {"net_status", NULL, cf_net_status, 0, {0}},
    {"net_close", NULL, cf_net_close, 0, {0}},
    {"disable_mouse_interaction", NULL, cf_disable_mouse_interaction, 0, {0}},
    {"enable_mouse_interaction", NULL, cf_enable_mouse_interaction, 0, {0}},
    {"screenshot",
     NULL,
     cf_screenshot,
     4,
     {CVF_RHS | CVF_ANY_MODEL, CVF_LHS | CVF_RHS | CVF_ANY_MODEL, CVF_LHS | CVF_RHS | CVF_ANY_MODEL,
      CVF_LHS | CVF_RHS | CVF_ANY_MODEL}},
    {"set_window_title", NULL, cf_set_window_title, 1, {CVF_RHS | CVF_ANY_MODEL}},
    {"set_point_size", NULL, cf_set_point_size, 1, {CVF_LHS | CVF_RHS | CVF_ANY_MODEL}},
    {"set_near_far_clip_planes",
     NULL,
     cf_set_near_far_clip_planes,
     2,
     {CVF_LHS | CVF_RHS | CVF_ANY_MODEL, CVF_LHS | CVF_RHS | CVF_ANY_MODEL}},
    {"set_text_scale", NULL, cf_set_text_scale, 1, {CVF_LHS | CVF_RHS | CVF_ANY_MODEL}},
    {"set_line_width", NULL, cf_set_line_width, 1, {CVF_LHS | CVF_RHS | CVF_ANY_MODEL}},
    {"set_vector_head_size", NULL, cf_set_vector_head_size, 1, {CVF_LHS | CVF_RHS | CVF_ANY_MODEL}},
    {"set_gl_lines", NULL, cf_set_gl_lines, 1, {CVF_LHS | CVF_RHS | CVF_ANY_MODEL}},
    {"set_point_sub_div", NULL, cf_set_point_sub_div, 1, {CVF_LHS | CVF_RHS | CVF_ANY_MODEL}},
    {"set_sphere_sub_div", NULL, cf_set_sphere_sub_div, 1, {CVF_LHS | CVF_RHS | CVF_ANY_MODEL}},
    {"cmd", NULL, cf_cmd, 1, {CVF_LHS | CVF_RHS | CVF_ANY_MODEL}},
    {"prompt", NULL, cf_prompt, 1, {CVF_LHS | CVF_RHS | CVF_ANY_MODEL}},
    {"prompt", NULL, cf_prompt_default, 0, {0}},
    {"mute_warnings", NULL, cf_mute_warnings, 0, {0}},
    {"unmute_warnings", NULL, cf_unmute_warnings, 0, {0}},
    {"diary", NULL, cf_diary, 0, {0}},
    {"diary", NULL, cf_diary_filename, 1, {CVF_LHS | CVF_RHS | CVF_ANY_MODEL}},
    {"cprint", NULL, cf_cprint, 1, {CVF_LHS | CVF_RHS | CVF_ANY_MODEL}},
    {"select", NULL, cf_select, 1, {CVF_LHS | CVF_RHS | CVF_ANY_MODEL}},
    {"remove", NULL, cf_remove, 1, {CVF_LHS | CVF_RHS | CVF_ANY_MODEL}},
    {"get_draw_flags", NULL, cf_get_draw_flags, 1, {CVF_RHS | CVF_ANY_MODEL}},
    {"get_interpretation", NULL, cf_get_interpretation, 1, {CVF_RHS | CVF_ANY_MODEL}},
    {"autocolorfunc", NULL, cf_autocolorfunc, 1, {CVF_RHS | CVF_ANY_MODEL}},
    {"camrot", NULL, cf_camrotfunc, 2, {CVF_RHS | MVI_E3GA, CVF_RHS | CVF_ANY_MODEL}},
    {"add_const", NULL, cf_add_const, 1, {CVF_RHS | CVF_ANY_MODEL}},
    {"remove_const", NULL, cf_remove_const, 1, {CVF_RHS | CVF_LHS | CVF_ANY_MODEL}},
    {"rename_builtin_const",
     NULL,
     cf_rename_builtin_const,
     2,
     {CVF_RHS | CVF_LHS | CVF_ANY_MODEL, CVF_RHS | CVF_LHS | CVF_ANY_MODEL}},
    {"label", NULL, cf_label, 1, {CVF_RHS | CVF_ANY_MODEL}},
    {"label", NULL, cf_label2, 2, {CVF_RHS | CVF_ANY_MODEL, CVF_RHS | CVF_ANY_MODEL}},

    {"ctrl_bool", NULL, cf_console_ctrl_bool, 1, {CVF_RHS | CVF_ANY_MODEL}},
    {"ctrl_range",
     NULL,
     cf_console_ctrl_range,
     3,
     {CVF_RHS | CVF_ANY_MODEL, CVF_LHS | CVF_RHS | CVF_ANY_MODEL, CVF_LHS | CVF_RHS | CVF_ANY_MODEL}},
    {"ctrl_range",
     NULL,
     cf_console_ctrl_range_step,
     4,
     {CVF_RHS | CVF_ANY_MODEL, CVF_LHS | CVF_RHS | CVF_ANY_MODEL, CVF_LHS | CVF_RHS | CVF_ANY_MODEL,
      CVF_LHS | CVF_RHS | CVF_ANY_MODEL}},
    {"ctrl_select", NULL, cf_console_ctrl_selection1, 2, {CVF_RHS | CVF_ANY_MODEL, CVF_RHS | CVF_ANY_MODEL}},
    {"ctrl_select",
     NULL,
     cf_console_ctrl_selection2,
     3,
     {CVF_RHS | CVF_ANY_MODEL, CVF_RHS | CVF_ANY_MODEL, CVF_RHS | CVF_ANY_MODEL}},
    {"ctrl_select",
     NULL,
     cf_console_ctrl_selection3,
     4,
     {CVF_RHS | CVF_ANY_MODEL, CVF_RHS | CVF_ANY_MODEL, CVF_RHS | CVF_ANY_MODEL, CVF_RHS | CVF_ANY_MODEL}},
    {"ctrl_select",
     NULL,
     cf_console_ctrl_selection4,
     5,
     {CVF_RHS | CVF_ANY_MODEL, CVF_RHS | CVF_ANY_MODEL, CVF_RHS | CVF_ANY_MODEL, CVF_RHS | CVF_ANY_MODEL,
      CVF_RHS | CVF_ANY_MODEL}},
    {"ctrl_select",
     NULL,
     cf_console_ctrl_selection5,
     6,
     {CVF_RHS | CVF_ANY_MODEL, CVF_RHS | CVF_ANY_MODEL, CVF_RHS | CVF_ANY_MODEL, CVF_RHS | CVF_ANY_MODEL,
      CVF_RHS | CVF_ANY_MODEL, CVF_RHS | CVF_ANY_MODEL}},
    {"ctrl_select",
     NULL,
     cf_console_ctrl_selection6,
     7,
     {CVF_RHS | CVF_ANY_MODEL, CVF_RHS | CVF_ANY_MODEL, CVF_RHS | CVF_ANY_MODEL, CVF_RHS | CVF_ANY_MODEL,
      CVF_RHS | CVF_ANY_MODEL, CVF_RHS | CVF_ANY_MODEL, CVF_RHS | CVF_ANY_MODEL}},
    {"ctrl_select",
     NULL,
     cf_console_ctrl_selection7,
     8,
     {CVF_RHS | CVF_ANY_MODEL, CVF_RHS | CVF_ANY_MODEL, CVF_RHS | CVF_ANY_MODEL, CVF_RHS | CVF_ANY_MODEL,
      CVF_RHS | CVF_ANY_MODEL, CVF_RHS | CVF_ANY_MODEL, CVF_RHS | CVF_ANY_MODEL, CVF_RHS | CVF_ANY_MODEL}},

    {"ctrl_bool_with_callback",
     NULL,
     cf_console_ctrl_bool_with_callback,
     2,
     {CVF_RHS | CVF_ANY_MODEL, CVF_RHS | CVF_ANY_MODEL}},
    {"ctrl_range_with_callback",
     NULL,
     cf_console_ctrl_range_with_callback,
     4,
     {CVF_RHS | CVF_ANY_MODEL, CVF_LHS | CVF_RHS | CVF_ANY_MODEL, CVF_LHS | CVF_RHS | CVF_ANY_MODEL,
      CVF_RHS | CVF_ANY_MODEL}},
    {"ctrl_range_with_callback",
     NULL,
     cf_console_ctrl_range_step_with_callback,
     5,
     {CVF_RHS | CVF_ANY_MODEL, CVF_LHS | CVF_RHS | CVF_ANY_MODEL, CVF_LHS | CVF_RHS | CVF_ANY_MODEL,
      CVF_LHS | CVF_RHS | CVF_ANY_MODEL, CVF_RHS | CVF_ANY_MODEL}},
    {"ctrl_select_with_callback",
     NULL,
     cf_console_ctrl_selection1_with_callback,
     3,
     {CVF_RHS | CVF_ANY_MODEL, CVF_RHS | CVF_ANY_MODEL, CVF_RHS | CVF_ANY_MODEL}},
    {"ctrl_select_with_callback",
     NULL,
     cf_console_ctrl_selection2_with_callback,
     4,
     {CVF_RHS | CVF_ANY_MODEL, CVF_RHS | CVF_ANY_MODEL, CVF_RHS | CVF_ANY_MODEL, CVF_RHS | CVF_ANY_MODEL}},
    {"ctrl_select_with_callback",
     NULL,
     cf_console_ctrl_selection3_with_callback,
     5,
     {CVF_RHS | CVF_ANY_MODEL, CVF_RHS | CVF_ANY_MODEL, CVF_RHS | CVF_ANY_MODEL, CVF_RHS | CVF_ANY_MODEL,
      CVF_RHS | CVF_ANY_MODEL}},
    {"ctrl_select_with_callback",
     NULL,
     cf_console_ctrl_selection4_with_callback,
     6,
     {CVF_RHS | CVF_ANY_MODEL, CVF_RHS | CVF_ANY_MODEL, CVF_RHS | CVF_ANY_MODEL, CVF_RHS | CVF_ANY_MODEL,
      CVF_RHS | CVF_ANY_MODEL, CVF_RHS | CVF_ANY_MODEL}},
    {"ctrl_select_with_callback",
     NULL,
     cf_console_ctrl_selection5_with_callback,
     7,
     {CVF_RHS | CVF_ANY_MODEL, CVF_RHS | CVF_ANY_MODEL, CVF_RHS | CVF_ANY_MODEL, CVF_RHS | CVF_ANY_MODEL,
      CVF_RHS | CVF_ANY_MODEL, CVF_RHS | CVF_ANY_MODEL, CVF_RHS | CVF_ANY_MODEL}},
    {"ctrl_select_with_callback",
     NULL,
     cf_console_ctrl_selection6_with_callback,
     8,
     {CVF_RHS | CVF_ANY_MODEL, CVF_RHS | CVF_ANY_MODEL, CVF_RHS | CVF_ANY_MODEL, CVF_RHS | CVF_ANY_MODEL,
      CVF_RHS | CVF_ANY_MODEL, CVF_RHS | CVF_ANY_MODEL, CVF_RHS | CVF_ANY_MODEL, CVF_RHS | CVF_ANY_MODEL}},

    {"ctrl_remove", NULL, cf_console_ctrl_remove, 1, {CVF_RHS | CVF_ANY_MODEL}},

    {"cld", NULL, cf_cld, 0, {0}},
    {"cld", NULL, cf_cld_1, 1, {CVF_RHS | CVF_ANY_MODEL}},

    {"rand", NULL, cf_rand, 0, {0}},
    {"time", NULL, cf_time, 0, {0}},
    {"randGaussStd", NULL, cf_randGaussStd, 0, {}},

    {"start_animation", NULL, cf_start_animation, 0, {0}},
    {"stop_animation", NULL, cf_stop_animation, 0, {0}},
    {"pause_animation", NULL, cf_pause_animation, 0, {0}},
    {"resume_animation", NULL, cf_start_animation, 0, {0}},

    {"reset", NULL, cf_reset, 0, {0}},
    {"help", NULL, cf_help_general, 0, {0}},
    {"help", NULL, cf_help, 1, {CVF_LHS | CVF_RHS | CVF_ANY_MODEL}},

    {"p3ga_point", NULL, cf_p3ga_point3, 3, {CVF_RHS | MVI_E3GA, CVF_RHS | MVI_E3GA, CVF_RHS | MVI_E3GA}},
    {"p3ga_point", NULL, cf_p3ga_point1, 1, {CVF_RHS | MVI_E3GA}},

    {"c3ga_point", NULL, cf_c3ga_point3, 3, {CVF_RHS | MVI_E3GA, CVF_RHS | MVI_E3GA, CVF_RHS | MVI_E3GA}},
    {"c3ga_point", NULL, cf_c3ga_point1, 1, {CVF_RHS | MVI_E3GA}},
    {"c3ga_point", NULL, cf_c3ga_pointP1, 1, {CVF_RHS | MVI_P3GA}},

    {"translation_versor",
     NULL,
     cf_translation_versor3,
     3,
     {CVF_RHS | MVI_E3GA, CVF_RHS | MVI_E3GA, CVF_RHS | MVI_E3GA}},
    {"tv", NULL, cf_translation_versor3, 3, {CVF_RHS | MVI_E3GA, CVF_RHS | MVI_E3GA, CVF_RHS | MVI_E3GA}},
    {"translation_versor", NULL, cf_translation_versor1, 1, {CVF_RHS | MVI_E3GA}},
    {"tv", NULL, cf_translation_versor1, 1, {CVF_RHS | MVI_E3GA}},

    {"clf", NULL, cf_clf, 0, {0}},
    {"clc", NULL, cf_clc, 0, {0}},
    {"set_object_fg_color", NULL, cf_set_object_fg_color_vector, 2, {CVF_RHS | CVF_ANY_MODEL, CVF_RHS | MVI_E3GA}},
    {"set_object_fg_color",
     NULL,
     cf_set_object_fg_color_RGB,
     4,
     {CVF_RHS | CVF_ANY_MODEL, CVF_RHS | MVI_E3GA, CVF_RHS | MVI_E3GA, CVF_RHS | MVI_E3GA}},
    {"set_object_fg_color",
     NULL,
     cf_set_object_fg_color_RGBA,
     5,
     {CVF_RHS | CVF_ANY_MODEL, CVF_RHS | MVI_E3GA, CVF_RHS | MVI_E3GA, CVF_RHS | MVI_E3GA, CVF_RHS | MVI_E3GA}},

    {"red", NULL, cf_red, 1, {CVF_RHS | CVF_ANY_MODEL}},
    {"green", NULL, cf_green, 1, {CVF_RHS | CVF_ANY_MODEL}},
    {"blue", NULL, cf_blue, 1, {CVF_RHS | CVF_ANY_MODEL}},
    {"white", NULL, cf_white, 1, {CVF_RHS | CVF_ANY_MODEL}},
    {"magenta", NULL, cf_magenta, 1, {CVF_RHS | CVF_ANY_MODEL}},
    {"yellow", NULL, cf_yellow, 1, {CVF_RHS | CVF_ANY_MODEL}},
    {"cyan", NULL, cf_cyan, 1, {CVF_RHS | CVF_ANY_MODEL}},
    {"black", NULL, cf_black, 1, {CVF_RHS | CVF_ANY_MODEL}},
    {"grey", NULL, cf_grey, 1, {CVF_RHS | CVF_ANY_MODEL}},
    {"gray", NULL, cf_grey, 1, {CVF_RHS | CVF_ANY_MODEL}},

    {"color", NULL, cf_color1, 2, {CVF_RHS | CVF_ANY_MODEL, CVF_RHS | MVI_E3GA}},
    {"color",
     NULL,
     cf_color3,
     4,
     {CVF_RHS | CVF_ANY_MODEL, CVF_RHS | MVI_E3GA, CVF_RHS | MVI_E3GA, CVF_RHS | MVI_E3GA}},
    {"color",
     NULL,
     cf_color4,
     5,
     {CVF_RHS | CVF_ANY_MODEL, CVF_RHS | MVI_E3GA, CVF_RHS | MVI_E3GA, CVF_RHS | MVI_E3GA, CVF_RHS | MVI_E3GA}},
    {"alpha", NULL, cf_alpha, 2, {CVF_RHS | CVF_ANY_MODEL, CVF_RHS | MVI_E3GA}},

    {"versor", NULL, cf_versor, 1, {CVF_RHS | CVF_ANY_MODEL}},
    {"blade", NULL, cf_blade, 1, {CVF_RHS | CVF_ANY_MODEL}},

    {"dm1", NULL, cf_dm1, 1, {CVF_RHS | CVF_ANY_MODEL}},
    {"dm2", NULL, cf_dm2, 1, {CVF_RHS | CVF_ANY_MODEL}},
    {"dm3", NULL, cf_dm3, 1, {CVF_RHS | CVF_ANY_MODEL}},
    {"dm4", NULL, cf_dm4, 1, {CVF_RHS | CVF_ANY_MODEL}},
    {"dm5", NULL, cf_dm5, 1, {CVF_RHS | CVF_ANY_MODEL}},
    {"dm6", NULL, cf_dm6, 1, {CVF_RHS | CVF_ANY_MODEL}},
    {"dm7", NULL, cf_dm7, 1, {CVF_RHS | CVF_ANY_MODEL}},

    {"stipple", NULL, cf_stipple, 1, {CVF_RHS | CVF_ANY_MODEL}},
    {"no_stipple", NULL, cf_no_stipple, 1, {CVF_RHS | CVF_ANY_MODEL}},
    {"ori", NULL, cf_ori, 1, {CVF_RHS | CVF_ANY_MODEL}},
    {"no_ori", NULL, cf_no_ori, 1, {CVF_RHS | CVF_ANY_MODEL}},
    {"mute", NULL, cf_mute, 1, {CVF_RHS | CVF_ANY_MODEL}},
    {"unmute", NULL, cf_unmute, 1, {CVF_RHS | CVF_ANY_MODEL}},
    {"wireframe", NULL, cf_wireframe, 1, {CVF_RHS | CVF_ANY_MODEL}},
    {"no_wireframe", NULL, cf_no_wireframe, 1, {CVF_RHS | CVF_ANY_MODEL}},
    {"shade", NULL, cf_shade, 1, {CVF_RHS | CVF_ANY_MODEL}},
    {"no_shade", NULL, cf_no_shade, 1, {CVF_RHS | CVF_ANY_MODEL}},
    {"hide", NULL, cf_hide, 1, {CVF_RHS | CVF_ANY_MODEL}},
    {"show", NULL, cf_show, 1, {CVF_RHS | CVF_ANY_MODEL}},
    {"outline", NULL, cf_outline, 1, {CVF_RHS | CVF_ANY_MODEL}},
    {"no_outline", NULL, cf_no_outline, 1, {CVF_RHS | CVF_ANY_MODEL}},
    {"weight", NULL, cf_weight, 1, {CVF_RHS | CVF_ANY_MODEL}},
    {"no_weight", NULL, cf_no_weight, 1, {CVF_RHS | CVF_ANY_MODEL}},

    {"get_color", NULL, cf_get_color, 1, {CVF_RHS | CVF_ANY_MODEL}},
    {"get_alpha", NULL, cf_get_alpha, 1, {CVF_RHS | CVF_ANY_MODEL}},
    {"get_stipple", NULL, cf_get_stipple, 1, {CVF_RHS | CVF_ANY_MODEL}},
    {"get_wireframe", NULL, cf_get_wireframe, 1, {CVF_RHS | CVF_ANY_MODEL}},
    {"get_outline", NULL, cf_get_outline, 1, {CVF_RHS | CVF_ANY_MODEL}},
    {"get_weight", NULL, cf_get_weight, 1, {CVF_RHS | CVF_ANY_MODEL}},
    {"get_ori", NULL, cf_get_ori, 1, {CVF_RHS | CVF_ANY_MODEL}},
    {"get_hide", NULL, cf_get_hide, 1, {CVF_RHS | CVF_ANY_MODEL}},

    {"bit_not", NULL, cf_bit_not, 1, {CVF_RHS | CVF_ANY_MODEL}},
    {"bit_and", NULL, cf_bit_and, 2, {CVF_RHS | CVF_ANY_MODEL, CVF_RHS | CVF_ANY_MODEL}},
    {"bit_or", NULL, cf_bit_or, 2, {CVF_RHS | CVF_ANY_MODEL, CVF_RHS | CVF_ANY_MODEL}},
    {"bit_xor", NULL, cf_bit_xor, 2, {CVF_RHS | CVF_ANY_MODEL, CVF_RHS | CVF_ANY_MODEL}},
    {"bit_shift", NULL, cf_bit_shift, 2, {CVF_RHS | CVF_ANY_MODEL, CVF_RHS | CVF_ANY_MODEL}},

    // arg 1 = name, arg 2 = draw mode, other args are vertex names
    {"create_polygon",
     NULL,
     cf_create_polygon_3,
     5,
     {CVF_RHS | CVF_ANY_MODEL, CVF_RHS | CVF_ANY_MODEL, CVF_RHS | CVF_ANY_MODEL, CVF_RHS | CVF_ANY_MODEL,
      CVF_RHS | CVF_ANY_MODEL}},
    {"create_polygon",
     NULL,
     cf_create_polygon_4,
     6,
     {CVF_RHS | CVF_ANY_MODEL, CVF_RHS | CVF_ANY_MODEL, CVF_RHS | CVF_ANY_MODEL, CVF_RHS | CVF_ANY_MODEL,
      CVF_RHS | CVF_ANY_MODEL, CVF_RHS | CVF_ANY_MODEL}},
    {"create_polygon",
     NULL,
     cf_create_polygon_5,
     7,
     {CVF_RHS | CVF_ANY_MODEL, CVF_RHS | CVF_ANY_MODEL, CVF_RHS | CVF_ANY_MODEL, CVF_RHS | CVF_ANY_MODEL,
      CVF_RHS | CVF_ANY_MODEL, CVF_RHS | CVF_ANY_MODEL, CVF_RHS | CVF_ANY_MODEL}},
    {"create_polygon",
     NULL,
     cf_create_polygon_6,
     8,
     {CVF_RHS | CVF_ANY_MODEL, CVF_RHS | CVF_ANY_MODEL, CVF_RHS | CVF_ANY_MODEL, CVF_RHS | CVF_ANY_MODEL,
      CVF_RHS | CVF_ANY_MODEL, CVF_RHS | CVF_ANY_MODEL, CVF_RHS | CVF_ANY_MODEL, CVF_RHS | CVF_ANY_MODEL}},

    {"create_arc",
     NULL,
     cf_create_arc,
     4,
     {CVF_RHS | CVF_ANY_MODEL, CVF_RHS | CVF_ANY_MODEL, CVF_RHS | CVF_ANY_MODEL, CVF_RHS | CVF_ANY_MODEL}},

    {NULL, NULL, NULL, 0, {}}};
} // namespace

const consoleFunc *consoleBuiltinFunctions()
{
    static const std::vector<consoleFunc> table = [] {
        std::vector<consoleFunc> t;
        for (const consoleFunc *f = c_generalConsoleFunc; f->name; f++)
            t.push_back(*f);
        appendAlgebraFunctions(t);
        t.push_back(consoleFunc{NULL, NULL, NULL, 0, {}});
        return t;
    }();
    return table.data();
}
