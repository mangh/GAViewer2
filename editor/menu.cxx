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
    app_menu_bar->add("File/New", FL_COMMAND + 'n', menu_new_callback);
    app_menu_bar->add("File/Open", FL_COMMAND + 'o', menu_open_callback, NULL, FL_MENU_DIVIDER);
    app_menu_bar->add("File/Save", FL_COMMAND + 's', menu_save_callback);
    app_menu_bar->add("File/Save as...", FL_COMMAND + 'S', menu_save_as_callback, NULL, FL_MENU_DIVIDER);
    app_menu_bar->add("File/Quit Editor", FL_COMMAND | 'q', menu_quit_callback);
    app_menu_bar->add("Edit/Undo", FL_COMMAND + 'z', menu_undo_callback);
    app_menu_bar->add("Edit/Redo", FL_COMMAND + 'Z', menu_redo_callback, NULL, FL_MENU_DIVIDER);
    app_menu_bar->add("Edit/Cut", FL_COMMAND + 'x', menu_cut_callback);
    app_menu_bar->add("Edit/Copy", FL_COMMAND + 'c', menu_copy_callback);
    app_menu_bar->add("Edit/Paste", FL_COMMAND + 'v', menu_paste_callback);
    app_menu_bar->add("Edit/Delete", 0, menu_delete_callback);
    app_menu_bar->add("Find/Find...", FL_COMMAND + 'f', menu_find_callback);
    app_menu_bar->add("Find/Find Next", FL_COMMAND + 'g', menu_find_next_callback, NULL, FL_MENU_DIVIDER);
    app_menu_bar->add("Find/Replace...", FL_COMMAND + 'r', menu_replace_callback);
    app_menu_bar->add("Find/Replace Next", FL_COMMAND + 't', menu_replace_next_callback);
    app_menu_bar->add("Window/Line Numbers", FL_COMMAND + 'l', menu_linenumbers_callback, NULL, FL_MENU_TOGGLE);
    app_menu_bar->add("Window/Word Wrap", 0, menu_wordwrap_callback, NULL, FL_MENU_TOGGLE);
    app_menu_bar->add("Window/Split", FL_COMMAND + 'i', menu_split_callback, NULL, FL_MENU_TOGGLE);
    app_menu_bar->add("Window/Syntax Highlighting", 0, menu_syntaxhighlight_callback, NULL, FL_MENU_TOGGLE);
    app_window->callback(menu_quit_callback);
    app_window->end();
}
