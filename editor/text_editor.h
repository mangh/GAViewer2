//
// Text editor widgets: main editor, split editor, and the shared text buffer.
//
#ifndef TEXT_EDITOR_H
#define TEXT_EDITOR_H

#include <FL/Fl_Text_Editor.H>
#include <FL/Fl_Tile.H>

extern Fl_Text_Editor *app_editor;
extern Fl_Text_Editor *app_split_editor;
extern Fl_Text_Buffer *app_text_buffer;
extern Fl_Tile *app_tile;

void build_main_editor();
void build_split_editor();

void menu_undo_callback(Fl_Widget *, void *);
void menu_redo_callback(Fl_Widget *, void *);
void menu_cut_callback(Fl_Widget *, void *);
void menu_copy_callback(Fl_Widget *, void *);
void menu_paste_callback(Fl_Widget *, void *);
void menu_delete_callback(Fl_Widget *, void *);
void menu_split_callback(Fl_Widget *, void *);

#endif // TEXT_EDITOR_H
