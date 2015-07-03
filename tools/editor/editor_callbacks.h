/*
This file is licensed under the MIT-License
Copyright (c) 2015 Marius Messerschmidt
For more details view file 'LICENSE'
*/

#include <string.h>
#include <gdk/gdkkeysyms.h>
#include <stdlib.h>
#include <ctype.h>
#include "../../shared/info.h"

void destroy(GtkWidget *w, GdkEvent *e, gpointer p)
{
    gtk_main_quit();
}
gboolean data_protect(GtkWidget *w, GdkEvent *e, gpointer p);
void quit_cb(GtkWidget *w, GdkEvent *e, gpointer p)
{
    if(!data_protect(win,e,p))
        gtk_widget_destroy(win);
}

gboolean save_win_data(GtkWidget *widget,GdkEvent *event, gpointer user_data);

gboolean data_protect(GtkWidget *w, GdkEvent *e, gpointer p)
{
    save_win_data(w,e,p);   //fetches window size before end
    g_settings_sync();      //sync all unsafed items
    if(modified)
        {
            GtkWidget *dialog = gtk_message_dialog_new (GTK_WINDOW(w),
                                 GTK_DIALOG_MODAL,
                                 GTK_MESSAGE_ERROR,
                                 GTK_BUTTONS_YES_NO,
                                 _("Warning, if you continue, your unsaved work is gone! Would you like to continue?"));
            int x = gtk_dialog_run(GTK_DIALOG(dialog));
            gtk_widget_destroy(dialog);

            if(x == GTK_RESPONSE_YES)
                return FALSE;
        }
    else
        {
            return FALSE;
        }

    return TRUE;
}


gboolean show_about(GtkWidget *w, GdkEvent *e, gpointer p)
{
    //for any reason, dialog fails, if modified is set to true...
    gboolean old_modify = modified;
    modified = FALSE;

    GtkWidget *dialog = gtk_about_dialog_new();
    gtk_about_dialog_set_program_name(GTK_ABOUT_DIALOG(dialog), "SIDE Editor");
    gtk_about_dialog_set_version(GTK_ABOUT_DIALOG(dialog), VERSION);
    gtk_about_dialog_set_website(GTK_ABOUT_DIALOG(dialog), "http://jetspace.tk");
    gtk_about_dialog_set_artists(GTK_ABOUT_DIALOG(dialog), ARTISTS);
    gtk_about_dialog_set_copyright(GTK_ABOUT_DIALOG(dialog), COPYRIGHT);
    gtk_about_dialog_set_comments(GTK_ABOUT_DIALOG(dialog), EDITOR_DISCRIPTION);
    gtk_about_dialog_set_license_type(GTK_ABOUT_DIALOG(dialog), GTK_LICENSE_MIT_X11);
    gtk_about_dialog_set_authors(GTK_ABOUT_DIALOG(dialog), AUTHORS);
    gtk_about_dialog_set_logo(GTK_ABOUT_DIALOG(dialog), gdk_pixbuf_scale_simple(gdk_pixbuf_new_from_file("/usr/share/icons/jetspace/JetSpace.png", NULL), 100, 100, GDK_INTERP_BILINEAR));
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
    modified = old_modify;
    return FALSE;
}

gboolean show_help(GtkTextBuffer *buffer)
{
    gtk_show_uri(gdk_screen_get_default (), "http://side.rtfd.org", GDK_CURRENT_TIME, NULL);
    return FALSE;
}

gboolean buffer_is_emptey(GtkTextBuffer *buffer)
{
    GtkTextIter start;
    GtkTextIter end;

    gtk_text_buffer_get_start_iter(buffer, &start);
    gtk_text_buffer_get_end_iter(buffer, &end);

    return !strlen(gtk_text_buffer_get_text(buffer, &start, &end, FALSE)) > 0;
}

void append_text(GtkTextBuffer *buffer, char *text)
{
    GtkTextIter end;
    gtk_text_buffer_get_end_iter(buffer, &end);
    gtk_text_buffer_insert (buffer, &end, text, -1);

}

gboolean new_file(GtkWidget *w, GdkEvent *e, gpointer data)
{
    //call the same command as this instance was called (argv[0])
    char *exec = g_strdup_printf("%s &", cmd);
    system(exec);
    free(exec);
}

void fetch_file(void)
{
    printf(_("Opening %s\n (as arg)"), filename);
    FILE *file = fopen(filename, "r");
    if(file == NULL)
        {
            g_warning("Accessing file failed! (%s)", filename);
            return;
        }
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(source));
    char buff[2000];
    while(fgets(buff, 2000, file) != NULL)
        append_text(buffer, buff);
    fclose(file);
    gtk_window_set_title(GTK_WINDOW(win), g_strdup_printf("SiDE Editor - %s", filename));
    modified = FALSE;
}

gboolean open_file(GtkWidget *w, GdkEvent *e, gpointer p)
{
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(source));
    GtkSourceBuffer *sbuffer = GTK_SOURCE_BUFFER(gtk_text_view_get_buffer(GTK_TEXT_VIEW(source)));
    if(!buffer_is_emptey(buffer) && modified)
        {
            GtkWidget *dialog = gtk_message_dialog_new (GTK_WINDOW(gtk_widget_get_parent(gtk_widget_get_parent(gtk_widget_get_parent(w)))),
                                 GTK_DIALOG_MODAL,
                                 GTK_MESSAGE_ERROR,
                                 GTK_BUTTONS_YES_NO,
                                 _("Warning, if you continue, your unsaved work is gone! Would you like to continue?"));
            int x = gtk_dialog_run(GTK_DIALOG(dialog));
            gtk_widget_destroy(dialog);

            if(x != GTK_RESPONSE_YES)
                return FALSE;
        }
    GtkWidget *dialog = gtk_file_chooser_dialog_new(_("Open File"), GTK_WINDOW(gtk_widget_get_parent(gtk_widget_get_parent(gtk_widget_get_parent(w)))), GTK_FILE_CHOOSER_ACTION_OPEN, "Cancel", GTK_RESPONSE_CANCEL, "Open", GTK_RESPONSE_ACCEPT, NULL);
    int result = gtk_dialog_run(GTK_DIALOG(dialog));
    if(result == GTK_RESPONSE_ACCEPT)
        {
            GtkTextIter start, end;
            gtk_source_buffer_begin_not_undoable_action(sbuffer);
            gtk_text_buffer_get_end_iter(buffer, &end);
            gtk_text_buffer_get_start_iter(buffer, &start);
            gtk_text_buffer_delete(buffer, &start, &end);
            filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
            gtk_window_set_title(GTK_WINDOW(win), g_strdup_printf("SiDE Editor - %s", filename));
            printf(_("Opening %s\n"), filename);

            FILE *file = fopen(filename, "r");
            if(file == NULL)
                {
                    gtk_source_buffer_end_not_undoable_action(sbuffer);
                    g_warning("Accessing file failed! (%s)", filename);
                    return FALSE;
                }
            char buff[2000];
            while(fgets(buff, 2000, file) != NULL)
                append_text(buffer, buff);
            fclose(file);
            gtk_source_buffer_end_not_undoable_action(sbuffer);
        }
    gtk_widget_destroy(dialog);
    modified = FALSE;
    return FALSE;
}
gboolean save_file_as(GtkWidget *w, GdkEvent *e, gpointer p);
gboolean save_file(GtkWidget *w, GdkEvent *e, gpointer p)
{
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(source));
    GtkTextIter start;
    GtkTextIter end;
    gtk_text_buffer_get_start_iter(buffer, &start);
    gtk_text_buffer_get_end_iter(buffer, &end);

    if(filename == NULL || filename == untitled)
        {
            save_file_as(w,e,p);
            return FALSE;
        }
    gtk_window_set_title(GTK_WINDOW(win), g_strdup_printf("SiDE Editor - %s", filename));
    FILE *output = fopen(filename, "w");
    fprintf(output, "%s", gtk_text_buffer_get_text(buffer, &start, &end, TRUE));
    modified = FALSE;
    return FALSE;
}

gboolean save_file_as(GtkWidget *w, GdkEvent *e, gpointer p)
{
    GtkWidget *dialog = gtk_file_chooser_dialog_new(_("Save File"), GTK_WINDOW(gtk_widget_get_parent(gtk_widget_get_parent(gtk_widget_get_parent(w)))), GTK_FILE_CHOOSER_ACTION_SAVE, "Cancel", GTK_RESPONSE_CANCEL, "Save", GTK_RESPONSE_ACCEPT, NULL);
    int result = gtk_dialog_run(GTK_DIALOG(dialog));
    if(result == GTK_RESPONSE_ACCEPT)
        {
            filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
            save_file(w,e,p);
        }
    gtk_widget_destroy(dialog);
    return FALSE;
}

void update_modify(GtkTextView *t, gchar *p,gpointer data)
{
    modified = TRUE;
    gtk_window_set_title(GTK_WINDOW(win), g_strdup_printf("SiDE Editor - %s*", filename));
}
GSettings *stats;
gboolean save_win_data(GtkWidget *widget,GdkEvent *event, gpointer user_data)
{
    if(!stats)
        stats = g_settings_new("org.jetspace.desktop.editor");
    int width, height, ypos, xpos;
    gtk_window_get_size(GTK_WINDOW(win), &width, &height);
    gtk_window_get_position(GTK_WINDOW(win), &xpos, &ypos);
    g_settings_set_value(stats, "width", g_variant_new_int32(width));
    g_settings_set_value(stats, "height", g_variant_new_int32(height));
    g_settings_set_value(stats, "xpos", g_variant_new_int32(xpos));
    g_settings_set_value(stats, "ypos", g_variant_new_int32(ypos));
    return FALSE;
}

GtkWidget *pos_switch, *num_switch, *high_switch, *font_button;
gboolean write_settings(GtkWidget *w, GdkEvent *e, gpointer p);
gboolean show_settings(GtkWidget *w, GdkEvent *e, gpointer p)
{
    GtkWidget *swin = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(swin), _("SiDE Editor - Settings"));
    gtk_container_set_border_width(GTK_CONTAINER(swin), 10);
    gtk_window_resize(GTK_WINDOW(swin), 425, 400);
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    GtkWidget *apply = gtk_button_new_with_label(_("Apply"));
    GtkWidget *notebook = gtk_notebook_new();
    GtkWidget *appearance = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), appearance, gtk_label_new(_("Appearance")));

    GSettings *conf = g_settings_new("org.jetspace.desktop.editor");

    GtkWidget *pos_label = gtk_label_new(_("Restore window position:"));
    pos_switch = gtk_switch_new();
    gtk_switch_set_active(GTK_SWITCH(pos_switch), g_variant_get_boolean(g_settings_get_value(conf, "savepos")));
    GtkWidget *pos_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_container_add(GTK_CONTAINER(pos_box), pos_label);
    gtk_box_pack_end(GTK_BOX(pos_box), pos_switch, FALSE, TRUE, 0);

    GtkWidget *num_label = gtk_label_new(_("Enable line numbers:"));
    num_switch = gtk_switch_new();
    gtk_switch_set_active(GTK_SWITCH(num_switch), g_variant_get_boolean(g_settings_get_value(conf, "linenumbers")));
    GtkWidget *num_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_container_add(GTK_CONTAINER(num_box), num_label);
    gtk_box_pack_end(GTK_BOX(num_box), num_switch, FALSE, TRUE, 0);

    GtkWidget *high_label = gtk_label_new(_("Highlight current line:"));
    high_switch = gtk_switch_new();
    gtk_switch_set_active(GTK_SWITCH(high_switch), g_variant_get_boolean(g_settings_get_value(conf, "linehighlight")));
    GtkWidget *high_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_container_add(GTK_CONTAINER(high_box), high_label);
    gtk_box_pack_end(GTK_BOX(high_box), high_switch, FALSE, TRUE, 0);

    GtkWidget *font_label = gtk_label_new(_("Font:"));
    font_button = gtk_font_button_new_with_font(g_variant_get_string(g_settings_get_value(conf, "font"), FALSE));
    GtkWidget *font_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_container_add(GTK_CONTAINER(font_box), font_label);
    gtk_box_pack_end(GTK_BOX(font_box), font_button, FALSE, TRUE, 0);


    gtk_container_add(GTK_CONTAINER(appearance), pos_box);
    gtk_container_add(GTK_CONTAINER(appearance), num_box);
    gtk_container_add(GTK_CONTAINER(appearance), high_box);
    gtk_container_add(GTK_CONTAINER(appearance), font_box);

    g_signal_connect(G_OBJECT(apply), "clicked", G_CALLBACK(write_settings), NULL);

    gtk_container_add(GTK_CONTAINER(box), notebook);
    gtk_box_pack_end(GTK_BOX(box), apply, FALSE, TRUE, 0);
    gtk_container_add(GTK_CONTAINER(swin), box);
    gtk_widget_show_all(swin);
    return FALSE;
}

gboolean write_settings(GtkWidget *w, GdkEvent *e, gpointer p)
{
    GSettings *conf = g_settings_new("org.jetspace.desktop.editor");
    g_settings_set_value(conf, "savepos", g_variant_new_boolean(gtk_switch_get_active(GTK_SWITCH(pos_switch))));
    g_settings_set_value(conf, "linenumbers", g_variant_new_boolean(gtk_switch_get_active(GTK_SWITCH(num_switch))));
    g_settings_set_value(conf, "linehighlight", g_variant_new_boolean(gtk_switch_get_active(GTK_SWITCH(high_switch))));
    g_settings_set_value(conf, "font", g_variant_new_string(gtk_font_chooser_get_font(GTK_FONT_CHOOSER(font_button))));
    return FALSE;
}

gboolean update_source(GtkWidget *w, GdkEvent *e, gpointer p)
{
    GSettings *win_data = g_settings_new("org.jetspace.desktop.editor");
    gtk_source_view_set_show_line_numbers(GTK_SOURCE_VIEW(source), g_variant_get_boolean(g_settings_get_value(win_data, "linenumbers")));
    gtk_source_view_set_highlight_current_line(GTK_SOURCE_VIEW(source), g_variant_get_boolean(g_settings_get_value(win_data, "linehighlight")));
    PangoFontDescription *font = pango_font_description_from_string(g_variant_get_string(g_settings_get_value(win_data, "font"), FALSE));
    gtk_widget_override_font(GTK_WIDGET(source), font);
    return FALSE;
}

gboolean do_undo(GtkWidget *w, GdkEvent *e, gpointer p)
{
  GtkSourceBuffer *sbuffer = GTK_SOURCE_BUFFER(gtk_text_view_get_buffer(GTK_TEXT_VIEW(source)));
  if(gtk_source_buffer_can_undo(sbuffer))
    gtk_source_buffer_undo(sbuffer);
}

gboolean do_redo(GtkWidget *w, GdkEvent *e, gpointer p)
{
  GtkSourceBuffer *sbuffer = GTK_SOURCE_BUFFER(gtk_text_view_get_buffer(GTK_TEXT_VIEW(source)));
  if(gtk_source_buffer_can_redo(sbuffer))
    gtk_source_buffer_redo(sbuffer);
}
