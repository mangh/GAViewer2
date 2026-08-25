//
// Find and replace: search helpers and the Find/Replace dialog.
//
#ifndef FIND_REPLACE_H
#define FIND_REPLACE_H

#include <FL/Fl_Widget.H>

bool find_next(const char *needle);
void replace_selection(const char *new_text);

void find_callback(Fl_Widget *, void *);
void find_next_callback(Fl_Widget *, void *);
void replace_callback(Fl_Widget *, void *);
void replace_next_callback(Fl_Widget *, void *);

#endif // FIND_REPLACE_H
