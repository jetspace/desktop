#include <gtk/gtk.h>
#include <glib/gi18n.h>

GtkWidget *win, *pic;
char *filename = NULL;
GdkPixbuf *origin;
GtkWidget *scroll;
GtkWidget *status;
float scale = 1;
#include "view_callbacks.h"
int main(int argc, char **argv)
{

    textdomain("side");

    gtk_init(&argc, &argv);

    if(argc > 1)
        filename = argv[1];

    win = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_resize(GTK_WINDOW(win), 500, 350);
    gtk_window_set_title(GTK_WINDOW(win), _("SiDE Picture Viewer"));
    gtk_container_set_border_width(GTK_CONTAINER(win), 10);

    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 2);
    gtk_container_add(GTK_CONTAINER(win), box);

    //create menu bar
    GtkWidget *menubar = gtk_menu_bar_new();

    //FILE
    GtkWidget *filemenu   = gtk_menu_new();
    GtkWidget *file       = gtk_menu_item_new_with_label(_("File"));
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(file), filemenu);
        GtkWidget *open   = gtk_menu_item_new_with_label(_("Open"));
        gtk_menu_shell_append(GTK_MENU_SHELL(filemenu), open);
        GtkWidget *quit   = gtk_menu_item_new_with_label(_("Quit"));
        gtk_menu_shell_append(GTK_MENU_SHELL(filemenu), quit);
    gtk_menu_shell_append(GTK_MENU_SHELL(menubar), file);
    //VIEW
    GtkWidget *viewmenu   = gtk_menu_new();
    GtkWidget *view       = gtk_menu_item_new_with_label(_("View"));
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(view), viewmenu);
        GtkWidget *zoom_in   = gtk_menu_item_new_with_label(_("Zoom in"));
        gtk_menu_shell_append(GTK_MENU_SHELL(viewmenu), zoom_in);
        GtkWidget *zoom_out  = gtk_menu_item_new_with_label(_("Zoom out"));
        gtk_menu_shell_append(GTK_MENU_SHELL(viewmenu), zoom_out);
        GtkWidget *reset_zoom  = gtk_menu_item_new_with_label(_("Original Size"));
        gtk_menu_shell_append(GTK_MENU_SHELL(viewmenu), reset_zoom);
        GtkWidget *fit_zoom  = gtk_menu_item_new_with_label(_("Fit to Window"));
        gtk_menu_shell_append(GTK_MENU_SHELL(viewmenu), fit_zoom);
    gtk_menu_shell_append(GTK_MENU_SHELL(menubar), view);
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
    scroll = gtk_scrolled_window_new(NULL, NULL);
    pic = gtk_image_new();
    if(argc > 1)
        {
            gtk_image_set_from_file(GTK_IMAGE(pic), filename);
            origin = gtk_image_get_pixbuf(GTK_IMAGE(pic));
            gtk_window_set_title(GTK_WINDOW(win), g_strdup_printf(_("SiDE Picture Viewer - %s"), filename));
            set_status();
        }

    gtk_container_add(GTK_CONTAINER(scroll), pic);


    //add labels
    status = gtk_label_new(_("No File"));
    gtk_box_pack_end(GTK_BOX(box), status, FALSE, FALSE, 0);
    gtk_box_pack_end(GTK_BOX(box), scroll, TRUE, TRUE, 0);

    //keyboard
    GtkAccelGroup *accel_group = gtk_accel_group_new();
    gtk_window_add_accel_group (GTK_WINDOW (win), accel_group);

    //FILE
    gtk_widget_add_accelerator (open, "activate", accel_group, gdk_unicode_to_keyval('o'), GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
    gtk_widget_add_accelerator (quit, "activate", accel_group, gdk_unicode_to_keyval('q'), GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);

    //VIEW
    gtk_widget_add_accelerator (zoom_in, "activate", accel_group, gdk_unicode_to_keyval('+'), GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
    gtk_widget_add_accelerator (zoom_out, "activate", accel_group, gdk_unicode_to_keyval('-'), GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
    gtk_widget_add_accelerator (reset_zoom, "activate", accel_group, gdk_unicode_to_keyval('0'), GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
    gtk_widget_add_accelerator (fit_zoom, "activate", accel_group, gdk_unicode_to_keyval('f'), GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);

    //ABOUT
    gtk_widget_add_accelerator (help, "activate", accel_group, gdk_keyval_from_name("F1"), GDK_SHIFT_MASK, GTK_ACCEL_VISIBLE);


    //signals
    g_signal_connect(G_OBJECT(win), "destroy", G_CALLBACK(destroy), NULL);
    g_signal_connect(G_OBJECT(quit), "activate", G_CALLBACK(destroy), NULL);
    g_signal_connect(G_OBJECT(ab), "activate", G_CALLBACK(show_about), NULL);
    g_signal_connect(G_OBJECT(help), "activate", G_CALLBACK(show_help), NULL);
    g_signal_connect(G_OBJECT(open), "activate", G_CALLBACK(open_file), NULL);
    g_signal_connect(G_OBJECT(zoom_in), "activate", G_CALLBACK(scale_in), NULL);
    g_signal_connect(G_OBJECT(zoom_out), "activate", G_CALLBACK(scale_out), NULL);
    g_signal_connect(G_OBJECT(reset_zoom), "activate", G_CALLBACK(scale_reset), NULL);
    g_signal_connect(G_OBJECT(fit_zoom), "activate", G_CALLBACK(scale_fit), NULL);


    gtk_widget_show_all(win);
    gtk_main();
    return 0;
}
