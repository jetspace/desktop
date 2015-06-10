/*
This file is licensed under the MIT-License
Copyright (c) 2015 Marius Messerschmidt
For more details view file 'LICENSE'
*/
#include <gtk/gtk.h>
#include <gtksourceview/gtksource.h>



GtkWidget *win;
GtkWidget *source;
char *cmd;
char *filename;
gboolean modified = FALSE;

//Callbacks
#include "editor_callbacks.h"

int main(int argc, char **argv)
{
    cmd = argv[0];
    gtk_init(&argc, &argv);

    win = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_resize(GTK_WINDOW(win), 500, 400);
    gtk_window_set_title(GTK_WINDOW(win), "SIDE Editor");
    gtk_container_set_border_width(GTK_CONTAINER(win), 10);

    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_container_add(GTK_CONTAINER(win), box);

    GtkWidget *scroll = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

    source = gtk_source_view_new();
    gtk_container_add(GTK_CONTAINER(scroll), source);
    gtk_box_pack_end(GTK_BOX(box), scroll, TRUE, TRUE, 0);

    //create menu bar
    GtkWidget *menubar = gtk_menu_bar_new();

    //FILE
    GtkWidget *filemenu   = gtk_menu_new();
    GtkWidget *file       = gtk_menu_item_new_with_label("File");
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(file), filemenu);
        GtkWidget *new   = gtk_menu_item_new_with_label("New");
        gtk_menu_shell_append(GTK_MENU_SHELL(filemenu), new);
        GtkWidget *open   = gtk_menu_item_new_with_label("Open");
        gtk_menu_shell_append(GTK_MENU_SHELL(filemenu), open);
        GtkWidget *save   = gtk_menu_item_new_with_label("Save");
        gtk_menu_shell_append(GTK_MENU_SHELL(filemenu), save);
        GtkWidget *saveas = gtk_menu_item_new_with_label("Save As");
        gtk_menu_shell_append(GTK_MENU_SHELL(filemenu), saveas);
        GtkWidget *quit   = gtk_menu_item_new_with_label("Quit");
        gtk_menu_shell_append(GTK_MENU_SHELL(filemenu), quit);
    gtk_menu_shell_append(GTK_MENU_SHELL(menubar), file);
    //ABOUT
    GtkWidget *aboutmenu   = gtk_menu_new();
    GtkWidget *about       = gtk_menu_item_new_with_label("About");
        gtk_menu_item_set_submenu(GTK_MENU_ITEM(about), aboutmenu);
        GtkWidget *ab     = gtk_menu_item_new_with_label("About");
        gtk_menu_shell_append(GTK_MENU_SHELL(aboutmenu), ab);
        GtkWidget *help   = gtk_menu_item_new_with_label("Help");
        gtk_menu_shell_append(GTK_MENU_SHELL(aboutmenu), help);
    gtk_menu_shell_append(GTK_MENU_SHELL(menubar), about);

    gtk_box_pack_start(GTK_BOX(box), menubar, FALSE, FALSE, 0);


    //keyboard
    GtkAccelGroup *accel_group = gtk_accel_group_new();
    gtk_window_add_accel_group (GTK_WINDOW (win), accel_group);

    gtk_widget_add_accelerator (save, "activate", accel_group, gdk_unicode_to_keyval('s'), GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
    gtk_widget_add_accelerator (saveas, "activate", accel_group, gdk_unicode_to_keyval('s'), GDK_CONTROL_MASK | GDK_SHIFT_MASK, GTK_ACCEL_VISIBLE);
    gtk_widget_add_accelerator (open, "activate", accel_group, gdk_unicode_to_keyval('o'), GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);




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

    g_signal_connect(G_OBJECT(gtk_text_view_get_buffer(GTK_TEXT_VIEW(source))), "changed", G_CALLBACK(update_modify), NULL);

    gtk_widget_show_all(win);
    gtk_main();
}
