//
// Main application window.
//
#include "main_window.h"
#include "file_handling.h"

#include <FL/filename.H>
#include <FL/platform.H>

#include <stdio.h>

Fl_Double_Window *_app_window = NULL;

void build_app_window()
{
    _app_window = new Fl_Double_Window(640, 480, "FLTK Editor");
    _app_window->xclass("fl_editor");
}

void update_app_title(const char *info)
{
    _app_window->label(info ? info : "FLTK Editor");
}

static int args_handler(int argc, char **argv, int &i)
{
    if (argv && argv[i] && argv[i][0] != '-')
    {
        file_load(argv[i]);
        i++;
        return 1;
    }
    return 0;
}

int app_run(int &argc, char **argv)
{
    // On Windows, command line arguments are already converted to UTF-8 (WinMain function).
    // On other platforms, the Fl::args_to_utf8 function doesn’t do anything:
    // Fl::args_to_utf8(argc, argv);

    int i = 0;
    Fl::args(argc, argv, i, args_handler);
    fl_open_callback(file_load);
    _app_window->show(argc, argv);
    return Fl::run();
}
