#include <gtk/gtk.h>

GtkWidget *win, *pic;
char *filename;
#include "view_callbacks.h"
int main(int argc, char **argv)
{
    gtk_init(&argc, &argv);

    if(argc > 1)
        filename = argv[1];

    win = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_resize(GTK_WINDOW(win), 500, 350);
    gtk_window_set_title(GTK_WINDOW(win), "SIDE Picture Viewer");
    gtk_container_set_border_width(GTK_CONTAINER(win), 10);

    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 2);
    gtk_container_add(GTK_CONTAINER(win), box);

    //create menu bar
    GtkWidget *menubar = gtk_menu_bar_new();

    //FILE
    GtkWidget *filemenu   = gtk_menu_new();
    GtkWidget *file       = gtk_menu_item_new_with_label("File");
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(file), filemenu);
        GtkWidget *open   = gtk_menu_item_new_with_label("Open");
        gtk_menu_shell_append(GTK_MENU_SHELL(filemenu), open);
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




    GtkWidget *scroll = gtk_scrolled_window_new(NULL, NULL);
    pic = gtk_image_new();
    if(argc > 1)
        {
            gtk_image_set_from_file(GTK_IMAGE(pic), filename);
            gtk_window_set_title(GTK_WINDOW(win), g_strdup_printf("SiDE Picture Viewer - %s", filename));
        }

    gtk_container_add(GTK_CONTAINER(scroll), pic);
    gtk_box_pack_end(GTK_BOX(box), scroll, TRUE, TRUE, 0);

    //keyboard
    GtkAccelGroup *accel_group = gtk_accel_group_new();
    gtk_window_add_accel_group (GTK_WINDOW (win), accel_group);

    gtk_widget_add_accelerator (open, "activate", accel_group, gdk_unicode_to_keyval('o'), GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
    gtk_widget_add_accelerator (quit, "activate", accel_group, gdk_unicode_to_keyval('q'), GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);


    //signals
    g_signal_connect(G_OBJECT(win), "destroy", G_CALLBACK(destroy), NULL);
    g_signal_connect(G_OBJECT(quit), "activate", G_CALLBACK(destroy), NULL);
    g_signal_connect(G_OBJECT(ab), "activate", G_CALLBACK(show_about), NULL);
    g_signal_connect(G_OBJECT(help), "activate", G_CALLBACK(show_help), NULL);
    g_signal_connect(G_OBJECT(open), "activate", G_CALLBACK(open_file), NULL);


    gtk_widget_show_all(win);
    gtk_main();
    return 0;
}
