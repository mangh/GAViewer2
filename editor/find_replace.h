//
// Find and replace: search helpers and the Find/Replace dialog.
//
#ifndef FIND_REPLACE_H
#define FIND_REPLACE_H

#include <FL/Fl_Widget.H>

extern char last_find_text[1024];
extern char last_replace_text[1024];

bool find_next(const char *needle);
void replace_selection(const char *new_text);

void menu_find_callback(Fl_Widget *, void *);
void menu_find_next_callback(Fl_Widget *, void *);
void menu_replace_callback(Fl_Widget *, void *);
void menu_replace_next_callback(Fl_Widget *, void *);

#endif // FIND_REPLACE_H
