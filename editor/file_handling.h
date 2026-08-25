//
// File handling and document state (current filename, changed flag).
//
#ifndef FILE_HANDLING_H
#define FILE_HANDLING_H

#include <FL/Fl_Widget.H>
#include <FL/filename.H>

void file_changed(bool v);
void file_path(const char *new_filename);
void file_load(const char *filename);

void file_new_callback(Fl_Widget *, void *);
void file_open_callback(Fl_Widget *, void *);
void file_save_callback(Fl_Widget *, void *);
void file_save_as_callback(Fl_Widget *, void *);
void file_quit_callback(Fl_Widget *, void *);

#endif // FILE_HANDLING_H
