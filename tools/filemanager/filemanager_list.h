#include <ctype.h>

void add_item_to_list(SiDEFilesProto *sf, char *name, GIcon *icon, const char *type)
{

  GtkWidget *box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
  GtkWidget *pic = gtk_image_new_from_gicon(icon, GTK_ICON_SIZE_DND);

  if(strcmp("inode/directory", type) == 0)
    gtk_widget_set_name(box, "dir");

  GtkWidget *lab = gtk_label_new(name);
  gtk_widget_set_name(lab, "name");

  gtk_box_pack_start(GTK_BOX(box), pic, FALSE, FALSE,10);
  gtk_box_pack_start(GTK_BOX(box), lab, FALSE, FALSE,0);


  GtkWidget *item = gtk_list_box_row_new();
  gtk_list_box_row_set_selectable(GTK_LIST_BOX_ROW(item), TRUE);
  gtk_container_add(GTK_CONTAINER(item), box);

  gtk_list_box_insert(GTK_LIST_BOX(sf->listbox), item, -1);
  gtk_widget_show_all(box);
}

gint filelist_sort (GtkListBoxRow *row1, GtkListBoxRow *row2, gpointer data)
{
  SiDEFilesProto *sf = data;
  GtkLabel *name1;
  GtkLabel *name2;

  const char *b1, *b2;

  GList *ch, *iter;
  GtkWidget *box = gtk_bin_get_child(GTK_BIN(row1));
  b1 = gtk_widget_get_name(box);
  ch = gtk_container_get_children(GTK_CONTAINER(box));
  for(iter = ch; iter != NULL; iter = g_list_next(iter))
  {
          if(strcmp(gtk_widget_get_name(iter->data), "name") == 0)
          {
            name1 = iter->data;
            break;
          }
  }

  box = gtk_bin_get_child(GTK_BIN(row2));
  b2 = gtk_widget_get_name(box);
  ch = gtk_container_get_children(GTK_CONTAINER(box));
  for(iter = ch; iter != NULL; iter = g_list_next(iter))
  {
          if(strcmp(gtk_widget_get_name(iter->data), "name") == 0)
          {
            name2 = iter->data;
            break;
          }
  }

  if(strcmp(b1, "dir") == 0)
  {
    if(strcmp(b2, "dir") != 0)
      return -1;
  }
  else if(strcmp(b2, "dir") == 0)
    return 1;

  char *n1, *n2;

  n1 = g_strdup(gtk_label_get_text(name1));
  for(int x = 0; x < strlen(n1); x++)
    n1[x] = tolower(n1[x]);

  n2 = g_strdup(gtk_label_get_text(name2));
  for(int x = 0; x < strlen(n2); x++)
    n2[x] = tolower(n2[x]);

  int n;

  switch(sf->sort_by)
  {
      case SORT_BY_NAME:
      n = strcmp(n1, n2);
      break;
  }

  g_free(n1);
  g_free(n2);

  return n;


}

void clear_list(SiDEFilesProto *sf)
{
  GList *ch, *iter;
  ch = gtk_container_get_children(GTK_CONTAINER(sf->listbox));
  for(iter = ch; iter != NULL; iter = g_list_next(iter))
  {
      gtk_widget_destroy(iter->data);
  }
}

void reload_files(SiDEFilesProto *sf)
{
  DIR *d = opendir(sf->path);

  if(d == NULL)
  {
    g_warning("INVALID PATH: %s", sf->path);
    return;
  }

  struct dirent *ent;
  clear_list(sf);

  while((ent = readdir(d)) != NULL)
  {
    if(ent->d_name[0] == '.')
      continue;

    char *path = g_strdup_printf("%s%s", sf->path, ent->d_name);
    GFile *gf = g_file_new_for_path(path);
    free(path);

    GFileInfo *info;
    info = g_file_query_info(gf, "standard::*", G_FILE_QUERY_INFO_NONE, NULL, NULL);
    GIcon *icon = g_file_info_get_icon(info);
    const char *content_type = g_file_info_get_content_type(info);
    add_item_to_list(sf, ent->d_name, icon, content_type);
  }
  gtk_widget_show_all(sf->listbox);
  closedir(d);
}
