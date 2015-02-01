/*
This file is licensed under the MIT-License
Copyright (c) 2015 Marius Messerschmidt
For more details view file 'LICENSE'
*/

#ifndef _HAVE_CONTEXT_H
#define _HAVE_CONTEXT_H
#include "../shared/settings.h"
#include "../shared/run.h"
#include <gtk/gtk.h>
#define TERMINAL_FALLBACK "xterm" //USE IF ERROR WITH GSETTINGS


gboolean run_clicked(GtkWidget *w, GdkEvent *e, gpointer *p)
{
  run_dialog("CONTEXT_MENU");
  return FALSE;
}

gboolean logout_clicked(GtkWidget *w, GdkEvent *e, gpointer *p)
{
  /*
  This is nothing more than a dirty test!!(!!!!)
  we will use some 'cooler' methods later!
  */
  system("killall j_*"); //will exit all j_ apps
  return FALSE;
}

gboolean terminal_clicked(GtkWidget *w, GdkEvent *e, gpointer *p)
{
  GSettings *term = g_settings_new("org.gnome.desktop.default-applications.terminal");
  char *ptr = strdup(g_variant_get_string(g_settings_get_value(term, "exec"), NULL));
  g_debug("[%s] - Executing Terminal Emulator %s", "PANEL", ptr);
  system(ptr != NULL ? ptr : TERMINAL_FALLBACK);
  return FALSE;
}


void add_context_menu_pannel(GtkWidget *menu, GtkWidget *panel)
{
  //All Actions from context menu
  GtkWidget *run, *terminal, *sep , *sep2, *logout, *settings;

  run = gtk_menu_item_new_with_label("Run...");
  terminal = gtk_menu_item_new_with_label("Open Terminal...");
  sep = gtk_separator_menu_item_new();
  sep2 = gtk_separator_menu_item_new();
  logout = gtk_menu_item_new_with_label("log out...");
  settings = gtk_menu_item_new_with_label("Settings...");


  g_signal_connect(G_OBJECT(run), "activate", G_CALLBACK(run_clicked), NULL);
  g_signal_connect(G_OBJECT(terminal), "activate", G_CALLBACK(terminal_clicked), NULL);
  g_signal_connect(G_OBJECT(logout), "activate", G_CALLBACK(logout_clicked), NULL);
  g_signal_connect(G_OBJECT(settings), "activate", G_CALLBACK(panel_settings), NULL);

  gtk_menu_shell_append(GTK_MENU_SHELL(menu), run);
  gtk_menu_shell_append(GTK_MENU_SHELL(menu), terminal);
  gtk_menu_shell_append(GTK_MENU_SHELL(menu), sep);
  gtk_menu_shell_append(GTK_MENU_SHELL(menu), settings);
  gtk_menu_shell_append(GTK_MENU_SHELL(menu), sep2);
  gtk_menu_shell_append(GTK_MENU_SHELL(menu), logout);

  gtk_menu_attach_to_widget(GTK_MENU(menu), panel, NULL);
  gtk_widget_show_all(menu);
}

void add_context_menu_desktop(GtkWidget *menu, GtkWidget *desktop)
{
  GtkWidget *run, *change_wallpaper, *sep, *logout;

  run = gtk_menu_item_new_with_label("Run...");
  change_wallpaper = gtk_menu_item_new_with_label("Change Wallpaper...");
  sep = gtk_separator_menu_item_new();
  logout = gtk_menu_item_new_with_label("log out...");


  g_signal_connect(G_OBJECT(run), "activate", G_CALLBACK(run_clicked), NULL);
  g_signal_connect(G_OBJECT(change_wallpaper), "activate", G_CALLBACK(wallpaper_settings), NULL);
  g_signal_connect(G_OBJECT(logout), "activate", G_CALLBACK(logout_clicked), NULL);

  gtk_menu_shell_append(GTK_MENU_SHELL(menu), run);
  gtk_menu_shell_append(GTK_MENU_SHELL(menu), change_wallpaper);
  gtk_menu_shell_append(GTK_MENU_SHELL(menu), sep);
  gtk_menu_shell_append(GTK_MENU_SHELL(menu), logout);

  gtk_menu_attach_to_widget(GTK_MENU(menu), desktop, NULL);
  gtk_widget_show_all(menu);

}

#endif
