/*
This file is licensed under the MIT-License
Copyright (c) 2015 Marius Messerschmidt
For more details view file 'LICENSE'
*/

#include <gtk/gtk.h>
#include <stdlib.h>
#include "../shared/info.h"

gboolean destroy(GtkWidget *w, GdkEvent *e, gpointer *p)
{
  gtk_main_quit();
  return FALSE;
}

gboolean wallpaper_settings(GtkWidget *w, GdkEvent *e, gpointer p)
{
  system("side-wallpaper-settings");
}

gboolean panel_settings(GtkWidget *w, GdkEvent *e, gpointer p)
{
  system("side-panel-settings");
}

gboolean gtk_settings(GtkWidget *w, GdkEvent *e, gpointer p)
{
  system("side-gtk-settings");
}

gboolean about_d(GtkWidget *w, GdkEvent *e, gpointer p)
{
  GtkWidget *dialog = gtk_about_dialog_new();
  gtk_about_dialog_set_program_name(GTK_ABOUT_DIALOG(dialog), "SIDE");
  gtk_about_dialog_set_version(GTK_ABOUT_DIALOG(dialog), VERSION);
  gtk_about_dialog_set_copyright(GTK_ABOUT_DIALOG(dialog), COPYRIGHT);
  gtk_about_dialog_set_comments(GTK_ABOUT_DIALOG(dialog), DISCRIPTION);
  gtk_about_dialog_set_license_type(GTK_ABOUT_DIALOG(dialog), GTK_LICENSE_MIT_X11);
  gtk_about_dialog_set_authors(GTK_ABOUT_DIALOG(dialog), AUTHORS);
  gtk_about_dialog_set_logo(GTK_ABOUT_DIALOG(dialog), gdk_pixbuf_scale_simple(gdk_pixbuf_new_from_file("/usr/share/icons/jetspace/JetSpace.png", NULL), 100, 100, GDK_INTERP_BILINEAR));
  gtk_dialog_run(GTK_DIALOG(dialog));
  gtk_widget_destroy(dialog);

}

int main(int argc, char **argv)
{

  gtk_init(&argc, &argv);

  GtkWidget *win, *wallpaper, *panel, *about, *box, *label_a, *label_i, *gtk;

  win = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_resize(GTK_WINDOW(win), 300, 200);
  gtk_window_set_title(GTK_WINDOW(win), "SIDE Settings");
  gtk_container_set_border_width(GTK_CONTAINER(win), 10);
  g_signal_connect(G_OBJECT(win), "delete-event", G_CALLBACK(destroy), NULL);

  box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);

  label_a = gtk_label_new("Appearance:");
  label_i = gtk_label_new("Informations:");

  wallpaper = gtk_button_new_with_label("Change Wallpaper");
  g_signal_connect(G_OBJECT(wallpaper), "button_press_event", G_CALLBACK(wallpaper_settings), NULL);

  panel = gtk_button_new_with_label("Configure Panel");
  g_signal_connect(G_OBJECT(panel), "button_press_event", G_CALLBACK(panel_settings), NULL);

  gtk = gtk_button_new_with_label("Setup GTK");
  g_signal_connect(G_OBJECT(gtk), "button_press_event", G_CALLBACK(gtk_settings), NULL);


  about = gtk_button_new_with_label("About SIDE");
  g_signal_connect(G_OBJECT(about), "button_press_event", G_CALLBACK(about_d), NULL);



  gtk_container_add(GTK_CONTAINER(box), label_a);
  gtk_container_add(GTK_CONTAINER(box), wallpaper);
  gtk_container_add(GTK_CONTAINER(box), panel);
  gtk_container_add(GTK_CONTAINER(box), gtk);
  gtk_container_add(GTK_CONTAINER(box), label_i);
  gtk_container_add(GTK_CONTAINER(box), about);

  gtk_container_add(GTK_CONTAINER(win), box);
  gtk_widget_show_all(win);

  gtk_main();

}
