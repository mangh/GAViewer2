//
// A simple text editor program for the Fast Light Tool Kit (FLTK).
//
// This program is described in chapter "Designing a Simple Text Editor"
// of the FLTK Programmer's Guide.
//
// Copyright 1998-2026 by Bill Spitzak and others.
//
// This library is free software. Distribution and use rights are outlined in
// the file "COPYING" which should have been included with this file.  If this
// file is missing or damaged, see the license at:
//
//     https://www.fltk.org/COPYING.php
//
// Please see the following page on how to report bugs and issues:
//
//     https://www.fltk.org/bugs.php
//

#include "main_window.h"
#include "menu.h"
#include "text_editor.h"

int main(int argc, char **argv)
{
    build_app_window();
    build_app_menu_bar();
    build_main_editor();
    build_split_editor();
    // handle command line arguments and run the application
    return app_run(argc, argv);
}
