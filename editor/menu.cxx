//
// Application menu bar.
//
#include "menu.h"
#include "editor_features.h"
#include "file_handling.h"
#include "find_replace.h"
#include "main_window.h"
#include "text_editor.h"

Fl_Menu_Bar *app_menu_bar = NULL;

void build_app_menu_bar()
{
    app_window->begin();
    app_menu_bar = new Fl_Menu_Bar(0, 0, app_window->w(), 25);
    app_menu_bar->add("File/New", FL_COMMAND + 'n', file_new_callback);
    app_menu_bar->add("File/Open", FL_COMMAND + 'o', file_open_callback, NULL, FL_MENU_DIVIDER);
    app_menu_bar->add("File/Save", FL_COMMAND + 's', file_save_callback);
    app_menu_bar->add("File/Save as...", FL_COMMAND + 'S', file_save_as_callback, NULL, FL_MENU_DIVIDER);
    app_menu_bar->add("File/Quit Editor", FL_COMMAND | 'q', file_quit_callback);
    app_menu_bar->add("Edit/Undo", FL_COMMAND + 'z', edit_undo_callback);
    app_menu_bar->add("Edit/Redo", FL_COMMAND + 'Z', edit_redo_callback, NULL, FL_MENU_DIVIDER);
    app_menu_bar->add("Edit/Cut", FL_COMMAND + 'x', edit_cut_callback);
    app_menu_bar->add("Edit/Copy", FL_COMMAND + 'c', edit_copy_callback);
    app_menu_bar->add("Edit/Paste", FL_COMMAND + 'v', edit_paste_callback);
    app_menu_bar->add("Edit/Delete", 0, edit_delete_callback);
    app_menu_bar->add("Find/Find...", FL_COMMAND + 'f', find_callback);
    app_menu_bar->add("Find/Find Next", FL_COMMAND + 'g', find_next_callback, NULL, FL_MENU_DIVIDER);
    app_menu_bar->add("Find/Replace...", FL_COMMAND + 'r', replace_callback);
    app_menu_bar->add("Find/Replace Next", FL_COMMAND + 't', replace_next_callback);
    app_menu_bar->add("View/Line Numbers", FL_COMMAND + 'l', view_linenumbers_callback, NULL, FL_MENU_TOGGLE);
    app_menu_bar->add("View/Word Wrap", 0, view_wordwrap_callback, NULL, FL_MENU_TOGGLE);
    app_menu_bar->add("View/Split", FL_COMMAND + 'i', view_split_callback, NULL, FL_MENU_TOGGLE);
    app_menu_bar->add("View/Syntax Highlighting", 0, view_syntaxhighlight_callback, NULL, FL_MENU_TOGGLE);
    app_window->callback(file_quit_callback);
    app_window->end();
}
