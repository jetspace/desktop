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

gboolean data_protect(GtkWidget *w, GdkEvent *e, gpointer p)
{
    if(modified)
        {
            GtkWidget *dialog = gtk_message_dialog_new (GTK_WINDOW(w),
                                 GTK_DIALOG_MODAL,
                                 GTK_MESSAGE_ERROR,
                                 GTK_BUTTONS_YES_NO,
                                 "Warning, if you continue, your unsaved work is gone! Would you like to continue?");
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
    GtkWidget *dialog = gtk_about_dialog_new();
    gtk_about_dialog_set_program_name(GTK_ABOUT_DIALOG(dialog), "SIDE Editor");
    gtk_about_dialog_set_version(GTK_ABOUT_DIALOG(dialog), VERSION);
    gtk_about_dialog_set_copyright(GTK_ABOUT_DIALOG(dialog), COPYRIGHT);
    gtk_about_dialog_set_comments(GTK_ABOUT_DIALOG(dialog), EDITOR_DISCRIPTION);
    gtk_about_dialog_set_license_type(GTK_ABOUT_DIALOG(dialog), GTK_LICENSE_MIT_X11);
    gtk_about_dialog_set_authors(GTK_ABOUT_DIALOG(dialog), AUTHORS);
    gtk_about_dialog_set_logo(GTK_ABOUT_DIALOG(dialog), gdk_pixbuf_scale_simple(gdk_pixbuf_new_from_file("/usr/share/icons/jetspace/JetSpace.png", NULL), 100, 100, GDK_INTERP_BILINEAR));
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
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
    char *exec = g_strdup_printf("%s &", cmd);
    system(exec);
    free(exec);
}

gboolean open_file(GtkWidget *w, GdkEvent *e, gpointer p)
{
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(source));
    if(!buffer_is_emptey(buffer) && modified)
        {
            GtkWidget *dialog = gtk_message_dialog_new (GTK_WINDOW(gtk_widget_get_parent(gtk_widget_get_parent(gtk_widget_get_parent(w)))),
                                 GTK_DIALOG_MODAL,
                                 GTK_MESSAGE_ERROR,
                                 GTK_BUTTONS_YES_NO,
                                 "Warning, if you continue, your unsaved work is gone! Would you like to continue?");
            int x = gtk_dialog_run(GTK_DIALOG(dialog));
            gtk_widget_destroy(dialog);

            if(x != GTK_RESPONSE_YES)
                return FALSE;
        }
    GtkWidget *dialog = gtk_file_chooser_dialog_new("Open File", GTK_WINDOW(gtk_widget_get_parent(gtk_widget_get_parent(gtk_widget_get_parent(w)))), GTK_FILE_CHOOSER_ACTION_OPEN, "Cancel", GTK_RESPONSE_CANCEL, "Open", GTK_RESPONSE_ACCEPT, NULL);
    int result = gtk_dialog_run(GTK_DIALOG(dialog));
    if(result == GTK_RESPONSE_ACCEPT)
        {
            filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
            gtk_window_set_title(GTK_WINDOW(win), g_strdup_printf("SiDE Editor - %s", filename));
            printf("Opening %s\n", filename);

            FILE *file = fopen(filename, "r");
            if(file == NULL)
                {
                    g_warning("Accessing file failed! (%s)", filename);
                    return FALSE;
                }
            char buff[2000];
            while(fgets(buff, 2000, file) != NULL)
                append_text(buffer, buff);
            fclose(file);
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

    if(filename == NULL)
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
    GtkWidget *dialog = gtk_file_chooser_dialog_new("Save File", GTK_WINDOW(gtk_widget_get_parent(gtk_widget_get_parent(gtk_widget_get_parent(w)))), GTK_FILE_CHOOSER_ACTION_SAVE, "Cancel", GTK_RESPONSE_CANCEL, "Save", GTK_RESPONSE_ACCEPT, NULL);
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
