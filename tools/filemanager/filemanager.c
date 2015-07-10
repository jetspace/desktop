/*
This file is licensed under the MIT-License
Copyright (c) 2015 Marius Messerschmidt
For more details view file 'LICENSE'
*/
#define _XOPEN_SOURCE 500
#include <gtk/gtk.h>
#include <stdio.h>
#include <glib/gi18n.h>

enum {
  COL_NAME = 0,
  COL_ICON,
  COL_SIZE,
  COL_DIR,
  COL_FULL_NAME,
  N_COLS
};
char *path;
GtkListStore *files;
GtkTreeIter iter;
GtkWidget *places_bar;
#include "filemanager_callbacks.h"



int main(int argc, char **argv)
{
  textdomain("side");
  gtk_init(&argc, &argv);

  GtkWidget *win = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_resize(GTK_WINDOW(win), 625, 400);
  gtk_window_set_title(GTK_WINDOW(win), _("SiDE File Manager"));
  gtk_container_set_border_width(GTK_CONTAINER(win), 10);

  /*

  Layout Plan:
  MENUBAR # # # # # ## # ##  # # ## # # # # #
  +-------------,-----------------------------+
  | PLACES    | FILES                        |
  | PLACES    | FILES                        |
  +------------------------------------------+
  */
  GtkWidget *main_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
  GtkWidget *second_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
  gtk_box_pack_end(GTK_BOX(main_box), second_box, TRUE, TRUE, 0);
  gtk_container_add(GTK_CONTAINER(win), main_box);


  //sidebar
  places_bar = gtk_places_sidebar_new();
  gtk_places_sidebar_set_show_desktop (GTK_PLACES_SIDEBAR(places_bar), FALSE); //SiDE does not support desktop icons right now
  gtk_places_sidebar_set_show_connect_to_server (GTK_PLACES_SIDEBAR(places_bar), FALSE);
  gtk_places_sidebar_set_local_only (GTK_PLACES_SIDEBAR(places_bar), TRUE);
  gtk_box_pack_start(GTK_BOX(second_box), places_bar, FALSE, FALSE, 0);

  //create menu bar
  GtkWidget *menubar = gtk_menu_bar_new();


  //ABOUT
  GtkWidget *aboutmenu   = gtk_menu_new();
  GtkWidget *about       = gtk_menu_item_new_with_label(_("About"));
      gtk_menu_item_set_submenu(GTK_MENU_ITEM(about), aboutmenu);
      GtkWidget *ab     = gtk_menu_item_new_with_label(_("About"));
      gtk_menu_shell_append(GTK_MENU_SHELL(aboutmenu), ab);
      GtkWidget *help   = gtk_menu_item_new_with_label(_("Help"));
      gtk_menu_shell_append(GTK_MENU_SHELL(aboutmenu), help);
  gtk_menu_shell_append(GTK_MENU_SHELL(menubar), about);

  gtk_box_pack_start(GTK_BOX(main_box), menubar, FALSE, FALSE, 0);

  //icon view
  files = gtk_list_store_new(5, G_TYPE_STRING, GDK_TYPE_PIXBUF, G_TYPE_INT, G_TYPE_BOOLEAN, G_TYPE_STRING);
  update_files(FALSE, ".");
  GtkWidget *scroll_win = gtk_scrolled_window_new(NULL, NULL);
  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll_win), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
  GtkWidget *file_view = gtk_icon_view_new_with_model(GTK_TREE_MODEL(files));
  gtk_icon_view_set_text_column(GTK_ICON_VIEW(file_view), COL_NAME);
  gtk_icon_view_set_pixbuf_column(GTK_ICON_VIEW(file_view), COL_ICON);
  gtk_container_add(GTK_CONTAINER(scroll_win), file_view);
  gtk_box_pack_end(GTK_BOX(second_box), scroll_win, TRUE, TRUE, 2);


  //signals
  g_signal_connect(G_OBJECT(places_bar), "open-location", G_CALLBACK(open_location_cb), NULL);
  g_signal_connect(G_OBJECT(win), "destroy", G_CALLBACK(destroy_cb), NULL);
  g_signal_connect(G_OBJECT(file_view), "item-activated", G_CALLBACK(activated_file), NULL);
  g_signal_connect(G_OBJECT(ab), "activate", G_CALLBACK(show_about), NULL);
  g_signal_connect(G_OBJECT(help), "activate", G_CALLBACK(show_help), NULL);


  //show
  gtk_widget_show_all(win);
  gtk_main();

}
