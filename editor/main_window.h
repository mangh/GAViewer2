//
// Main application window.
//
#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <FL/Fl_Double_Window.H>

extern Fl_Double_Window *_app_window;

void build_app_window();
void update_app_title(const char *info);
int app_run(int &argc, char **argv);

#endif // MAIN_WINDOW_H
