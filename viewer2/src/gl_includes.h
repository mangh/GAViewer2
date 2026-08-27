#ifndef _GAVIEWER_GL_INCLUDES_H_
#define _GAVIEWER_GL_INCLUDES_H_

// umbrella header: the headers below are deliberately provided to includers
// IWYU pragma: begin_exports

#ifdef WIN32
#include <windows.h>
#endif

#ifdef __MACH__
#include <opengl/gl.h>
#include <opengl/glu.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif

// IWYU pragma: end_exports


#endif /* _GAVIEWER_GL_INCLUDES_H_ */
