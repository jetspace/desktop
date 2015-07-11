/*
This file is licensed under the MIT-License
Copyright (c) 2015 Marius Messerschmidt
For more details view file 'LICENSE'
*/
#include "../../shared/info.h"
#include <stdlib.h>
#include <limits.h>
gboolean destroy_cb(GtkWidget *w, GdkEvent *e, gpointer p)
{
  gtk_main_quit();
  return FALSE;
}

gboolean show_about(GtkWidget *w, GdkEvent *e, gpointer p)
{
    GtkWidget *dialog = gtk_about_dialog_new();
    gtk_about_dialog_set_program_name(GTK_ABOUT_DIALOG(dialog), _("SiDE File Manager"));
    gtk_about_dialog_set_version(GTK_ABOUT_DIALOG(dialog), VERSION);
    gtk_about_dialog_set_copyright(GTK_ABOUT_DIALOG(dialog), COPYRIGHT);
    gtk_about_dialog_set_comments(GTK_ABOUT_DIALOG(dialog), FILE_MANAGER_DISCRIPTION);
    gtk_about_dialog_set_license_type(GTK_ABOUT_DIALOG(dialog), GTK_LICENSE_MIT_X11);
    gtk_about_dialog_set_authors(GTK_ABOUT_DIALOG(dialog), AUTHORS);
    gtk_about_dialog_set_logo(GTK_ABOUT_DIALOG(dialog), gdk_pixbuf_scale_simple(gdk_pixbuf_new_from_file("/usr/share/icons/jetspace/JetSpace.png", NULL), 100, 100, GDK_INTERP_BILINEAR));
    gtk_about_dialog_set_website(GTK_ABOUT_DIALOG(dialog), "http://jetspace.tk");
    gtk_about_dialog_set_artists(GTK_ABOUT_DIALOG(dialog), ARTISTS);
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
    return FALSE;
}
gboolean show_help(GtkWidget *w, GdkEvent *e, gpointer p)
{
    gtk_show_uri(gdk_screen_get_default (), "http://side.rtfd.org", GDK_CURRENT_TIME, NULL);
    return FALSE;
}

/*

  0 -From sidebar
  1 - From click
  2 - From pathbar

*/
void update_files(int get_path, char *name)
{

  if(get_path == 0)
  {
    if(g_file_get_path(gtk_places_sidebar_get_location(GTK_PLACES_SIDEBAR(places_bar))) == NULL)
      return;
    if(path)
      g_free(path);
    path = g_file_get_path(gtk_places_sidebar_get_location(GTK_PLACES_SIDEBAR(places_bar)));
  }
  else if(get_path == 1)
  {
    if(!path)
      path = g_strdup_printf("%s", name);
    else
      path = g_strdup_printf("%s/%s", path, name);
  }

  if(!path)
    return;
  else if(access(g_strdup_printf("%s/", path), F_OK) != 0 && get_path == 2)
  {
    return;
  }

  gtk_entry_set_text(GTK_ENTRY(path_entry), path);

  chdir(path);

  gtk_list_store_clear(files); // clear all old files
  DIR *d = opendir(path);

  //fill liststore
  struct dirent *de;
  do {
    de = readdir(d);

    //end of dir/false trash
    if(de == NULL)
      break;
    else if(de->d_name[0] == '.' && strlen(de->d_name) == 1)
      continue;
    else if(de->d_name[0] == '.' && strlen(de->d_name) == 2 && de->d_name[1] == '.')
      continue;

    GtkIconInfo *icon_info = gtk_icon_theme_lookup_by_gicon (gtk_icon_theme_get_default (), g_content_type_get_icon(g_content_type_guess(de->d_name, NULL, 0, NULL)), 48, GTK_ICON_LOOKUP_GENERIC_FALLBACK);

    GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file(gtk_icon_info_get_filename (icon_info), NULL);

    gboolean isDir = TRUE;;
    char *buf = g_strdup_printf("%s/%s", path, de->d_name);
    DIR *b = opendir(buf);
    if(b == NULL)
      isDir = FALSE;
    else
      closedir(b);

    g_free(buf);

    char *bu = malloc(strlen(de->d_name) +1);
    strncpy(bu, de->d_name, strlen(de->d_name)+1);

    if(strlen(de->d_name) > 20)//truncate after 20 chars...
    {
      de->d_name[17] = '.';
      de->d_name[18] = '.';
      de->d_name[19] = '.';
      de->d_name[20] = 0;
    }

    gtk_list_store_append(files , &iter);

    if(isDir)
    {
      gtk_list_store_set(files, &iter, COL_NAME, de->d_name, COL_ICON, gtk_icon_theme_load_icon(gtk_icon_theme_get_default(), "folder", 48,GTK_ICON_LOOKUP_GENERIC_FALLBACK ,NULL), COL_DIR, TRUE, COL_FULL_NAME, bu, -1);
    }
    else
    {
      gtk_list_store_set(files, &iter, COL_NAME, de->d_name, COL_ICON, pixbuf, COL_DIR, FALSE, COL_FULL_NAME, bu, -1);
    }
    g_object_unref(pixbuf);

    free(bu);
  } while(de != NULL);


  closedir(d);
}

void open_location_cb (GtkPlacesSidebar  *sidebar, GObject *location,GtkPlacesOpenFlags open_flags, gpointer user_data)
{
  if(location == NULL)
    return;
  update_files(0, NULL);
}

gboolean activated_file(GtkIconView *fileview, gpointer *data)
{
  GList *list = gtk_icon_view_get_selected_items(fileview);
  GList *it;

  for(it = list; it != NULL; it = g_list_next(it))
    {
      if(gtk_icon_view_path_is_selected(fileview, it->data))
        {
          GtkTreeIter i;
          GtkTreePath *p =  gtk_tree_model_sort_convert_path_to_child_path(GTK_TREE_MODEL_SORT(sorted_files), it->data);
          GtkTreeModel *model = GTK_TREE_MODEL(files);

          gboolean dir;
          gchar *name;
          gtk_tree_model_get_iter(model, &i, p);
          gtk_tree_model_get(model, &i, COL_DIR, &dir, COL_FULL_NAME, &name, -1);

          if(dir)
          {
            update_files(1, name);
            return FALSE;
          }
          else
          {
            //get mime
            GFile *gf = g_file_new_for_path(g_strdup_printf("%s/%s", path, name));
            GFileInfo *fi = g_file_query_info(gf, "standard::*", G_FILE_QUERY_INFO_NONE ,NULL, NULL);
            char *mime = g_content_type_get_mime_type(g_file_info_get_content_type(fi));

            //TODO: Improve file opening...
            system(g_strdup_printf("xdg-open \"%s/%s\" &", path, name));

            g_object_unref(gf);
            g_object_unref(fi);
            g_free(mime);

          }


        }
    }
  return FALSE;
}


gboolean update_from_pathbar(GtkWidget *widget, GdkEventKey *event, gpointer data)
{
  if(event->keyval == GDK_KEY_Return)
  {
    path = g_strdup(gtk_entry_get_text(GTK_ENTRY(path_entry)));
    update_files(2, NULL);
  }


  return FALSE;
}

gboolean go_up(GtkWidget *widget, GdkEventKey *event, gpointer data)
{
  int n = 0,x = 0;

  for(unsigned int y = 0; y < strlen(path); y++)
  {
    if(path[y] == '/')
    {
      n++;
      x = y;
    }
  }

  if(n == 0 || strlen(path) == 1) // ignore /
    return FALSE;

  if(n == 1)
    path[x+1] = 0;
  else
    path[x] = 0;
  update_files(2, NULL);

  return FALSE;
}
