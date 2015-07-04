void destroy(GtkWidget *w, GdkEvent *e, gpointer p)
{
    gtk_main_quit();
}

#include "../../shared/info.h"
gboolean show_about(GtkWidget *w, GdkEvent *e, gpointer p)
{
    GtkWidget *dialog = gtk_about_dialog_new();
    gtk_about_dialog_set_program_name(GTK_ABOUT_DIALOG(dialog), _("SiDE Picture Viewer"));
    gtk_about_dialog_set_version(GTK_ABOUT_DIALOG(dialog), VERSION);
    gtk_about_dialog_set_copyright(GTK_ABOUT_DIALOG(dialog), COPYRIGHT);
    gtk_about_dialog_set_comments(GTK_ABOUT_DIALOG(dialog), PICTURE_VIEWER_DISCRIPTION);
    gtk_about_dialog_set_license_type(GTK_ABOUT_DIALOG(dialog), GTK_LICENSE_MIT_X11);
    gtk_about_dialog_set_authors(GTK_ABOUT_DIALOG(dialog), AUTHORS);
    gtk_about_dialog_set_logo(GTK_ABOUT_DIALOG(dialog), gdk_pixbuf_scale_simple(gdk_pixbuf_new_from_file("/usr/share/icons/jetspace/JetSpace.png", NULL), 100, 100, GDK_INTERP_BILINEAR));
    gtk_about_dialog_set_website(GTK_ABOUT_DIALOG(dialog), "http://jetspace.tk");
    gtk_about_dialog_set_artists(GTK_ABOUT_DIALOG(dialog), ARTISTS);
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
    return FALSE;
}

gboolean show_help(GtkWidget *w, GdkEvent *e, gpointer p)
{
    gtk_show_uri(gdk_screen_get_default (), "http://side.rtfd.org", GDK_CURRENT_TIME, NULL);
    return FALSE;
}

gboolean open_file(GtkWidget *w, GdkEvent *e, gpointer p)
{
    GtkWidget *dialog = gtk_file_chooser_dialog_new(_("Open File"), GTK_WINDOW(win), GTK_FILE_CHOOSER_ACTION_OPEN, _("Cancel"), GTK_RESPONSE_CANCEL, _("Open"), GTK_RESPONSE_ACCEPT, NULL);
    GtkFileFilter *filter = gtk_file_filter_new();
    gtk_file_filter_add_pixbuf_formats (filter);
    gtk_file_filter_set_name(filter, _("Supported Images"));
	gtk_file_chooser_add_filter (GTK_FILE_CHOOSER (dialog), filter);

    int result = gtk_dialog_run(GTK_DIALOG(dialog));
    if(result == GTK_RESPONSE_ACCEPT)
        {
            filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
            gtk_window_set_title(GTK_WINDOW(win), g_strdup_printf(_("SiDE Picture Viewer - %s"), filename));
            printf(_("Opening %s\n"), filename);
            gtk_image_set_from_file(GTK_IMAGE(pic), filename);
        }
        gtk_widget_destroy(dialog);

    return FALSE;
}
