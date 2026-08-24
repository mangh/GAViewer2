//
// Find and replace: search helpers and the Find/Replace dialog.
//
#include "find_replace.h"
#include "text_editor.h"

#include <FL/Fl.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Flex.H>
#include <FL/Fl_Input.H>
#include <FL/fl_ask.H>
#include <FL/fl_string_functions.h>

#include <string.h>

char last_find_text[1024] = "";
char last_replace_text[1024] = "";

bool find_next(const char *needle)
{
    Fl_Text_Editor *editor = app_editor;
    Fl_Widget *e = Fl::focus();
    if (e && e == app_split_editor)
        editor = app_split_editor;
    int pos = editor->insert_position();
    int found = app_text_buffer->search_forward(pos, needle, &pos);
    if (found)
    {
        app_text_buffer->select(pos, pos + (int)strlen(needle));
        editor->insert_position(pos + (int)strlen(needle));
        editor->show_insert_position();
        return true;
    }
    else
    {
        fl_alert("No further occurrences of '%s' found!", needle);
        return false;
    }
}

void replace_selection(const char *new_text)
{
    Fl_Text_Editor *editor = app_editor;
    Fl_Widget *e = Fl::focus();
    if (e && e == app_split_editor)
        editor = app_split_editor;
    int start, end;
    if (app_text_buffer->selection_position(&start, &end))
    {
        app_text_buffer->remove_selection();
        app_text_buffer->insert(start, new_text);
        app_text_buffer->select(start, start + (int)strlen(new_text));
        editor->insert_position(start + (int)strlen(new_text));
        editor->show_insert_position();
    }
}

void find_callback(Fl_Widget *, void *)
{
    const char *find_text = fl_input("Find in text:", last_find_text);
    if (find_text)
    {
        fl_strlcpy(last_find_text, find_text, sizeof(last_find_text));
        find_next(find_text);
    }
}

void find_next_callback(Fl_Widget *, void *)
{
    if (last_find_text[0])
    {
        find_next(last_find_text);
    }
    else
    {
        find_callback(NULL, NULL);
    }
}

class Replace_Dialog : public Fl_Double_Window
{
    Fl_Input *find_text_input;
    Fl_Input *replace_text_input;
    Fl_Button *find_next_button;
    Fl_Button *replace_and_find_button;
    Fl_Button *close_button;

  public:
    Replace_Dialog(const char *label);
    void show() FL_OVERRIDE;

  private:
    static void find_next_callback(Fl_Widget *, void *);
    static void replace_and_find_callback(Fl_Widget *, void *);
    static void close_callback(Fl_Widget *, void *);
};

static Replace_Dialog *replace_dialog = NULL;

Replace_Dialog::Replace_Dialog(const char *label) : Fl_Double_Window(430, 110, label)
{
    find_text_input = new Fl_Input(100, 10, 320, 25, "Find:");
    replace_text_input = new Fl_Input(100, 40, 320, 25, "Replace:");
    Fl_Flex *button_field = new Fl_Flex(100, 70, w() - 100, 40);
    button_field->type(Fl_Flex::HORIZONTAL);
    button_field->margin(0, 5, 10, 10);
    button_field->gap(10);
    find_next_button = new Fl_Button(0, 0, 0, 0, "Next");
    find_next_button->callback(find_next_callback, this);
    replace_and_find_button = new Fl_Button(0, 0, 0, 0, "Replace");
    replace_and_find_button->callback(replace_and_find_callback, this);
    close_button = new Fl_Button(0, 0, 0, 0, "Close");
    close_button->callback(close_callback, this);
    button_field->end();
    set_non_modal();
}

void Replace_Dialog::show()
{
    find_text_input->value(last_find_text);
    replace_text_input->value(last_replace_text);
    Fl_Double_Window::show();
}

void Replace_Dialog::find_next_callback(Fl_Widget *, void *my_dialog)
{
    Replace_Dialog *dlg = static_cast<Replace_Dialog *>(my_dialog);
    fl_strlcpy(last_find_text, dlg->find_text_input->value(), sizeof(last_find_text));
    fl_strlcpy(last_replace_text, dlg->replace_text_input->value(), sizeof(last_replace_text));
    if (last_find_text[0])
        find_next(last_find_text);
}

void Replace_Dialog::replace_and_find_callback(Fl_Widget *, void *my_dialog)
{
    Replace_Dialog *dlg = static_cast<Replace_Dialog *>(my_dialog);
    replace_selection(dlg->replace_text_input->value());
    find_next_callback(NULL, my_dialog);
}

void Replace_Dialog::close_callback(Fl_Widget *, void *my_dialog)
{
    Replace_Dialog *dlg = static_cast<Replace_Dialog *>(my_dialog);
    dlg->hide();
}

void replace_callback(Fl_Widget *, void *)
{
    if (!replace_dialog)
        replace_dialog = new Replace_Dialog("Find and Replace");
    replace_dialog->show();
}

void replace_next_callback(Fl_Widget *, void *)
{
    if (!last_find_text[0])
    {
        replace_callback(NULL, NULL);
    }
    else
    {
        replace_selection(last_replace_text);
        find_next(last_find_text);
    }
}
