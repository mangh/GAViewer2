//
// Text editor widgets: main editor, split editor, and the shared text buffer.
//
#include "text_editor.h"
#include "file_handling.h"
#include "main_window.h"
#include "menu.h"

#include <FL/Fl.H>

Fl_Text_Editor *app_editor = NULL;
Fl_Text_Editor *app_split_editor = NULL;
Fl_Text_Buffer *app_text_buffer = NULL;
Fl_Tile *app_tile = NULL;

static void text_changed_callback(int, int n_inserted, int n_deleted, int, const char *, void *)
{
    if (n_inserted || n_deleted)
        set_changed(true);
}

void build_main_editor()
{
    app_window->begin();
    app_text_buffer = new Fl_Text_Buffer();
    app_text_buffer->add_modify_callback(text_changed_callback, NULL);
    app_editor = new Fl_Text_Editor(0, app_menu_bar->h(), app_window->w(), app_window->h() - app_menu_bar->h());
    app_editor->buffer(app_text_buffer);
    app_editor->textfont(FL_COURIER);
    app_window->resizable(app_editor);
    app_window->end();
}

void build_split_editor()
{
    app_window->begin();
    app_tile = new Fl_Tile(app_editor->x(), app_editor->y(), app_editor->w(), app_editor->h());
    app_window->remove(app_editor);
    app_tile->add(app_editor);
    app_split_editor = new Fl_Text_Editor(app_tile->x(), app_tile->y() + app_tile->h(), app_tile->w(), 0);
    app_split_editor->buffer(app_text_buffer);
    app_split_editor->textfont(FL_COURIER);
    app_split_editor->hide();
    app_tile->end();
    app_tile->size_range(0, 25, 25);
    app_tile->size_range(1, 25, 25);
    app_tile->init_sizes();
    app_window->end();
    app_window->resizable(app_tile);
    app_tile->resizable(app_editor);
}

void edit_undo_callback(Fl_Widget *, void *)
{
    Fl_Widget *e = Fl::focus();
    if (e && (e == app_editor || e == app_split_editor))
        Fl_Text_Editor::kf_undo(0, (Fl_Text_Editor *)e);
}

void edit_redo_callback(Fl_Widget *, void *)
{
    Fl_Widget *e = Fl::focus();
    if (e && (e == app_editor || e == app_split_editor))
        Fl_Text_Editor::kf_redo(0, (Fl_Text_Editor *)e);
}

void edit_cut_callback(Fl_Widget *, void *)
{
    Fl_Widget *e = Fl::focus();
    if (e && (e == app_editor || e == app_split_editor))
        Fl_Text_Editor::kf_cut(0, (Fl_Text_Editor *)e);
}

void edit_copy_callback(Fl_Widget *, void *)
{
    Fl_Widget *e = Fl::focus();
    if (e && (e == app_editor || e == app_split_editor))
        Fl_Text_Editor::kf_copy(0, (Fl_Text_Editor *)e);
}

void edit_paste_callback(Fl_Widget *, void *)
{
    Fl_Widget *e = Fl::focus();
    if (e && (e == app_editor || e == app_split_editor))
        Fl_Text_Editor::kf_paste(0, (Fl_Text_Editor *)e);
}

void edit_delete_callback(Fl_Widget *, void *)
{
    Fl_Widget *e = Fl::focus();
    if (e && (e == app_editor || e == app_split_editor))
        Fl_Text_Editor::kf_delete(0, (Fl_Text_Editor *)e);
}

void view_split_callback(Fl_Widget *w, void *)
{
    Fl_Menu_Bar *menu = static_cast<Fl_Menu_Bar *>(w);
    const Fl_Menu_Item *splitview_item = menu->mvalue();
    if (splitview_item->value())
    {
        int h_split = app_tile->h() / 2;
        app_editor->size(app_tile->w(), h_split);
        app_split_editor->resize(app_tile->x(), app_tile->y() + h_split, app_tile->w(), app_tile->h() - h_split);
        app_split_editor->show();
    }
    else
    {
        app_editor->size(app_tile->w(), app_tile->h());
        app_split_editor->resize(app_tile->x(), app_tile->y() + app_tile->h(), app_tile->w(), 0);
        app_split_editor->hide();
    }
    app_tile->resizable(app_editor);
    app_tile->init_sizes();
    app_tile->redraw();
}
