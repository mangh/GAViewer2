//
// Text editor widgets: main editor, split editor, and the shared text buffer.
//
#include "text_editor.h"
#include "file_handling.h"
#include "main_window.h"
#include "menu.h"

#include <FL/Fl.H>

Fl_Text_Editor *_editor = NULL;
Fl_Text_Editor *_split_editor = NULL;
Fl_Text_Buffer *_text_buffer = NULL;
Fl_Tile *_tile = NULL;

static void text_changed_callback(int, int n_inserted, int n_deleted, int, const char *, void *)
{
    if (n_inserted || n_deleted)
        file_changed(true);
}

void build_main_editor()
{
    _app_window->begin();
    _text_buffer = new Fl_Text_Buffer();
    _text_buffer->add_modify_callback(text_changed_callback, NULL);
    _editor = new Fl_Text_Editor(0, _menu_bar->h(), _app_window->w(), _app_window->h() - _menu_bar->h());
    _editor->buffer(_text_buffer);
    _editor->textfont(FL_COURIER);
    _app_window->resizable(_editor);
    _app_window->end();
}

void build_split_editor()
{
    _app_window->begin();
    _tile = new Fl_Tile(_editor->x(), _editor->y(), _editor->w(), _editor->h());
    _app_window->remove(_editor);
    _tile->add(_editor);
    _split_editor = new Fl_Text_Editor(_tile->x(), _tile->y() + _tile->h(), _tile->w(), 0);
    _split_editor->buffer(_text_buffer);
    _split_editor->textfont(FL_COURIER);
    _split_editor->hide();
    _tile->end();
    _tile->size_range(0, 25, 25);
    _tile->size_range(1, 25, 25);
    _tile->init_sizes();
    _app_window->end();
    _app_window->resizable(_tile);
    _tile->resizable(_editor);
}

void edit_undo_callback(Fl_Widget *, void *)
{
    Fl_Widget *e = Fl::focus();
    if (e && (e == _editor || e == _split_editor))
        Fl_Text_Editor::kf_undo(0, (Fl_Text_Editor *)e);
}

void edit_redo_callback(Fl_Widget *, void *)
{
    Fl_Widget *e = Fl::focus();
    if (e && (e == _editor || e == _split_editor))
        Fl_Text_Editor::kf_redo(0, (Fl_Text_Editor *)e);
}

void edit_cut_callback(Fl_Widget *, void *)
{
    Fl_Widget *e = Fl::focus();
    if (e && (e == _editor || e == _split_editor))
        Fl_Text_Editor::kf_cut(0, (Fl_Text_Editor *)e);
}

void edit_copy_callback(Fl_Widget *, void *)
{
    Fl_Widget *e = Fl::focus();
    if (e && (e == _editor || e == _split_editor))
        Fl_Text_Editor::kf_copy(0, (Fl_Text_Editor *)e);
}

void edit_paste_callback(Fl_Widget *, void *)
{
    Fl_Widget *e = Fl::focus();
    if (e && (e == _editor || e == _split_editor))
        Fl_Text_Editor::kf_paste(0, (Fl_Text_Editor *)e);
}

void edit_delete_callback(Fl_Widget *, void *)
{
    Fl_Widget *e = Fl::focus();
    if (e && (e == _editor || e == _split_editor))
        Fl_Text_Editor::kf_delete(0, (Fl_Text_Editor *)e);
}

void view_split_callback(Fl_Widget *w, void *)
{
    Fl_Menu_Bar *menu = static_cast<Fl_Menu_Bar *>(w);
    const Fl_Menu_Item *splitview_item = menu->mvalue();
    if (splitview_item->value())
    {
        int h_split = _tile->h() / 2;
        _editor->size(_tile->w(), h_split);
        _split_editor->resize(_tile->x(), _tile->y() + h_split, _tile->w(), _tile->h() - h_split);
        _split_editor->show();
    }
    else
    {
        _editor->size(_tile->w(), _tile->h());
        _split_editor->resize(_tile->x(), _tile->y() + _tile->h(), _tile->w(), 0);
        _split_editor->hide();
    }
    _tile->resizable(_editor);
    _tile->init_sizes();
    _tile->redraw();
}
