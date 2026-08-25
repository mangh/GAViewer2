//
// Text editor widgets: main editor, split editor, and the shared text buffer.
//
#ifndef TEXT_EDITOR_H
#define TEXT_EDITOR_H

#include <FL/Fl_Text_Editor.H>
#include <FL/Fl_Tile.H>

extern Fl_Text_Editor *_editor;
extern Fl_Text_Editor *_split_editor;
extern Fl_Text_Buffer *_text_buffer;
extern Fl_Tile *_tile;

void build_main_editor();
void build_split_editor();

void edit_undo_callback(Fl_Widget *, void *);
void edit_redo_callback(Fl_Widget *, void *);
void edit_cut_callback(Fl_Widget *, void *);
void edit_copy_callback(Fl_Widget *, void *);
void edit_paste_callback(Fl_Widget *, void *);
void edit_delete_callback(Fl_Widget *, void *);
void view_split_callback(Fl_Widget *, void *);

#endif // TEXT_EDITOR_H
