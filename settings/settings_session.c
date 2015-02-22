/*
This file is licensed under the MIT-License
Copyright (c) 2015 Marius Messerschmidt
For more details view file 'LICENSE'
*/

#include <gtk/gtk.h>
#include <string.h>
#include <stdlib.h>
#include "../shared/strdup.h"

enum
{
  COL_EXEC = 0,
  COL_N
};

GtkTreeIter iter;
GtkListStore *list;
GtkWidget *win, *box, *label, *apply, *add, *del ,*view, *scroll_win, *button_box, *wm, *wm_label, *wm_box, *s, *s_label, *s_box, *r, *r_label, *r_box;

gboolean write_session_settings(GtkWidget *w, GdkEvent *e, gpointer *p)
{
  GSettings *se = g_settings_new("org.jetspace.desktop.session");

  g_settings_set_value(se, "wm", g_variant_new_string(gtk_entry_get_text(GTK_ENTRY(wm))));
  g_settings_set_value(se, "reboot", g_variant_new_string(gtk_entry_get_text(GTK_ENTRY(r))));
  g_settings_set_value(se, "shutdown", g_variant_new_string(gtk_entry_get_text(GTK_ENTRY(s))));

  char *str = malloc(1);
  memset(str, 0, sizeof(str));
  char i_b[5];
  int x = 0;
  char *buff;
  char buffer[2005];
  GtkTreeModel *model;

  model = gtk_tree_view_get_model(GTK_TREE_VIEW(view));
  snprintf(i_b, 5, "%d", x);
  while(gtk_tree_model_get_iter_from_string(model, &iter, i_b))
    {
      gtk_tree_model_get(model, &iter, COL_EXEC, &buff, -1);
      snprintf(buffer, 2005, "%s;", buff);
      str = realloc(str, sizeof(str) + sizeof(buffer));
      strcat(str, buffer);
      x++;
      snprintf(i_b, 5, "%d", x);
    }

  g_settings_set_value(se, "autostart", g_variant_new_string(str));

  free(str);

  return FALSE;
}


gboolean destroy(GtkWidget *w, GdkEvent *e, gpointer *p)
{
  gtk_main_quit();
  return FALSE;
}

gboolean cell_edit_e(GtkCellRendererText *renderer, gchar *path, gchar *text, GtkTreeView *treeview)
{
  GtkTreeModel *model;
  GtkTreeIter iter;

  model = gtk_tree_view_get_model(treeview);

  if(gtk_tree_model_get_iter_from_string(model, &iter, path))
    {
      gtk_list_store_set(GTK_LIST_STORE(model), &iter, COL_EXEC, text, -1);
    }
  else
      g_warning("Failed editing cell: PATH: %s", path);


  return FALSE;
}
gboolean add_item(GtkWidget *w, GdkEvent *e, gpointer p)
{
  gtk_list_store_append(list, &iter);
  gtk_list_store_set(list, &iter, COL_EXEC, "#Path" , -1);
  return FALSE;
}
gboolean remove_item(GtkWidget *w, GdkEvent *e, gpointer p)
{
  GtkTreeModel *model;
  GtkTreeIter iter;

  model = gtk_tree_view_get_model(GTK_TREE_VIEW(view));

  if(gtk_tree_selection_get_selected(gtk_tree_view_get_selection(GTK_TREE_VIEW(view)), &model, &iter))
  {
    gtk_list_store_remove(list, &iter);
  }
  return FALSE;
}


int main(int argc, char **argv)
{
  gtk_init(&argc, &argv);

  win = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_resize(GTK_WINDOW(win), 800, 500);
  gtk_window_set_title(GTK_WINDOW(win), "Settings - Session");
  gtk_container_set_border_width(GTK_CONTAINER(win), 10);
  g_signal_connect(G_OBJECT(win), "delete-event", G_CALLBACK(destroy), NULL);

  box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
  gtk_container_add(GTK_CONTAINER(win), box);

  label = gtk_label_new("Choose the autostart applications:");
  gtk_container_add(GTK_CONTAINER(box), label);

  scroll_win = gtk_scrolled_window_new(NULL, NULL);
  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll_win), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
  gtk_scrolled_window_set_min_content_height(GTK_SCROLLED_WINDOW(scroll_win), 200);
  gtk_container_add(GTK_CONTAINER(box), scroll_win);


  list = gtk_list_store_new(COL_N, G_TYPE_STRING);

  GSettings *se = g_settings_new("org.jetspace.desktop.session");
  char *str    = strdup(g_variant_get_string(g_settings_get_value(se, "autostart"), NULL));

  char *p = strtok(str, ";");

  while(p != NULL)
    {
        gtk_list_store_append(list, &iter);
        gtk_list_store_set(list, &iter, COL_EXEC, p, -1);
        p = strtok(NULL, ";");
    }
  free(str);
  free(p);

  view = gtk_tree_view_new();

  GtkCellRenderer *renderer;
  GtkTreeViewColumn *column;

  renderer = gtk_cell_renderer_text_new();
  g_object_set(renderer, "editable", TRUE, "editable-set", TRUE, NULL); //make it editable...
  column = gtk_tree_view_column_new_with_attributes("App", renderer, "text", COL_EXEC, NULL);
  g_signal_connect(G_OBJECT(renderer), "edited", G_CALLBACK(cell_edit_e), (gpointer) view);
  gtk_tree_view_append_column(GTK_TREE_VIEW(view), column);

  gtk_tree_view_set_model(GTK_TREE_VIEW(view), GTK_TREE_MODEL(list));
  gtk_tree_view_expand_all(GTK_TREE_VIEW(view));
  gtk_widget_show(view);
  gtk_container_add(GTK_CONTAINER(scroll_win), view);

  //Button Box
  button_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 2);
  gtk_box_set_homogeneous(GTK_BOX(button_box), TRUE);

  //ADD a button to remove a app
  del = gtk_button_new_with_label("Delete");
  gtk_container_add(GTK_CONTAINER(button_box), del);
  g_signal_connect(G_OBJECT(del), "button_press_event", G_CALLBACK(remove_item), NULL);

  //ADD a button for a new app
  add = gtk_button_new_with_label("Add");
  gtk_container_add(GTK_CONTAINER(button_box), add);
  g_signal_connect(G_OBJECT(add), "button_press_event", G_CALLBACK(add_item), NULL);

  gtk_container_add(GTK_CONTAINER(box), button_box);

  //Window Manager (WM)

  wm_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);

    wm_label = gtk_label_new("Please choose the Window Manager (WM):");
    gtk_box_pack_start(GTK_BOX(wm_box), wm_label, FALSE, FALSE, 5);

    wm = gtk_entry_new();
    char *ptr = strdup(g_variant_get_string(g_settings_get_value(se, "wm"), NULL));
    gtk_entry_set_text(GTK_ENTRY(wm), ptr);
    free(ptr);

    gtk_box_pack_end(GTK_BOX(wm_box), wm, FALSE, FALSE, 5);


  gtk_container_add(GTK_CONTAINER(box), wm_box);

  //Shutdown

  s_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);

    s_label = gtk_label_new("The command used to shutdown the computer:");
    gtk_box_pack_start(GTK_BOX(s_box), s_label, FALSE, FALSE, 5);

    s = gtk_entry_new();
    ptr = strdup(g_variant_get_string(g_settings_get_value(se, "shutdown"), NULL));
    gtk_entry_set_text(GTK_ENTRY(s), ptr);
    free(ptr);

    gtk_box_pack_end(GTK_BOX(s_box), s, FALSE, FALSE, 5);


  gtk_container_add(GTK_CONTAINER(box), s_box);

  //Reboot

  r_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);

    r_label = gtk_label_new("The command used to reboot the computer:");
    gtk_box_pack_start(GTK_BOX(r_box), r_label, FALSE, FALSE, 5);

    r = gtk_entry_new();
    ptr = strdup(g_variant_get_string(g_settings_get_value(se, "reboot"), NULL));
    gtk_entry_set_text(GTK_ENTRY(r), ptr);
    free(ptr);

    gtk_box_pack_end(GTK_BOX(r_box), r, FALSE, FALSE, 5);


  gtk_container_add(GTK_CONTAINER(box), r_box);




  apply = gtk_button_new_with_label("Apply");
gtk_box_pack_end(GTK_BOX(box), apply, FALSE, FALSE, 5);
  g_signal_connect(G_OBJECT(apply), "button-press-event", G_CALLBACK(write_session_settings), NULL);


  gtk_widget_show_all(win);
  gtk_main();
}
