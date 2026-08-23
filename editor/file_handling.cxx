//
// File handling and document state (current filename, changed flag).
//
#include "file_handling.h"
#include "main_window.h"
#include "text_editor.h"

#include <FL/Fl.H>
#include <FL/Fl_Native_File_Chooser.H>
#include <FL/fl_ask.H>
#include <FL/fl_string_functions.h>
#include <FL/platform.H>

#include <errno.h>
#include <string.h>

bool text_changed = false;
char app_filename[FL_PATH_MAX] = "";

void set_changed(bool v)
{
    if (v != text_changed)
    {
        text_changed = v;
        update_title();
    }
}

void set_filename(const char *new_filename)
{
    if (new_filename)
    {
        fl_strlcpy(app_filename, new_filename, FL_PATH_MAX);
    }
    else
    {
        app_filename[0] = 0;
    }
    update_title();
}

void load(const char *filename)
{
    if (app_text_buffer->loadfile(filename) == 0)
    {
        set_filename(filename);
        set_changed(false);
    }
    else
    {
        fl_alert("Failed to load file\n%s\n%s", filename, strerror(errno));
    }
}

void menu_new_callback(Fl_Widget *, void *)
{
    if (text_changed)
    {
        int c = fl_choice("Changes in your text have not been saved.\n"
                          "Do you want to start a new text anyway?",
                          "New", "Cancel", NULL);
        if (c == 1)
            return;
    }
    app_text_buffer->text("");
    set_filename(NULL);
    set_changed(false);
}

void menu_save_as_callback(Fl_Widget *, void *)
{
    Fl_Native_File_Chooser file_chooser;
    file_chooser.title("Save File As...");
    file_chooser.type(Fl_Native_File_Chooser::BROWSE_SAVE_FILE);
    if (app_filename[0])
    {
        char temp_filename[FL_PATH_MAX];
        fl_strlcpy(temp_filename, app_filename, FL_PATH_MAX);
        const char *name = fl_filename_name(temp_filename);
        if (name)
        {
            file_chooser.preset_file(name);
            temp_filename[name - temp_filename] = 0;
            file_chooser.directory(temp_filename);
        }
    }
    if (file_chooser.show() == 0)
    {
        if (app_text_buffer->savefile(file_chooser.filename()) == 0)
        {
            set_filename(file_chooser.filename());
            set_changed(false);
        }
        else
        {
            fl_alert("Failed to save file\n%s\n%s", file_chooser.filename(), strerror(errno));
        }
    }
}

void menu_save_callback(Fl_Widget *, void *)
{
    if (!app_filename[0])
    {
        menu_save_as_callback(NULL, NULL);
    }
    else
    {
        if (app_text_buffer->savefile(app_filename) == 0)
        {
            set_changed(false);
        }
        else
        {
            fl_alert("Failed to save file\n%s\n%s", app_filename, strerror(errno));
        }
    }
}

void menu_quit_callback(Fl_Widget *, void *)
{
    if (text_changed)
    {
        int r = fl_choice("The current file has not been saved.\n"
                          "Would you like to save it now?",
                          "Cancel", "Save", "Don't Save");
        if (r == 0) // cancel
            return;
        if (r == 1)
        { // save
            menu_save_callback(NULL, NULL);
            return;
        }
    }
    Fl::hide_all_windows();
}

void menu_open_callback(Fl_Widget *, void *)
{
    if (text_changed)
    {
        int r = fl_choice("The current file has not been saved.\n"
                          "Would you like to save it now?",
                          "Cancel", "Save", "Don't Save");
        if (r == 0) // cancel
            return;
        if (r == 1) // save
            menu_save_callback(NULL, NULL);
    }
    Fl_Native_File_Chooser file_chooser;
    file_chooser.title("Open File...");
    file_chooser.type(Fl_Native_File_Chooser::BROWSE_FILE);
    if (app_filename[0])
    {
        char temp_filename[FL_PATH_MAX];
        fl_strlcpy(temp_filename, app_filename, FL_PATH_MAX);
        const char *name = fl_filename_name(temp_filename);
        if (name)
        {
            file_chooser.preset_file(name);
            temp_filename[name - temp_filename] = 0;
            file_chooser.directory(temp_filename);
        }
    }
    if (file_chooser.show() == 0)
        load(file_chooser.filename());
}

static int args_handler(int argc, char **argv, int &i)
{
    if (argv && argv[i] && argv[i][0] != '-')
    {
        load(argv[i]);
        i++;
        return 1;
    }
    return 0;
}

int handle_commandline_and_run(int &argc, char **argv)
{
    int i = 0;
    Fl::args_to_utf8(argc, argv);
    Fl::args(argc, argv, i, args_handler);
    fl_open_callback(load);
    app_window->show(argc, argv);
    return Fl::run();
}
