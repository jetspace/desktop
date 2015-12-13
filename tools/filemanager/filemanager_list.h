#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <side/timelabel.h>

void add_item_to_list(SiDEFilesProto *sf, char *name, GIcon *icon, const char *type, time_t changed)
{

  GtkWidget *box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
  GtkWidget *pic = gtk_image_new_from_gicon(icon, GTK_ICON_SIZE_DND);

  GtkWidget *change = side_timelabel_new(changed);

  if(strcmp("inode/directory", type) == 0)
    gtk_widget_set_name(box, "dir");

  GtkWidget *lab = gtk_label_new(name);
  gtk_label_set_ellipsize(GTK_LABEL(lab),PANGO_ELLIPSIZE_END);
  gtk_widget_set_name(lab, "name");

  gtk_box_pack_start(GTK_BOX(box), pic, FALSE, FALSE,10);
  gtk_box_pack_start(GTK_BOX(box), lab, FALSE, FALSE,0);

  gtk_box_pack_end(GTK_BOX(box), change, FALSE, FALSE,10);


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
  clear_list(sf);
  DIR *d = opendir(sf->path);

  if(d == NULL)
  {
    g_warning("INVALID PATH: %s", sf->path);
    toggle_entry_icon(sf, ICON_INVALID);
    return;
  }

  toggle_entry_icon(sf, ICON_OK);
  struct dirent *ent;

  while((ent = readdir(d)) != NULL)
  {
    if(ent->d_name[0] == '.')
      continue;

    if(gtk_revealer_get_reveal_child(GTK_REVEALER(sf->show_search)) == TRUE)
    {// We have searching enabled, do aditional filtering...
      const char *querry = gtk_entry_get_text(GTK_ENTRY(sf->search));
      char *origin = g_strdup(ent->d_name);
      for(int x = 0; x < strlen(origin); x++)
        origin[x] = tolower(origin[x]);

      char *q = g_strdup(querry);
      for(int x = 0; x < strlen(q); x++)
        q[x] = tolower(q[x]);

      if(strstr(origin, q) == NULL)
      {
        g_free(q);
        g_free(origin);
        continue;
      }
      g_free(q);
      g_free(origin);
    }

    char *path = g_strdup_printf("%s%s", sf->path, ent->d_name);
    GFile *gf = g_file_new_for_path(path);

    GFileInfo *info;
    info = g_file_query_info(gf, "standard::*", G_FILE_QUERY_INFO_NONE, NULL, NULL);
    GIcon *icon = g_file_info_get_icon(info);
    const char *content_type = g_file_info_get_content_type(info);

    struct stat attr;
    stat(path, &attr);
    free(path);
    add_item_to_list(sf, ent->d_name, icon, content_type, attr.st_ctime);
  }
  gtk_widget_show_all(sf->listbox);
  closedir(d);

  GtkWidget *scroll = gtk_scrolled_window_get_vscrollbar(GTK_SCROLLED_WINDOW(sf->scroll));
  gtk_range_set_value(GTK_RANGE(scroll), 0);
}
