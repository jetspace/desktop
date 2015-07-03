/*
This file is licensed under the MIT-License
Copyright (c) 2015 Marius Messerschmidt
For more details view file 'LICENSE'
*/

#include <gtk/gtk.h>
#include <string.h>
#include <stdlib.h>
#include <side/apps.h>
#include "../shared/strdup.h"
#include <ctype.h>

#include <glib/gi18n.h>


GtkEntryCompletion *comp;
GtkListStore *apps;
GtkTreeIter iter;
GtkTreeModelFilter *filter;

GtkWidget *entry;

enum
{
  COL_NAME = 0,
  COL_EXEC,
  COL_PIC
};

gboolean destroy(GtkWidget *w, GdkEvent *e, gpointer *p)
{
  gtk_main_quit();
  return FALSE;
}

gboolean activated(GtkIconView *iconview, gpointer *data)
{
  GList *list = gtk_icon_view_get_selected_items(iconview);
  GList *it;

  for(it = list; it != NULL; it = g_list_next(it))
    {
      if(gtk_icon_view_path_is_selected(iconview, it->data))
        {
          char buffer[2005];
          char *e;
          GtkTreeIter i;
          GtkTreePath *p = gtk_tree_model_filter_convert_path_to_child_path(filter, it->data);
          GtkTreeModel *model = gtk_tree_model_filter_get_model(filter);

          gtk_tree_model_get_iter(model, &i, p);
          gtk_tree_model_get(model, &i, 1, &e, -1);
          strcpy(buffer, e);
          free(e);
          strcat(buffer, " &");
          system(buffer);
        }
    }
  return FALSE;
}


static gboolean show_app(GtkTreeModel *apps, GtkTreeIter *iter, gpointer *data)
{

    char *name, *exec;
    gtk_tree_model_get (apps, iter, COL_NAME, &name, COL_EXEC, &exec, -1);
    char *querry = strdup(gtk_entry_get_text(GTK_ENTRY(entry)));


    //Ignore Case Sensitive
    for(int x = 0; x < strlen(querry); x++)
      querry[x] = tolower(querry[x]);

      for(int x = 0; x < strlen(name); x++)
        name[x] = tolower(name[x]);

    /*if(strlen(name) < strlen(querry))
    {
      free(querry);
      free(name);
      free(exec);
      return FALSE;
    }*/


    if(strstr(name, querry) != 0)
      {
        free(querry);
        free(name);
        free(exec);
        return TRUE;
      }
    else if(strstr(exec, querry) != 0)
      {
        free(querry);
        free(name);
        free(exec);
        return TRUE;
      }
    else
      {
        free(querry);
        free(name);
        free(exec);
        return FALSE;
      }

}

gboolean querry_change(GtkWidget *b, GdkEvent *e, GtkTreeModelFilter *filter)
{
  gtk_tree_model_filter_refilter(filter);
  return FALSE;
}

int main(int argc, char **argv)
{

  textdomain("side");

  gtk_init(&argc, &argv);

  GSettings *conf = g_settings_new("org.jetspace.desktop.search");

  GtkWidget *win = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_resize(GTK_WINDOW(win), 500, 400);
  gtk_window_set_title(GTK_WINDOW(win), _("SiDE Search Tool"));
  gtk_container_set_border_width(GTK_CONTAINER(win), 10);
  g_signal_connect(G_OBJECT(win), "delete-event", G_CALLBACK(destroy), NULL);

  GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);

  GtkWidget *notebook = gtk_notebook_new();
  gtk_notebook_append_page(GTK_NOTEBOOK(notebook), box, gtk_label_new(_("Apps")));


  gtk_container_add(GTK_CONTAINER(win), notebook);

  apps = gtk_list_store_new(3, G_TYPE_STRING, G_TYPE_STRING, GDK_TYPE_PIXBUF, -1);

  //setup APP db
  side_apps_load();
  AppEntry ent;

  GtkIconTheme *theme = gtk_icon_theme_get_default();

  do
  {
    ent = side_apps_get_next_entry();

    if(ent.show == FALSE || ent.valid == FALSE)
      continue;
    gtk_list_store_append(apps, &iter);
    gtk_list_store_set(apps, &iter, COL_NAME, ent.app_name, COL_EXEC, ent.exec, COL_PIC,gtk_icon_theme_load_icon(theme, ent.icon, 32,GTK_ICON_LOOKUP_FORCE_SIZE ,NULL), -1);

  }while(ent.valid == TRUE);
  side_apps_close();

  filter = GTK_TREE_MODEL_FILTER(gtk_tree_model_filter_new(GTK_TREE_MODEL(apps), NULL));
  gtk_tree_model_filter_set_visible_func (filter,(GtkTreeModelFilterVisibleFunc) show_app , apps, NULL);

  entry = gtk_entry_new();
  if(g_variant_get_boolean(g_settings_get_value(conf, "show-completion")))
    {
      comp = gtk_entry_completion_new();
      gtk_entry_completion_set_text_column(comp, COL_NAME);
      gtk_entry_set_completion(GTK_ENTRY(entry), comp);
      gtk_entry_completion_set_model(comp, GTK_TREE_MODEL(apps));
    }
  g_signal_connect(G_OBJECT(entry), "key-press-event", G_CALLBACK(querry_change), filter);

  gtk_box_pack_start(GTK_BOX(box), entry, FALSE, FALSE, 0);


  GtkWidget *icon_view = gtk_icon_view_new_with_model(GTK_TREE_MODEL(filter));
  gtk_icon_view_set_text_column(GTK_ICON_VIEW(icon_view), COL_NAME);
  gtk_icon_view_set_pixbuf_column(GTK_ICON_VIEW(icon_view), COL_PIC);
  g_signal_connect(G_OBJECT(icon_view), "item-activated", G_CALLBACK(activated), filter);

  GtkWidget *scroll_win = gtk_scrolled_window_new(NULL, NULL);
  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll_win), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
  gtk_scrolled_window_set_min_content_height(GTK_SCROLLED_WINDOW(scroll_win), 200);



  gtk_container_add(GTK_CONTAINER(scroll_win), icon_view);
  gtk_box_pack_end(GTK_BOX(box), scroll_win, TRUE, TRUE, 2);
  gtk_box_pack_end(GTK_BOX(box), gtk_separator_new(GTK_ORIENTATION_HORIZONTAL), FALSE, FALSE, 0);

  gtk_widget_show_all(win);
  gtk_main();

}
