/*
This file is licensed under the MIT-License
Copyright (c) 2015 Marius Messerschmidt
For more details view file 'LICENSE'
*/
#include <gtk/gtk.h>
#include <gtksourceview/gtksource.h>
#include <glib/gi18n.h>


GtkWidget *win;
GtkWidget *source;
char *cmd;
char untitled[] = "untitled";
char *filename = untitled;
gboolean modified = FALSE;

//Callbacks
#include "editor_callbacks.h"

int main(int argc, char **argv)
{

    textdomain("side");

    cmd = argv[0];
    gtk_init(&argc, &argv);
    GSettings *win_data = g_settings_new("org.jetspace.desktop.editor");
    win = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_resize(GTK_WINDOW(win), g_variant_get_int32(g_settings_get_value(win_data, "width")), g_variant_get_int32(g_settings_get_value(win_data, "height")));
    int x, y;
    x = g_variant_get_int32(g_settings_get_value(win_data, "xpos"));
    y = g_variant_get_int32(g_settings_get_value(win_data, "ypos"));
    if(x >= 0 && y >= 0 && g_variant_get_boolean(g_settings_get_value(win_data, "savepos")))
        gtk_window_move(GTK_WINDOW(win), x, y);
    gtk_window_set_title(GTK_WINDOW(win), _("SiDE Editor"));
    gtk_container_set_border_width(GTK_CONTAINER(win), 10);

    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_container_add(GTK_CONTAINER(win), box);

    GtkWidget *scroll = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

    source = gtk_source_view_new();
    gtk_source_view_set_show_line_numbers(GTK_SOURCE_VIEW(source), g_variant_get_boolean(g_settings_get_value(win_data, "linenumbers")));
    gtk_source_view_set_highlight_current_line(GTK_SOURCE_VIEW(source), g_variant_get_boolean(g_settings_get_value(win_data, "linehighlight")));
    PangoFontDescription *font = pango_font_description_from_string(g_variant_get_string(g_settings_get_value(win_data, "font"), FALSE));
    gtk_widget_override_font(GTK_WIDGET(source), font);
    gtk_container_add(GTK_CONTAINER(scroll), source);
    gtk_box_pack_end(GTK_BOX(box), scroll, TRUE, TRUE, 0);

    //create menu bar
    GtkWidget *menubar = gtk_menu_bar_new();

    //FILE
    GtkWidget *filemenu   = gtk_menu_new();
    GtkWidget *file       = gtk_menu_item_new_with_label(_("File"));
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(file), filemenu);
        GtkWidget *new   = gtk_menu_item_new_with_label(_("New"));
        gtk_menu_shell_append(GTK_MENU_SHELL(filemenu), new);
        GtkWidget *open   = gtk_menu_item_new_with_label(_("Open"));
        gtk_menu_shell_append(GTK_MENU_SHELL(filemenu), open);
        GtkWidget *save   = gtk_menu_item_new_with_label(_("Save"));
        gtk_menu_shell_append(GTK_MENU_SHELL(filemenu), save);
        GtkWidget *saveas = gtk_menu_item_new_with_label(_("Save As"));
        gtk_menu_shell_append(GTK_MENU_SHELL(filemenu), saveas);
        GtkWidget *quit   = gtk_menu_item_new_with_label(_("Quit"));
        gtk_menu_shell_append(GTK_MENU_SHELL(filemenu), quit);
    gtk_menu_shell_append(GTK_MENU_SHELL(menubar), file);
    //EDIT
    GtkWidget *editmenu         = gtk_menu_new();
    GtkWidget *edit             = gtk_menu_item_new_with_label(_("Edit"));
        gtk_menu_item_set_submenu(GTK_MENU_ITEM(edit), editmenu);
        GtkWidget *undo         = gtk_menu_item_new_with_label(_("Undo"));
        gtk_menu_shell_append(GTK_MENU_SHELL(editmenu), undo);
        GtkWidget *redo         = gtk_menu_item_new_with_label(_("Redo"));
        gtk_menu_shell_append(GTK_MENU_SHELL(editmenu), redo);
        GtkWidget *settings     = gtk_menu_item_new_with_label(_("Settings"));
        gtk_menu_shell_append(GTK_MENU_SHELL(editmenu), settings);
    gtk_menu_shell_append(GTK_MENU_SHELL(menubar), edit);
    //ABOUT
    GtkWidget *aboutmenu   = gtk_menu_new();
    GtkWidget *about       = gtk_menu_item_new_with_label(_("About"));
        gtk_menu_item_set_submenu(GTK_MENU_ITEM(about), aboutmenu);
        GtkWidget *ab     = gtk_menu_item_new_with_label(_("About"));
        gtk_menu_shell_append(GTK_MENU_SHELL(aboutmenu), ab);
        GtkWidget *help   = gtk_menu_item_new_with_label(_("Help"));
        gtk_menu_shell_append(GTK_MENU_SHELL(aboutmenu), help);
    gtk_menu_shell_append(GTK_MENU_SHELL(menubar), about);



    gtk_box_pack_start(GTK_BOX(box), menubar, FALSE, FALSE, 0);


    //keyboard
    GtkAccelGroup *accel_group = gtk_accel_group_new();
    gtk_window_add_accel_group (GTK_WINDOW (win), accel_group);

    //FILE
    gtk_widget_add_accelerator (save, "activate", accel_group, gdk_unicode_to_keyval('s'), GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
    gtk_widget_add_accelerator (saveas, "activate", accel_group, gdk_unicode_to_keyval('s'), GDK_CONTROL_MASK | GDK_SHIFT_MASK, GTK_ACCEL_VISIBLE);
    gtk_widget_add_accelerator (open, "activate", accel_group, gdk_unicode_to_keyval('o'), GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
    gtk_widget_add_accelerator (new, "activate", accel_group, gdk_unicode_to_keyval('n'), GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
    gtk_widget_add_accelerator (quit, "activate", accel_group, gdk_unicode_to_keyval('q'), GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);

    //EDIT
    gtk_widget_add_accelerator (settings, "activate", accel_group, gdk_unicode_to_keyval(','), GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
    gtk_widget_add_accelerator (undo, "activate", accel_group, gdk_unicode_to_keyval('z'), GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
    gtk_widget_add_accelerator (redo, "activate", accel_group, gdk_unicode_to_keyval('y'), GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);

    //ABOUT
    gtk_widget_add_accelerator (help, "activate", accel_group, gdk_keyval_from_name("F1"), GDK_SHIFT_MASK, GTK_ACCEL_VISIBLE);



    //signals
    g_signal_connect(G_OBJECT(win), "destroy", G_CALLBACK(destroy), NULL);
    g_signal_connect(G_OBJECT(win), "delete-event", G_CALLBACK(data_protect), NULL);
    g_signal_connect(G_OBJECT(quit), "activate", G_CALLBACK(quit_cb), NULL);
    g_signal_connect(G_OBJECT(ab), "activate", G_CALLBACK(show_about), NULL);
    g_signal_connect(G_OBJECT(help), "activate", G_CALLBACK(show_help), NULL);
    g_signal_connect(G_OBJECT(new), "activate", G_CALLBACK(new_file), NULL);
    g_signal_connect(G_OBJECT(open), "activate", G_CALLBACK(open_file), NULL);
    g_signal_connect(G_OBJECT(save), "activate", G_CALLBACK(save_file), NULL);
    g_signal_connect(G_OBJECT(saveas), "activate", G_CALLBACK(save_file_as), NULL);
    g_signal_connect(G_OBJECT(settings), "activate", G_CALLBACK(show_settings), NULL);
    g_signal_connect(G_OBJECT(undo), "activate", G_CALLBACK(do_undo), NULL);
    g_signal_connect(G_OBJECT(redo), "activate", G_CALLBACK(do_redo), NULL);

    g_signal_connect(G_OBJECT(win), "window-state-event", G_CALLBACK(save_win_data), NULL);
    g_signal_connect(G_OBJECT(win_data), "changed", G_CALLBACK(update_source), NULL);

    g_signal_connect(G_OBJECT(gtk_text_view_get_buffer(GTK_TEXT_VIEW(source))), "changed", G_CALLBACK(update_modify), NULL);

    //check for command line
    if(argc > 1)
        {
            filename = argv[1];
            fetch_file();
        }


    gtk_widget_show_all(win);
    gtk_main();
}
