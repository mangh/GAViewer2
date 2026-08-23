//
// File handling and document state (current filename, changed flag).
//
#ifndef FILE_HANDLING_H
#define FILE_HANDLING_H

#include <FL/Fl_Widget.H>
#include <FL/filename.H>

extern bool text_changed;
extern char app_filename[FL_PATH_MAX];

void set_changed(bool v);
void set_filename(const char *new_filename);
void load(const char *filename);

void menu_new_callback(Fl_Widget *, void *);
void menu_open_callback(Fl_Widget *, void *);
void menu_save_callback(Fl_Widget *, void *);
void menu_save_as_callback(Fl_Widget *, void *);
void menu_quit_callback(Fl_Widget *, void *);

int handle_commandline_and_run(int &argc, char **argv);

#endif // FILE_HANDLING_H
