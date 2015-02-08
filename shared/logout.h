/*
This file is licensed under the MIT-License
Copyright (c) 2015 Marius Messerschmidt
For more details view file 'LICENSE'
*/

#ifndef _LOGOUT_H
#define _LOGOUT_H
#include <gtk/gtk.h>

gboolean destroy(GtkWidget *w, GdkEvent *e, gpointer *p)
{
  gtk_widget_destroy(w);
  return FALSE;
}

gboolean shutdown_f(GtkWidget *w, GdkEvent *e, gpointer *p)
{
  system("shutdown -h now");
  return FALSE;
}

gboolean reboot_f(GtkWidget *w, GdkEvent *e, gpointer *p)
{
  system("shutdown -r now");
  return FALSE;
}

gboolean logout_f(GtkWidget *w, GdkEvent *e, gpointer *p)
{
  //not implemented yet
  gtk_main_quit();
  return FALSE;
}

void logout(void)
{
  GtkWidget *win, *shutdown, *reboot, *logout, *label, *box, *button_box;

  win = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_resize(GTK_WINDOW(win), 500, 200);
  gtk_window_set_title(GTK_WINDOW(win), "SIDE Logout");
  gtk_container_set_border_width(GTK_CONTAINER(win), 15);
  g_signal_connect(G_OBJECT(win), "delete-event", G_CALLBACK(destroy), NULL);

  box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
  button_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);

  shutdown = gtk_button_new_with_label("Shutdown");
  g_signal_connect(G_OBJECT(shutdown), "button_press_event", G_CALLBACK(shutdown_f), NULL);

  reboot = gtk_button_new_with_label("Reboot");
  g_signal_connect(G_OBJECT(reboot), "button_press_event", G_CALLBACK(reboot_f), NULL);

  logout = gtk_button_new_with_label("Logout");
  g_signal_connect(G_OBJECT(logout), "button_press_event", G_CALLBACK(logout_f), NULL);

  gtk_container_add(GTK_CONTAINER(button_box), shutdown);
  gtk_container_add(GTK_CONTAINER(button_box), reboot);
  gtk_container_add(GTK_CONTAINER(button_box), logout);

  label = gtk_label_new("Please save all your data before you continue...");
  gtk_container_add(GTK_CONTAINER(box), label);
  gtk_container_add(GTK_CONTAINER(box), button_box);

  gtk_container_add(GTK_CONTAINER(win), box);

  gtk_widget_show_all(win);


}

#endif
