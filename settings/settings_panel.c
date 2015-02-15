/*
This file is licensed under the MIT-License
Copyright (c) 2015 Marius Messerschmidt
For more details view file 'LICENSE'
*/

#include <gtk/gtk.h>
#include <string.h>
#include <stdlib.h>
#include "../shared/strdup.h"
#include "../shared/listed.h"

//page basic layout
GtkWidget *win_box;

//page 1
GtkTreeIter iter;
GtkListStore *list;
GtkWidget *win, *box, *apply, *entry_path, *label_term, *label_sum, *tree, *scroll_win, *label_apps, *add_app_button, *remove_app_button, *clear_app_button, *app_button_box, *notebook;

//page 2

GtkWidget *box2, *label_mod, *modview, *scroll_win2, *label_apply;
GtkListStore *modlist;
GtkTreeIter iter2;

//page 3

GtkWidget *box3, *s_visible, *label_visible, *visible_box;

enum
{
  COL_EXEC = 0,
  COL_ICON,
  COL_ACTIVE,
  N_COLS
};

enum
{
  COL_NAME = 0,
  COL_ENABLED,
  COLS_MOD
};

gboolean add_item(GtkWidget *w, GdkEvent *e, gpointer p)
{
  gtk_list_store_append(list, &iter);
  gtk_list_store_set(list, &iter, COL_EXEC, "#App Path", COL_ICON, "#Icon", COL_ACTIVE, TRUE, -1);
  return FALSE;
}

gboolean clear_items(GtkWidget *w, GdkEvent *e, gpointer p)
{
  gtk_list_store_clear(list);
  return FALSE;
}

gboolean remove_item(GtkWidget *w, GdkEvent *e, gpointer p)
{
  GtkTreeModel *model;
  GtkTreeIter iter;

  model = gtk_tree_view_get_model(GTK_TREE_VIEW(tree));

  if(gtk_tree_selection_get_selected(gtk_tree_view_get_selection(GTK_TREE_VIEW(tree)), &model, &iter))
  {
    gtk_list_store_remove(list, &iter);
  }
  return FALSE;
}

gboolean destroy(GtkWidget *w, GdkEvent *e, gpointer *p)
{
  gtk_main_quit();
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


  x = 0;
  str = malloc(2);
  memset(str, 0, sizeof(str));
  model = gtk_tree_view_get_model(GTK_TREE_VIEW(modview));
  snprintf(i_b, 5, "%d", x);
  while(gtk_tree_model_get_iter_from_string(model, &iter, i_b))
    {
      gtk_tree_model_get(model, &iter, COL_NAME, &buff1, -1);
      gtk_tree_model_get(model, &iter, COL_ENABLED, &state, -1);

      if(!state)
        {
          snprintf(buffer, 1000, "%s;", buff1);
          str = realloc(str, sizeof(str) + sizeof(buffer));
          strcat(str, buffer);
        }
        x++;
        snprintf(i_b, 5, "%d", x);
    }

  icons = g_settings_new("org.jetspace.desktop.panel");
  g_settings_set_value(icons, "ignored-plugins", g_variant_new_string(str));

  g_settings_set_value(icons, "show-app-menu", g_variant_new_boolean(gtk_switch_get_active(GTK_SWITCH(s_visible))));

  g_settings_sync();
  return FALSE;
}



gboolean check_box_toggle_mods(GtkCellRendererToggle *renderer, gchar *path, GtkTreeView *treeview)
{
  GtkTreeModel *model;
  GtkTreeIter iter;
  gboolean state;

  model = gtk_tree_view_get_model(treeview);
  if(gtk_tree_model_get_iter_from_string(model, &iter, path))
    {
      gtk_tree_model_get(model, &iter, COL_ENABLED, &state, -1);
      gtk_list_store_set(GTK_LIST_STORE(model), &iter, COL_ENABLED, !state, -1);
    }
  else
      g_warning("Failed switching state of checkbox: PATH: %s", path);

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


gboolean panel_settings(void)
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
  g_signal_connect(G_OBJECT(win), "delete-event", G_CALLBACK(destroy), NULL);

  notebook = gtk_notebook_new();
  //page 1
    box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);

    apply = gtk_button_new_with_label("Apply");

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

    //Button Box
    app_button_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 2);
    gtk_box_set_homogeneous(GTK_BOX(app_button_box), TRUE);

    //ADD a button to clear all apps
    clear_app_button = gtk_button_new_with_label("Clear");
    gtk_container_add(GTK_CONTAINER(app_button_box), clear_app_button);
    g_signal_connect(G_OBJECT(clear_app_button), "button_press_event", G_CALLBACK(clear_items), NULL);

    //ADD a button to remove a app
    remove_app_button = gtk_button_new_with_label("Delete");
    gtk_container_add(GTK_CONTAINER(app_button_box), remove_app_button);
    g_signal_connect(G_OBJECT(remove_app_button), "button_press_event", G_CALLBACK(remove_item), NULL);


    //ADD a button for a new app
    add_app_button = gtk_button_new_with_label("Add");
    gtk_container_add(GTK_CONTAINER(app_button_box), add_app_button);
    g_signal_connect(G_OBJECT(add_app_button), "button_press_event", G_CALLBACK(add_item), NULL);

    gtk_container_add(GTK_CONTAINER(box), app_button_box);


  //Page 2

    box2 = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
                                      //   name        Bool
    modlist = gtk_list_store_new(COLS_MOD, G_TYPE_STRING, G_TYPE_BOOLEAN);

    DIR *d;
    struct dirent *de;

    d = opendir("/usr/lib/jetspace/panel/plugins/");
    if(d == NULL)
      {
        g_error("MODULE PATH NOT FOUND");
      }


    ptr = strdup(g_variant_get_string(g_settings_get_value(icons, "ignored-plugins"), NULL));
    char **mods = malloc(sizeof(ptr));

    char *mp = strtok(ptr, ";");

    int x = 0;
    while(mp != NULL)
      {
        mods[x] = strdup(mp);
        x++;
        mp = strtok(NULL, ";");
      }

    while((de = readdir(d)) != NULL)
      {
        gboolean c = FALSE;

        if(de->d_name[0] == '.')
            continue;

        if(is_listed(mods, x, de->d_name) == TRUE)
            c = FALSE;
          else
            c = TRUE;

        gtk_list_store_append(modlist, &iter2);
        gtk_list_store_set(modlist, &iter2, COL_NAME, de->d_name, COL_ENABLED, c, -1);
      }

    modview = gtk_tree_view_new();

    scroll_win2 = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll_win2), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_scrolled_window_set_min_content_height(GTK_SCROLLED_WINDOW(scroll_win2), 200);


    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Plugin", renderer, "text", COL_NAME, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(modview), column);

    renderer = gtk_cell_renderer_toggle_new();
    column = gtk_tree_view_column_new_with_attributes("Is Active", renderer, "active", COL_ENABLED, NULL);
    g_signal_connect(G_OBJECT(renderer), "toggled", G_CALLBACK(check_box_toggle_mods), (gpointer) modview);
    gtk_tree_view_append_column(GTK_TREE_VIEW(modview), column);

    gtk_tree_view_set_model(GTK_TREE_VIEW(modview), GTK_TREE_MODEL(modlist));
    gtk_tree_view_expand_all(GTK_TREE_VIEW(modview));

    gtk_container_add(GTK_CONTAINER(scroll_win2), modview);

    label_mod = gtk_label_new("Here you can disable Plugins from beeing loaded");
    label_apply = gtk_label_new("");
    gtk_label_set_markup(GTK_LABEL(label_apply), "<b>NOTICE:</b> a restart of the Desktop is requierd to reload the Plugins!\nThis is because all plugins are loaded at startup and are fixed, so they can not be changed later...");


    gtk_container_add(GTK_CONTAINER(box2), label_mod);
    gtk_container_add(GTK_CONTAINER(box2), scroll_win2);
    gtk_container_add(GTK_CONTAINER(box2), label_apply);

  // Page 3

    box3 = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);

    label_visible = gtk_label_new("Would you like to enable the built-in application menu?");
    s_visible = gtk_switch_new();
    visible_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_container_add(GTK_CONTAINER(visible_box), label_visible);
    gtk_box_pack_end(GTK_BOX(visible_box), s_visible, FALSE, FALSE, 5);

    gtk_switch_set_active(GTK_SWITCH(s_visible), g_variant_get_boolean(g_settings_get_value(icons, "show-app-menu")));

    gtk_container_add(GTK_CONTAINER(box3), visible_box);

  gtk_notebook_append_page(GTK_NOTEBOOK(notebook), box, gtk_label_new("Apps"));
  gtk_notebook_append_page(GTK_NOTEBOOK(notebook), box2, gtk_label_new("Plugins"));
  gtk_notebook_append_page(GTK_NOTEBOOK(notebook), box3, gtk_label_new("Application Menu"));

  //create win_box

  win_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);


  gtk_container_add(GTK_CONTAINER(win_box), notebook);
  gtk_container_add(GTK_CONTAINER(win_box), apply);

  gtk_container_add(GTK_CONTAINER(win), win_box);



  gtk_widget_show_all(win);
  return FALSE;

}

int main(int argc, char **argv)
{
  gtk_init(&argc, &argv);

  panel_settings();

  gtk_main();
}
