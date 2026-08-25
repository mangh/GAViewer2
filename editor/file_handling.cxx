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

bool _changed = false;
char _path[FL_PATH_MAX] = "";
char _precis[FL_PATH_MAX + 3];

const char *file_precis()
{
    if (_path[0])
    {
        _precis[FL_PATH_MAX + 2] = '\0'; // ensure that the buffer is always terminated
        snprintf(_precis, FL_PATH_MAX + 2, _changed ? "%s *" : "%s", fl_filename_name(_path));
        return _precis;
    }
    else
    {
        return NULL;
    }
}

void file_changed(bool v)
{
    if (v != _changed)
    {
        _changed = v;
        update_app_title(file_precis());
    }
}

void file_path(const char *new_path)
{
    if (new_path)
    {
        fl_strlcpy(_path, new_path, FL_PATH_MAX);
    }
    else
    {
        _path[0] = 0;
    }
}

void file_load(const char *filename)
{
    if (_text_buffer->loadfile(filename) == 0)
    {
        file_path(filename);
        file_changed(false);
    }
    else
    {
        update_app_title(NULL);
        fl_alert("Failed to file_load file\n%s\n%s", filename, strerror(errno));
    }
}

void file_new_callback(Fl_Widget *, void *)
{
    if (_changed)
    {
        int c = fl_choice("Changes in your text have not been saved.\n"
                          "Do you want to start a new text anyway?",
                          "New", "Cancel", NULL);
        if (c == 1)
            return;
    }
    _text_buffer->text("");
    file_path(NULL);
    file_changed(false);
}

void file_save_as_callback(Fl_Widget *, void *)
{
    Fl_Native_File_Chooser file_chooser;
    file_chooser.title("Save File As...");
    file_chooser.type(Fl_Native_File_Chooser::BROWSE_SAVE_FILE);
    if (_path[0])
    {
        char temp_path[FL_PATH_MAX];
        fl_strlcpy(temp_path, _path, FL_PATH_MAX);
        const char *name = fl_filename_name(temp_path);
        if (name)
        {
            file_chooser.preset_file(name);
            temp_path[name - temp_path] = 0;
            file_chooser.directory(temp_path);
        }
    }
    if (file_chooser.show() == 0)
    {
        if (_text_buffer->savefile(file_chooser.filename()) == 0)
        {
            file_path(file_chooser.filename());
            file_changed(false);
        }
        else
        {
            fl_alert("Failed to save file\n%s\n%s", file_chooser.filename(), strerror(errno));
        }
    }
}

void file_save_callback(Fl_Widget *, void *)
{
    if (!_path[0])
    {
        file_save_as_callback(NULL, NULL);
    }
    else
    {
        if (_text_buffer->savefile(_path) == 0)
        {
            file_changed(false);
        }
        else
        {
            fl_alert("Failed to save file\n%s\n%s", _path, strerror(errno));
        }
    }
}

void file_quit_callback(Fl_Widget *, void *)
{
    if (_changed)
    {
        int r = fl_choice("The current file has not been saved.\n"
                          "Would you like to save it now?",
                          "Cancel", "Save", "Don't Save");
        if (r == 0) // cancel
            return;
        if (r == 1)
        { // save
            file_save_callback(NULL, NULL);
            return;
        }
    }
    Fl::hide_all_windows();
}

void file_open_callback(Fl_Widget *, void *)
{
    if (_changed)
    {
        int r = fl_choice("The current file has not been saved.\n"
                          "Would you like to save it now?",
                          "Cancel", "Save", "Don't Save");
        if (r == 0) // cancel
            return;
        if (r == 1) // save
            file_save_callback(NULL, NULL);
    }
    Fl_Native_File_Chooser file_chooser;
    file_chooser.title("Open File...");
    file_chooser.type(Fl_Native_File_Chooser::BROWSE_FILE);
    if (_path[0])
    {
        char temp_path[FL_PATH_MAX];
        fl_strlcpy(temp_path, _path, FL_PATH_MAX);
        const char *name = fl_filename_name(temp_path);
        if (name)
        {
            file_chooser.preset_file(name);
            temp_path[name - temp_path] = 0;
            file_chooser.directory(temp_path);
        }
    }
    if (file_chooser.show() == 0)
        file_load(file_chooser.filename());
}
