/*
This file is licensed under the MIT-License
Copyright (c) 2015 Marius Messerschmidt
For more details view file 'LICENSE'
*/

#include <jetspace/configkit.h>
#include <gtk/gtk.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <glib/gi18n.h>
#include <side/plugin.h>

char MIMEDB[2000];
#define MIMEFALLBACK "/etc/side/mime.conf"

GtkListStore *list;
GtkTreeIter iter;

gboolean edited_app(GtkCellRendererText *renderer, gchar *path, gchar *text, GtkTreeView *treeview)
{
  GtkTreeModel *model;
  GtkTreeIter iter;

  model = gtk_tree_view_get_model(treeview);

  if(gtk_tree_model_get_iter_from_string(model, &iter, path))
    {
      gtk_list_store_set(GTK_LIST_STORE(model), &iter, 1, text, -1);
    }
  else
      g_warning("Failed editing cell: PATH: {%s}", path);


  return FALSE;
}

gboolean destroy(GtkWidget *w, GdkEvent *e, gpointer p)
{
  gtk_main_quit();
  return FALSE;
}

gboolean write_mime_config(GtkWidget *w, GdkEvent *e, gpointer p)
{
  char i_b[5]; //int buffer
  GtkTreeModel *model;
  GtkTreeIter iter;
  int x = 0;

  model = GTK_TREE_MODEL(list);
  snprintf(i_b, 5, "%d", x);
  while(gtk_tree_model_get_iter_from_string(model, &iter, i_b))
    {
      char *ent = NULL, *val = NULL;
      gtk_tree_model_get(model, &iter, 0, &ent, -1);
      gtk_tree_model_get(model, &iter, 1, &val, -1);

      jet_set_value(MIMEDB, ent, val, true);

      x++;
      snprintf(i_b, 5, "%d", x);
    }

  return FALSE;
}

GtkWidget *build_mime_settigns(void)
{
  memset(MIMEDB, 0, 2000);
  struct passwd *pw = getpwuid(getuid());
  const char *homedir = pw->pw_dir;

  strcat(MIMEDB, homedir);
  strcat(MIMEDB, "/.config/side/mime.conf");

  if(access(MIMEDB, F_OK ) == -1 )
  {
    fprintf(stderr, _("Can't find local configuration {%s} using system fallback {%s}\n"), MIMEDB, MIMEFALLBACK);
    memset(MIMEDB, 0, 2000);
    strcat(MIMEDB, MIMEFALLBACK);
  }

  list = gtk_list_store_new(2, G_TYPE_STRING, G_TYPE_STRING);

  jet_init_config_read(MIMEDB);

  char *ret = NULL;
  do
    {
      ret = jet_get_next_entry();
      if(ret == NULL)
        break;

      gtk_list_store_append(GTK_LIST_STORE(list), &iter);
      gtk_list_store_set(GTK_LIST_STORE(list), &iter, 0, ret, 1, jet_lookup_value(MIMEDB, ret), -1);



    } while(ret != NULL);

  jet_close_config_read();

  GtkWidget *label = gtk_label_new(_("Here you can select which app should handle which MIME type by default:"));
  GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 2);
  gtk_box_pack_start(GTK_BOX(box), label, FALSE, FALSE, 0);


  //LISTVIEW
  GtkWidget *tree = gtk_tree_view_new();
  GtkWidget *scroll_win = gtk_scrolled_window_new(NULL, NULL);
  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll_win), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
  gtk_scrolled_window_set_min_content_height(GTK_SCROLLED_WINDOW(scroll_win), 200);

  GtkCellRenderer *renderer;
  GtkTreeViewColumn *column;

  renderer = gtk_cell_renderer_text_new();
  g_object_set(renderer, "editable", TRUE, "editable-set", TRUE, NULL); //make it editable...
  column = gtk_tree_view_column_new_with_attributes(_("MIME Type"), renderer, "text", 0, NULL);
  gtk_tree_view_append_column(GTK_TREE_VIEW(tree), column);

  renderer = gtk_cell_renderer_text_new();
  g_object_set(renderer, "editable", TRUE, "editable-set", TRUE, NULL); //make it editable...
  column = gtk_tree_view_column_new_with_attributes(_("App"), renderer, "text", 1, NULL);
  //signal
  g_signal_connect(G_OBJECT(renderer), "edited", G_CALLBACK(edited_app), (gpointer) tree);
  gtk_tree_view_append_column(GTK_TREE_VIEW(tree), column);


  gtk_tree_view_set_model(GTK_TREE_VIEW(tree), GTK_TREE_MODEL(list));
  gtk_box_pack_start(GTK_BOX(box), scroll_win, TRUE, TRUE, 0);
  gtk_container_add(GTK_CONTAINER(scroll_win), tree);

  GtkWidget *button = gtk_button_new_with_label(_("Apply"));
  g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(write_mime_config), NULL);
  gtk_box_pack_end(GTK_BOX(box), button, FALSE, FALSE, 0);

  return box;

}

void callback(gpointer d)
{
  GtkBox *container = GTK_BOX(d);
  GtkWidget *cont = build_mime_settigns();
  gtk_box_pack_start(container, cont, TRUE, TRUE, 0);
}

SiDESettingsPluginDescription side_mime_plugin_desc;

SiDESettingsPluginDescription *identify(gpointer data)
{
  side_mime_plugin_desc.label = _("MiME-Types");
  side_mime_plugin_desc.hover = _("Setup MiME types and default applications");
  side_mime_plugin_desc.icon  = "preferences-desktop-personal";
  side_mime_plugin_desc.category = 1;
  return &side_mime_plugin_desc;
}
