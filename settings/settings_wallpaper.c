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
#include <glib/gi18n.h>
#include <gmodule.h>
#include <side/plugin.h>

GtkWidget *win, *label, *image, *box, *path, *clear, *choose, *button_box, *apply, *notebook;
GtkWidget *mainbox;
GtkWidget *box2;

GtkListStore *list;
GtkTreeIter iter;
GtkWidget *view;


gboolean check_box_toggle_mods(GtkCellRendererToggle *renderer, gchar *path, GtkTreeView *treeview)
{
  GtkTreeModel *model;
  GtkTreeIter iter;
  gboolean state;

  model = GTK_TREE_MODEL(list);
  if(gtk_tree_model_get_iter_from_string(model, &iter, path))
    {
      gtk_tree_model_get(model, &iter, 1, &state, -1);
      gtk_list_store_set(GTK_LIST_STORE(model), &iter, 1, !state, -1);
    }
  else
      g_warning("Failed switching state of checkbox: PATH: %s", path);

  return FALSE;
}

gboolean clear_wallpaper(GtkWidget *w, GdkEvent *e, gpointer *p)
{
  gtk_label_set_text(GTK_LABEL(path), "file://");
  gtk_image_set_from_icon_name(GTK_IMAGE(image), "gtk-missing-image", GTK_ICON_SIZE_DIALOG);
  return FALSE;
}

gboolean open_wallpaper(GtkWidget *w, GdkEvent *e, gpointer *p)
{
  GtkWidget *dialog = gtk_file_chooser_dialog_new(_("Open Wallpaper"), GTK_WINDOW(win), GTK_FILE_CHOOSER_ACTION_OPEN, _("Cancel"), GTK_RESPONSE_CANCEL, _("Open"), GTK_RESPONSE_ACCEPT, NULL);

  GSettings *s = g_settings_new("org.gnome.desktop.background");
  char *t = g_settings_get_string(s, "picture-uri");

  char *n = t;
  // try to skip file://
  for(int x = 0; x < strlen(t) && x <7;x++)
    ++t;

  char *pathstr = g_path_get_dirname(t);

  if(pathstr != NULL && strlen(pathstr) >0)
  {
    gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(dialog), pathstr);
    g_free(pathstr);
    g_free(n);
  }


  GtkFileFilter *pics = gtk_file_filter_new();
  gtk_file_filter_add_pixbuf_formats(pics);
  gtk_file_chooser_set_filter(GTK_FILE_CHOOSER(dialog), pics);

  gint res = gtk_dialog_run(GTK_DIALOG(dialog));
  if(res == GTK_RESPONSE_ACCEPT)
    {
      char *f = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
      char *p = malloc(strlen(f) + 10);
      snprintf(p, strlen(f) + 10, "file://%s", f);
      gtk_label_set_text(GTK_LABEL(path), p);

      char *pic_path = strdup(p);
      pic_path = strtok(pic_path, "//");
      pic_path = strtok(NULL, "\0");
      gtk_image_set_from_pixbuf(GTK_IMAGE(image), gdk_pixbuf_scale_simple(gdk_pixbuf_new_from_file(pic_path, NULL), 450, 250, GDK_INTERP_NEAREST));

      free(p);
    }

  gtk_widget_destroy (dialog);
  return FALSE;
}

gboolean write_wallpaper_settings(GtkWidget *w, GdkEvent *e, gpointer *p)
{
  GSettings *wp = g_settings_new("org.gnome.desktop.background");
  g_settings_set_value(wp, "picture-uri", g_variant_new_string(gtk_label_get_text(GTK_LABEL(path))));


  int x = 0;
  char *str = malloc(2);
  memset(str, 0, sizeof(str));
  GtkTreeModel *model = gtk_tree_view_get_model(GTK_TREE_VIEW(view));
  char i_b[5];
  snprintf(i_b, 5, "%d", x);
  while(gtk_tree_model_get_iter_from_string(model, &iter, i_b))
    {
      char *buff1 = NULL;
      gboolean state = FALSE;
      gtk_tree_model_get(model, &iter, 0, &buff1, -1);
      gtk_tree_model_get(model, &iter, 1, &state, -1);
      char buffer[1000];
      if(!state)
        {
          snprintf(buffer, 1000, "%s;", buff1);
          str = realloc(str, sizeof(str) + sizeof(buffer));
          strcat(str, buffer);
        }
        x++;
        snprintf(i_b, 5, "%d", x);
    }

  GSettings *plugins = g_settings_new("org.jetspace.desktop.wallpaper");
  g_settings_set_string(plugins, "ignored-plugins", str);
  g_settings_sync();

  return FALSE;
}

GtkWidget *build_wallpaper_settings(void)
{
    notebook = gtk_notebook_new();
    box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);

    label = gtk_label_new(_("On This page you can setup your Wallpaper:"));

    //Get Current Wallpaper
    GSettings *gnome_conf;
    gnome_conf = g_settings_new("org.gnome.desktop.background");
    const char *ptr = g_variant_get_string(g_settings_get_value(gnome_conf, "picture-uri"), NULL);
    g_debug("Loading Wallpaper %s", ptr);

    //Setup GtkImage
    char *pic_path = strdup(ptr);
    pic_path = strtok(pic_path, "//");
    pic_path = strtok(NULL, "\0");
    image = gtk_image_new_from_file(pic_path);
    gtk_image_set_from_pixbuf(GTK_IMAGE(image), gdk_pixbuf_scale_simple(gtk_image_get_pixbuf(GTK_IMAGE(image)), 450, 250, GDK_INTERP_NEAREST));

    button_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 2);
    gtk_box_set_homogeneous(GTK_BOX(button_box), TRUE);

    clear  = gtk_button_new_with_label(_("Clear"));
    gtk_container_add(GTK_CONTAINER(button_box), clear);
    g_signal_connect(G_OBJECT(clear), "button_press_event", G_CALLBACK(clear_wallpaper), NULL);

    choose = gtk_button_new_with_label(_("Open"));
    gtk_container_add(GTK_CONTAINER(button_box), choose);
    g_signal_connect(G_OBJECT(choose), "button_press_event", G_CALLBACK(open_wallpaper), NULL);

    path = gtk_label_new(ptr);

    apply = gtk_button_new_with_label(_("Apply"));
    g_signal_connect(G_OBJECT(apply), "button_press_event", G_CALLBACK(write_wallpaper_settings), NULL);

    gtk_box_pack_start(GTK_BOX(box), label, FALSE, FALSE, 10);
    gtk_box_pack_start(GTK_BOX(box), image, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(box), path, FALSE, FALSE, 0);
    gtk_box_pack_end(GTK_BOX(box), button_box, FALSE, TRUE, 0);

    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), box, gtk_label_new(_("Wallpaper")));


    //PLUGIN Page
    box2 = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), box2, gtk_label_new(_("Plugins")));

    list = gtk_list_store_new(2, G_TYPE_STRING, G_TYPE_BOOLEAN);

    GSettings *plugins = g_settings_new("org.jetspace.desktop.wallpaper");

    DIR *d = opendir("/usr/lib/jetspace/wallpaper/plugins/");
    if(d == NULL)
     {
       g_error("MODULE PATH NOT FOUND");
     }


    char *mods_buff = strdup(g_variant_get_string(g_settings_get_value(plugins, "ignored-plugins"), NULL));
    char **mods = malloc(sizeof(mods_buff));
    char *mp = strtok(mods_buff, ";");
    int x = 0;
    while(mp != NULL)
     {
       mods[x] = strdup(mp);
       x++;
       mp = strtok(NULL, ";");
     }
    struct dirent *de;
    while((de = readdir(d)) != NULL)
     {
       gboolean c = FALSE;

       if(de->d_name[0] == '.')
           continue;

       if(is_listed(mods, x, de->d_name) == TRUE)
         c = FALSE;
       else
         c = TRUE;

       gtk_list_store_append(list, &iter);
       gtk_list_store_set(list, &iter, 0, de->d_name, 1, c, -1);
     }
    closedir(d);


     view = gtk_tree_view_new();
     gtk_tree_view_set_model(GTK_TREE_VIEW(view), GTK_TREE_MODEL(list));
     GtkWidget *scroll_win = gtk_scrolled_window_new(NULL, NULL);
     gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll_win), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
     gtk_scrolled_window_set_min_content_height(GTK_SCROLLED_WINDOW(scroll_win), 200);


     GtkCellRenderer *renderer = gtk_cell_renderer_text_new();
     GtkTreeViewColumn *column = gtk_tree_view_column_new_with_attributes(_("Plugin"), renderer, "text", 0, NULL);
     gtk_tree_view_append_column(GTK_TREE_VIEW(view), column);

     renderer = gtk_cell_renderer_toggle_new();
     column = gtk_tree_view_column_new_with_attributes(_("Is Active"), renderer, "active", 1, NULL);
     g_signal_connect(G_OBJECT(renderer), "toggled", G_CALLBACK(check_box_toggle_mods), (gpointer) view);
     gtk_tree_view_append_column(GTK_TREE_VIEW(view), column);

     gtk_container_add(GTK_CONTAINER(scroll_win), view);
     gtk_box_pack_end(GTK_BOX(box2), scroll_win, TRUE, TRUE, 10);


    mainbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_box_pack_start(GTK_BOX(mainbox), notebook, TRUE, TRUE, 0);



    gtk_box_pack_end(GTK_BOX(mainbox), apply, FALSE, TRUE, 0);
    return mainbox;
}


void callback(gpointer d)
{
  GtkBox *container = GTK_BOX(d);
  GtkWidget *cont = build_wallpaper_settings();
  gtk_box_pack_start(container, cont, TRUE, TRUE, 0);
}

SiDESettingsPluginDescription side_wallpaper_plugin_desc;

SiDESettingsPluginDescription *identify(gpointer data)
{
  side_wallpaper_plugin_desc.label = _("Wallpaper");
  side_wallpaper_plugin_desc.hover = _("Setup background of your environment");
  side_wallpaper_plugin_desc.icon  = "preferences-desktop-wallpaper";
  side_wallpaper_plugin_desc.title = _("Wallpaper");
  side_wallpaper_plugin_desc.cmd = "settings.wallpaper";
  side_wallpaper_plugin_desc.category = SIDE_SETTINGS_CATEGORY_APPERANCE;
  return &side_wallpaper_plugin_desc;
}
