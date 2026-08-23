//
// Main application window.
//
#include "main_window.h"
#include "file_handling.h"

#include <FL/filename.H>

#include <stdio.h>

Fl_Double_Window *app_window = NULL;

void build_app_window()
{
    app_window = new Fl_Double_Window(640, 480, "FLTK Editor");
    app_window->xclass("fl_editor");
}

void update_title()
{
    const char *fname = NULL;
    if (app_filename[0])
        fname = fl_filename_name(app_filename);
    if (fname)
    {
        char buf[FL_PATH_MAX + 3];
        buf[FL_PATH_MAX + 2] = '\0'; // ensure that the buffer is always terminated
        if (text_changed)
        {
            snprintf(buf, FL_PATH_MAX + 2, "%s *", fname);
        }
        else
        {
            snprintf(buf, FL_PATH_MAX + 2, "%s", fname);
        }
        app_window->copy_label(buf);
    }
    else
    {
        app_window->label("FLTK Editor");
    }
}
