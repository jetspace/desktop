/*
This file is licensed under the MIT-License
Copyright (c) 2015 Marius Messerschmidt
For more details view file 'LICENSE'
*/

/*
Will contain all settings and their dialogs!
*/

#include <gtk/gtk.h>

GtkTreeIter iter;
GtkListStore *list;
GtkWidget *win, *box, *apply, *entry_path, *label_term, *label_sum, *tree, *scroll_win, *label_apps, *add_app_button;

enum
{
  COL_EXEC = 0,
  COL_ICON,
  COL_ACTIVE,
  N_COLS
};

gboolean add_item(GtkWidget *w, GdkEvent *e, gpointer p)
{
  gtk_list_store_append(list, &iter);
  gtk_list_store_set(list, &iter, COL_EXEC, "#App Path", COL_ICON, "#Icon", COL_ACTIVE, TRUE, -1);
  return FALSE;
}

gboolean write_panel_settings(GtkWidget *w, GdkEvent *e, gpointer *p)
{
  GSettings *term_app = g_settings_new("org.gnome.desktop.default-applications.terminal");
  g_settings_set_value(term_app, "exec", g_variant_new_string(gtk_entry_get_text(GTK_ENTRY(entry_path))));

  char *buff1;
  char *buff2;
  char  buffer[1000];
  char  *str = malloc(2);
  memset(str, 0, sizeof(str));
  char i_b[5]; //int buffer
  gboolean state;
  GtkTreeModel *model;
  GtkTreeIter iter;
  int x = 0;

  model = gtk_tree_view_get_model(GTK_TREE_VIEW(tree));
  snprintf(i_b, 5, "%d", x);
  while(gtk_tree_model_get_iter_from_string(model, &iter, i_b))
    {
      gtk_tree_model_get(model, &iter, COL_ACTIVE, &state, -1);
      gtk_tree_model_get(model, &iter, COL_EXEC, &buff2, -1);
      gtk_tree_model_get(model, &iter, COL_ICON, &buff1, -1);

      snprintf(buffer, 1000, "%s:%s:%d;", buff1, buff2, state == TRUE ? 1 : 0);
      str = realloc(str, sizeof(str) + sizeof(buffer));
      strcat(str, buffer);
      x++;
      snprintf(i_b, 5, "%d", x);
    }

  GSettings *icons = g_settings_new("org.jetspace.desktop.panel");
  g_settings_set_value(icons, "apps", g_variant_new_string(str));
  free(str);

  gtk_widget_destroy(win);
  return FALSE;
}



gboolean check_box_toggle(GtkCellRendererToggle *renderer, gchar *path, GtkTreeView *treeview)
{
  GtkTreeModel *model;
  GtkTreeIter iter;
  gboolean state;

  model = gtk_tree_view_get_model(treeview);
  if(gtk_tree_model_get_iter_from_string(model, &iter, path))
    {
      gtk_tree_model_get(model, &iter, COL_ACTIVE, &state, -1);
      gtk_list_store_set(GTK_LIST_STORE(model), &iter, COL_ACTIVE, !state, -1);
    }
  else
      g_warning("Failed switching state of checkbox: PATH: %s", path);

  return FALSE;
}

//EXEC
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

//ICON
gboolean cell_edit_i(GtkCellRendererText *renderer, gchar *path, gchar *text, GtkTreeView *treeview)
{
  GtkTreeModel *model;
  GtkTreeIter iter;

  model = gtk_tree_view_get_model(treeview);

  if(gtk_tree_model_get_iter_from_string(model, &iter, path))
    {
      gtk_list_store_set(GTK_LIST_STORE(model), &iter, COL_ICON, text, -1);
    }
  else
      g_warning("Failed editing cell: PATH: %s", path);


  return FALSE;
}


gboolean panel_settings(GtkWidget *w, GdkEvent *e, gpointer *p)
{
  GSettings *term_app = g_settings_new("org.gnome.desktop.default-applications.terminal");
  char *ptr = strdup(g_variant_get_string(g_settings_get_value(term_app, "exec"), NULL));



  label_sum = gtk_label_new("On this page you can set up the Panel:\n\n");
  label_term= gtk_label_new("Select the default Terminal Application:");

  entry_path = gtk_entry_new();
  gtk_entry_set_text(GTK_ENTRY(entry_path), ptr);

  win = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_resize(GTK_WINDOW(win), 800, 500);
  gtk_window_set_title(GTK_WINDOW(win), "Settings - Panel");
  gtk_container_set_border_width(GTK_CONTAINER(win), 10);

  box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);

  apply = gtk_button_new_from_icon_name("document-save", GTK_ICON_SIZE_BUTTON);

  gtk_container_add(GTK_CONTAINER(box), label_sum);
  gtk_container_add(GTK_CONTAINER(box), label_term);
  gtk_container_add(GTK_CONTAINER(box), entry_path);
  g_signal_connect(G_OBJECT(apply), "button_press_event", G_CALLBACK(write_panel_settings), NULL);

  //create list store for panel icons...

                                //   EXEC           ICON          SWITCH
  list = gtk_list_store_new(N_COLS, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_BOOLEAN);

  //clear old string, now read apps
  g_free(ptr);
  GSettings *icons = g_settings_new("org.jetspace.desktop.panel");
  ptr = strdup(g_variant_get_string(g_settings_get_value(icons, "apps"), NULL));

  char *exec, *icon, *active;
  icon = strtok(ptr, ":");
  exec = strtok(NULL, ":");
  active = strtok(NULL, ";");
  while(icon != NULL && exec != NULL)
    {
      int enabled = atoi(active);
      gtk_list_store_append(list, &iter);
      gtk_list_store_set(list, &iter, COL_EXEC, exec, COL_ICON, icon, COL_ACTIVE, enabled == 1 ? TRUE : FALSE, -1);
      icon = strtok(NULL, ":");
      exec = strtok(NULL, ":");
      active = strtok(NULL, ";");
    }


  tree = gtk_tree_view_new();

  scroll_win = gtk_scrolled_window_new(NULL, NULL);
  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll_win), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
  gtk_scrolled_window_set_min_content_height(GTK_SCROLLED_WINDOW(scroll_win), 200);

  GtkCellRenderer *renderer;
  GtkTreeViewColumn *column;

  renderer = gtk_cell_renderer_text_new();
  g_object_set(renderer, "editable", TRUE, "editable-set", TRUE, NULL); //make it editable...
  column = gtk_tree_view_column_new_with_attributes("Path", renderer, "text", COL_EXEC, NULL);
  //signal
  g_signal_connect(G_OBJECT(renderer), "edited", G_CALLBACK(cell_edit_e), (gpointer) tree);
  gtk_tree_view_append_column(GTK_TREE_VIEW(tree), column);

  renderer = gtk_cell_renderer_text_new();
  g_object_set(renderer, "editable", TRUE, "editable-set", TRUE, NULL); //make it editable...
  column = gtk_tree_view_column_new_with_attributes("Icon", renderer, "text", COL_ICON, NULL);
  //signal
  g_signal_connect(G_OBJECT(renderer), "edited", G_CALLBACK(cell_edit_i), (gpointer) tree);
  gtk_tree_view_append_column(GTK_TREE_VIEW(tree), column);

  renderer = gtk_cell_renderer_toggle_new();
  column = gtk_tree_view_column_new_with_attributes("Enabled", renderer, "active", COL_ACTIVE, NULL);
  //signal
  g_signal_connect(G_OBJECT(renderer), "toggled", G_CALLBACK(check_box_toggle), (gpointer) tree);

  gtk_tree_view_append_column(GTK_TREE_VIEW(tree), column);

  gtk_tree_view_set_model(GTK_TREE_VIEW(tree), GTK_TREE_MODEL(list));
  gtk_tree_view_expand_all(GTK_TREE_VIEW(tree));

  //disc for the app editor
  label_apps = gtk_label_new("Choose the Apps which are displayed in the Panel. You can disable, edit or add a new icon.");
  gtk_container_add(GTK_CONTAINER(box), label_apps);

  //put the scroll_win in the box
  gtk_container_add(GTK_CONTAINER(scroll_win), tree);
  gtk_container_add(GTK_CONTAINER(box), scroll_win);

  //ADD a button for a new app
  add_app_button = gtk_button_new_from_icon_name("document-new", GTK_ICON_SIZE_BUTTON);
  gtk_container_add(GTK_CONTAINER(box), add_app_button);
  g_signal_connect(G_OBJECT(add_app_button), "button_press_event", G_CALLBACK(add_item), NULL);

  gtk_container_add(GTK_CONTAINER(box), apply);
  gtk_container_add(GTK_CONTAINER(win), box);



  gtk_widget_show_all(win);
  return FALSE;

}
