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
  system("side-session --shutdown");
  return FALSE;
}

gboolean reboot_f(GtkWidget *w, GdkEvent *e, gpointer *p)
{
  system("side-session --reboot");
  return FALSE;
}

gboolean logout_f(GtkWidget *w, GdkEvent *e, gpointer *p)
{
  system("side-session --logout");
  return FALSE;
}

void logout(void)
{
  GtkWidget *win, *shutdown, *reboot, *logout, *label, *box, *button_box, *icon;

  win = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_resize(GTK_WINDOW(win), 320, 200);
  gtk_window_set_title(GTK_WINDOW(win), "SIDE Logout");
  gtk_container_set_border_width(GTK_CONTAINER(win), 15);
  g_signal_connect(G_OBJECT(win), "delete-event", G_CALLBACK(destroy), NULL);

  box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
  button_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);

  shutdown = gtk_button_new_with_label(_("Shutdown"));
  g_signal_connect(G_OBJECT(shutdown), "button_press_event", G_CALLBACK(shutdown_f), NULL);

  reboot = gtk_button_new_with_label(_("Reboot"));
  g_signal_connect(G_OBJECT(reboot), "button_press_event", G_CALLBACK(reboot_f), NULL);

  logout = gtk_button_new_with_label(_("Logout"));
  g_signal_connect(G_OBJECT(logout), "button_press_event", G_CALLBACK(logout_f), NULL);

  gtk_box_pack_end(GTK_BOX(button_box), shutdown, FALSE, FALSE, 5);
  gtk_box_pack_end(GTK_BOX(button_box), reboot, FALSE, FALSE, 5);
  gtk_box_pack_end(GTK_BOX(button_box), logout, FALSE, FALSE, 5);

  icon = gtk_image_new_from_icon_name("application-exit", GTK_ICON_SIZE_DIALOG);
  gtk_box_pack_start(GTK_BOX(box), icon, FALSE, FALSE, 5);

  label = gtk_label_new(_("Please save all your data before you continue..."));
  gtk_box_pack_end(GTK_BOX(box), label, FALSE, FALSE, 5);

  gtk_box_pack_end(GTK_BOX(box), button_box, FALSE, FALSE, 5);

  gtk_container_add(GTK_CONTAINER(win), box);

  gtk_widget_show_all(win);


}

#endif
