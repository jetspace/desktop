/*
This file is licensed under the MIT-License
Copyright (c) 2015 Marius Messerschmidt
For more details view file 'LICENSE'
*/

#include <gtk/gtk.h>
#include <string.h>
#include <stdlib.h>
#include "../shared/strdup.h"
#include <glib/gi18n.h>
#include <gmodule.h>
#include <side/plugin.h>

enum
{
  COL_EXEC = 0,
  COL_N
};

GtkWidget *box, *label, *apply, *add, *del ,*view, *scroll_win, *button_box, *wm, *wm_label, *wm_box, *s, *s_label, *s_box, *r, *r_label, *r_box, *xdg_box, *xdg_switch, *xdg_label;
GtkWidget *cb_theme;
GtkListStore *themes;
GtkTreeIter iter;

gboolean write_themes = FALSE;

gboolean write_session_settings(GtkWidget *w, GdkEvent *e, gpointer *p)
{
  GSettings *se = g_settings_new("org.jetspace.desktop.session");

  g_settings_set_value(se, "wm", g_variant_new_string(gtk_entry_get_text(GTK_ENTRY(wm))));
  g_settings_set_value(se, "reboot", g_variant_new_string(gtk_entry_get_text(GTK_ENTRY(r))));
  g_settings_set_value(se, "shutdown", g_variant_new_string(gtk_entry_get_text(GTK_ENTRY(s))));
  g_settings_set_value(se, "xdg-autostart", g_variant_new_boolean(gtk_switch_get_active(GTK_SWITCH(xdg_switch))));

  char *str = malloc(1);
  memset(str, 0, sizeof(str));
  char i_b[5];
  int x = 0;
  char *buff;
  char buffer[2005];
  GtkTreeModel *model;
  GtkTreeIter iter;
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

  char *theme_string;
  model = gtk_combo_box_get_model(GTK_COMBO_BOX(cb_theme));
  gtk_combo_box_get_active_iter(GTK_COMBO_BOX(cb_theme), &iter);
  if(write_themes)
    gtk_tree_model_get(model, &iter, 0, &theme_string, -1);

  g_settings_set_value(se, "custom-theme-path", g_variant_new_string(g_strdup_printf("/usr/share/themes/%s",  theme_string)));

  if(write_themes)
    free(theme_string);

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
gboolean add_autostart_item(GtkWidget *w, GdkEvent *e, gpointer p)
{
  GtkTreeIter iter;
  gtk_list_store_append(GTK_LIST_STORE(p), &iter);
  gtk_list_store_set(GTK_LIST_STORE(p), &iter, COL_EXEC, "#Path" , -1);
  return FALSE;
}
gboolean remove_autostart_item(GtkWidget *w, GdkEvent *e, gpointer p)
{
  GtkTreeModel *model;
  GtkTreeIter iter;

  model = gtk_tree_view_get_model(GTK_TREE_VIEW(view));

  if(gtk_tree_selection_get_selected(gtk_tree_view_get_selection(GTK_TREE_VIEW(view)), &model, &iter))
  {
    gtk_list_store_remove(GTK_LIST_STORE(p), &iter);
  }
  return FALSE;
}


GtkWidget *build_session_settings(void)
{

  box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
  gtk_container_set_border_width(GTK_CONTAINER(box), 25);

  label = gtk_label_new(_("Choose the autostart applications:"));
  gtk_container_add(GTK_CONTAINER(box), label);

  scroll_win = gtk_scrolled_window_new(NULL, NULL);
  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll_win), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
  gtk_scrolled_window_set_min_content_height(GTK_SCROLLED_WINDOW(scroll_win), 200);
  gtk_container_add(GTK_CONTAINER(box), scroll_win);


  GtkListStore *autostart_list = gtk_list_store_new(COL_N, G_TYPE_STRING);

  GSettings *se = g_settings_new("org.jetspace.desktop.session");
  char *str    = strdup(g_variant_get_string(g_settings_get_value(se, "autostart"), NULL));

  char *p = strtok(str, ";");
  GtkTreeIter iter;
  while(p != NULL)
    {
        gtk_list_store_append(autostart_list, &iter);
        gtk_list_store_set(autostart_list, &iter, COL_EXEC, p, -1);
        p = strtok(NULL, ";");
    }
  free(str);
  free(p);

  view = gtk_tree_view_new();

  GtkCellRenderer *renderer;
  GtkTreeViewColumn *column;

  renderer = gtk_cell_renderer_text_new();
  g_object_set(renderer, "editable", TRUE, "editable-set", TRUE, NULL); //make it editable...
  column = gtk_tree_view_column_new_with_attributes(_("App"), renderer, "text", COL_EXEC, NULL);
  g_signal_connect(G_OBJECT(renderer), "edited", G_CALLBACK(cell_edit_e), (gpointer) view);
  gtk_tree_view_append_column(GTK_TREE_VIEW(view), column);

  gtk_tree_view_set_model(GTK_TREE_VIEW(view), GTK_TREE_MODEL(autostart_list));
  gtk_tree_view_expand_all(GTK_TREE_VIEW(view));
  gtk_widget_show(view);
  gtk_container_add(GTK_CONTAINER(scroll_win), view);

  //Button Box
  button_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 2);
  gtk_box_set_homogeneous(GTK_BOX(button_box), TRUE);

  //ADD a button to remove a app
  del = gtk_button_new_with_label(_("Remove"));
  gtk_container_add(GTK_CONTAINER(button_box), del);
  g_signal_connect(G_OBJECT(del), "button_press_event", G_CALLBACK(remove_autostart_item), autostart_list);

  //ADD a button for a new app
  add = gtk_button_new_with_label(_("Add"));
  gtk_container_add(GTK_CONTAINER(button_box), add);
  g_signal_connect(G_OBJECT(add), "button_press_event", G_CALLBACK(add_autostart_item), autostart_list);

  gtk_container_add(GTK_CONTAINER(box), button_box);

  //Window Manager (WM)

  wm_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);

    wm_label = gtk_label_new(_("Please choose the Window Manager (WM):"));
    gtk_box_pack_start(GTK_BOX(wm_box), wm_label, FALSE, FALSE, 5);

    wm = gtk_entry_new();
    char *ptr = strdup(g_variant_get_string(g_settings_get_value(se, "wm"), NULL));
    gtk_entry_set_text(GTK_ENTRY(wm), ptr);
    free(ptr);

    gtk_box_pack_end(GTK_BOX(wm_box), wm, FALSE, FALSE, 5);


  gtk_container_add(GTK_CONTAINER(box), wm_box);

  //Shutdown

  s_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);

    s_label = gtk_label_new(_("The command used to shutdown the computer:"));
    gtk_box_pack_start(GTK_BOX(s_box), s_label, FALSE, FALSE, 5);

    s = gtk_entry_new();
    ptr = strdup(g_variant_get_string(g_settings_get_value(se, "shutdown"), NULL));
    gtk_entry_set_text(GTK_ENTRY(s), ptr);
    free(ptr);

    gtk_box_pack_end(GTK_BOX(s_box), s, FALSE, FALSE, 5);


  gtk_container_add(GTK_CONTAINER(box), s_box);

  //Reboot

  r_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);

    r_label = gtk_label_new(_("The command used to reboot the computer:"));
    gtk_box_pack_start(GTK_BOX(r_box), r_label, FALSE, FALSE, 5);

    r = gtk_entry_new();
    ptr = strdup(g_variant_get_string(g_settings_get_value(se, "reboot"), NULL));
    gtk_entry_set_text(GTK_ENTRY(r), ptr);
    free(ptr);

    gtk_box_pack_end(GTK_BOX(r_box), r, FALSE, FALSE, 5);
    gtk_container_add(GTK_CONTAINER(box), r_box);

  //XDG autostart

    xdg_label = gtk_label_new(_("Enable XDG autostart:"));
    xdg_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_box_pack_start(GTK_BOX(xdg_box), xdg_label, FALSE, FALSE, 5);

    xdg_switch = gtk_switch_new();
    gtk_box_pack_end(GTK_BOX(xdg_box), xdg_switch, FALSE, FALSE, 5);

    gtk_switch_set_active(GTK_SWITCH(xdg_switch), g_variant_get_boolean(g_settings_get_value(se, "xdg-autostart")));

    gtk_container_add(GTK_CONTAINER(box), xdg_box);

  //themes

    themes = gtk_list_store_new(1, G_TYPE_STRING);
    char *querry = strdup(g_variant_get_string(g_settings_get_value(se, "custom-theme-path"), NULL));
    gboolean empty = FALSE;

    if(querry == NULL || strlen(querry) < 1 || strncmp(querry, "/usr/share/themes/", 18) != 0)
        empty = TRUE;

    if(!empty)
        {
            strtok(querry, "/");
            strtok(NULL, "/");
            strtok(NULL, "/");
            querry = strtok(NULL, "\n\0");
        }

    DIR *dir = opendir("/usr/share/themes/");
    struct dirent *e;
    int counter = -1;
    int id = 0;
    //setup panel themes
    while((e = readdir(dir)) != NULL)
    {
    if(e->d_name[0] != '.')//Switch to subdir
       {
         char *buffer = malloc(strlen("/usr/share/themes/ ") + strlen(e->d_name) +1);
         char *theme = strdup(e->d_name); //backup theme name if we have a match later
         snprintf(buffer, strlen("/usr/share/themes/ ") + strlen(e->d_name), "/usr/share/themes/%s", e->d_name);
         DIR *sub = opendir(buffer);
         free(buffer);
         if(sub == NULL)
             continue;
         struct dirent *f;
         while((f = readdir(sub)) != NULL)
          {
            if(strncmp(f->d_name, "side-session", 12) == 0) //contains a SiDE theme
              {
                write_themes = TRUE;
                gtk_list_store_append(GTK_LIST_STORE(themes), &iter);
                gtk_list_store_set(GTK_LIST_STORE(themes), &iter, 0, theme, -1);
                counter++;
                if(!empty)
                    {
                        if(strcmp(theme, querry) == 0)
                            {
                                id=counter; //select the current theme
                            }
                    }
              }
          }
          closedir(sub);
        free(theme);
       }
    else
       continue;
    }
    closedir(dir);

    cb_theme = gtk_combo_box_new_with_model(GTK_TREE_MODEL(themes));

    renderer = gtk_cell_renderer_text_new();
    gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(cb_theme), renderer, TRUE);
    gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(cb_theme), renderer,"text", 0,NULL);
    gtk_combo_box_set_active(GTK_COMBO_BOX(cb_theme), id);

    GtkWidget *tbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_box_pack_start(GTK_BOX(tbox), gtk_label_new(_("SiDE Theme:")), FALSE, FALSE, 0);
    gtk_box_pack_end(GTK_BOX(tbox), cb_theme, FALSE, FALSE, 0);

    gtk_box_pack_start(GTK_BOX(box), tbox, FALSE, FALSE, 0);




  apply = gtk_button_new_with_label(_("Apply"));
  gtk_box_pack_end(GTK_BOX(box), apply, FALSE, FALSE, 5);
  g_signal_connect(G_OBJECT(apply), "button-press-event", G_CALLBACK(write_session_settings), NULL);

  return box;
}

void callback(gpointer d)
{
  GtkBox *container = GTK_BOX(d);
  GtkWidget *cont = build_session_settings();
  gtk_box_pack_start(container, cont, TRUE, TRUE, 0);
}

SiDESettingsPluginDescription side_session_plugin_desc;
SiDESettingsPluginDescription *identify(gpointer data)
{
  side_session_plugin_desc.label = _("Session");
  side_session_plugin_desc.hover = _("Setup session autostart and more");
  side_session_plugin_desc.icon  = "preferences-system";
  side_session_plugin_desc.title = _("Session");
  side_session_plugin_desc.cmd = "settings.session";
  side_session_plugin_desc.category = SIDE_SETTINGS_CATEGORY_SYSTEM;
  return &side_session_plugin_desc;
}
