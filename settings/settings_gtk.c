/*
This file is licensed under the MIT-License
Copyright (c) 2015 Marius Messerschmidt
For more details view file 'LICENSE'
*/

#include "../shared/strdup.h"
#include <gtk/gtk.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <errno.h>
#include <glib/gi18n.h>
#include <side/widgets.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <gmodule.h>
#include <side/plugin.h>

typedef struct {
  GtkWidget *gtk3;
  GtkWidget *gtk2;
}SiDEGTKSettingsData;

SiDEGTKSettingsData data;

SiDESettingsPluginDescription side_gtk_plugin_desc;
void callback(gpointer data);


gboolean destroy(GtkWidget *w, GdkEvent *e, gpointer *p)
{
  gtk_main_quit();
  return FALSE;
}

static gboolean apply_clicked(GtkWidget *w, GdkEvent *e, GtkWidget *cb)
{
  GtkTreeIter iter;
  gtk_combo_box_get_active_iter(GTK_COMBO_BOX(data.gtk3), &iter);

  GtkTreeModel *list = gtk_combo_box_get_model(GTK_COMBO_BOX(data.gtk3));

  char *theme;
  gtk_tree_model_get(list, &iter, 0, &theme, -1);

  if(!theme)
  {
    return FALSE;
  }

  char *buffer = malloc(strlen(getenv("HOME")) + strlen("/.config/gtk-3.0/settings.ini"));
  strcpy(buffer, getenv("HOME"));
  strcat(buffer, "/.config/gtk-3.0/settings.ini");

  FILE *in = fopen(buffer, "r");
  FILE *out = fopen("/tmp/gtk_theme_conf_buff.temp", "w");

  char buff[2000];
  while(fgets(buff, 2000, in) != NULL)
  {
    if(strncmp(buff, "gtk-theme-name", 14) == 0)
      fprintf(out, "gtk-theme-name=%s\n", theme);
    else
      fprintf(out, "%s", buff);
  }

  fclose(in);
  fclose(out);

  rename("/tmp/gtk_theme_conf_buff.temp", buffer);

  free(buffer);



  gtk_combo_box_get_active_iter(GTK_COMBO_BOX(data.gtk2), &iter);

  list = gtk_combo_box_get_model(GTK_COMBO_BOX(data.gtk2));

  gtk_tree_model_get(list, &iter, 0, &theme, -1);

  buffer = malloc(strlen(getenv("HOME")) + strlen(".gtkrc-2.0"));
  strcpy(buffer, getenv("HOME"));
  strcat(buffer, "/.gtkrc-2.0");

  out = fopen(buffer, "w");
  fprintf(out , "# ---     WRITTEN BY SIDE-GTK-SETTINGS     --- \n\n");
  fprintf(out , "include \"/usr/share/themes/%s/gtk-2.0/gtkrc\"\n\n", theme);
  fprintf(out , "# -WARNING: GTK2 is old, use GTK3 if possible!- \n");
  fclose(out);




  return FALSE;
}


int counter = 0;
int scan_dir_for_themes(GtkListStore *list, GtkTreeIter iter, char *path, char *target, char *themes)
{
  int ret = -1;
  DIR *d = opendir(path);
  struct dirent *e;
  struct stat st;


  while((e = readdir(d)) != NULL)
    {

      stat(e->d_name, &st);
      if(S_ISDIR(st.st_mode) && e->d_name[0] != '.')//Switch to subdir
         {
           char *buffer = malloc(strlen(path) + strlen(e->d_name) +2);
           char *theme = strdup(e->d_name); //backup theme name if we have a match later
           snprintf(buffer, strlen(path) +1 + strlen(e->d_name), "%s%s",path, e->d_name);
           DIR *sub = opendir(buffer);
           g_debug("Found new GTK theme in %s", buffer);
           free(buffer);
           if(!sub)
               continue;
           while((e = readdir(sub)) != NULL)
            {
              if(strncmp(e->d_name, themes, 7) == 0) //contains a GTK3 theme
                {

                  gtk_list_store_append(list, &iter);
                  gtk_list_store_set(list, &iter, 0, theme, -1);
                  if(strcmp(theme, target) == 0)
                    ret = counter;

                  counter ++;
                }
            }
            closedir(sub);

          free(theme);

         }
      else
         continue;
    }
    closedir(d);

    return ret;
}

char *get_current_theme()
{
  char *buffer = malloc(strlen(getenv("HOME")) + strlen("/.config/gtk-3.0/settings.ini"));
  strcpy(buffer, getenv("HOME"));
  strcat(buffer, "/.config/gtk-3.0/settings.ini");

  FILE *in = fopen(buffer, "r");

  char buff[2000];
  while(fgets(buff, 2000, in) != NULL)
  {
    if(strncmp(buff, "gtk-theme-name", 14) == 0)
    {
      char *ret;
      strtok(buff, "=");
      char *p = strtok(NULL, "\n\0");
      ret = malloc(strlen(p) +1);
      strncpy(ret, p, strlen(p) +1);

      free(buffer);
      fclose(in);
      return ret;
    }
  }

  free(buffer);

}
char *get_current_theme2()
{
  char *buffer = malloc(strlen(getenv("HOME")) + strlen(".gtkrc-2.0"));
  strcpy(buffer, getenv("HOME"));
  strcat(buffer, "/.gtkrc-2.0");
  FILE *in = fopen(buffer, "r");

  char buff[2000];
  while(fgets(buff, 2000, in) != NULL)
  {
    if(strncmp(buff, "include", 7) == 0)
    {
      char *ret;

      strtok(buff, "m");
      strtok(NULL, "/");
      char *p = strtok(NULL, "/");
      ret = malloc(strlen(p) +1);
      strncpy(ret, p, strlen(p) +1);

      fclose(in);
      free(buffer);
      return ret;
    }
  }

  free(buffer);

}

GtkWidget *build_gtk_settings(void)
{
  GtkWidget *box, *label, *apply, *cb;
  box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
  gtk_container_set_border_width(GTK_CONTAINER(box), 25);

  label = side_gtk_label_new(_("Select your GTK 3.0 theme:"));
  gtk_box_pack_start(GTK_BOX(box), label, FALSE, FALSE, 0);

  cb = gtk_combo_box_new();
  gtk_box_pack_start(GTK_BOX(box), cb, FALSE, FALSE, 0);

  GtkListStore *list;
  GtkTreeIter iter;

  list = gtk_list_store_new(1, G_TYPE_STRING, -1);
  GtkTreeModel *sorted_list;

  sorted_list = gtk_tree_model_sort_new_with_model(GTK_TREE_MODEL(list));
  gtk_tree_sortable_set_sort_column_id(GTK_TREE_SORTABLE(sorted_list), 0, GTK_SORT_ASCENDING);



  //setup GTK3 themes
  int target_pos = -1;
  char *theme = get_current_theme();

  //GLOBAL
  target_pos = scan_dir_for_themes(list, iter, "/usr/share/themes/", theme, "gtk-3.0");

  //USER
  char userthemes[2000];
  memset(userthemes, 0, 2000);
  struct passwd *pw = getpwuid(getuid());
  const char *homedir = pw->pw_dir;

  strcat(userthemes, homedir);
  strcat(userthemes, "/.themes/");

  if(target_pos == -1)
    target_pos = scan_dir_for_themes(list, iter, userthemes, theme, "gtk-3.0");
  else
    scan_dir_for_themes(list, iter, userthemes, theme, "gtk-3.0");

  gtk_combo_box_set_model(GTK_COMBO_BOX(cb), GTK_TREE_MODEL(sorted_list));

  char *tgt = g_strdup_printf("%d", target_pos);

  gtk_tree_model_get_iter_from_string(GTK_TREE_MODEL(list), &iter, tgt);

  g_free(tgt);

  GtkTreeIter si;
  gtk_tree_model_sort_convert_child_iter_to_iter(GTK_TREE_MODEL_SORT(sorted_list), &si, &iter);

  gtk_combo_box_set_active_iter(GTK_COMBO_BOX(cb), &si);




  GtkCellRenderer *render = gtk_cell_renderer_text_new();
  gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(cb), render, TRUE);

  gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(cb), render,"text", 0,NULL);
  data.gtk3 = cb;




  // GTK2 Themes
  counter = 0;

  GtkWidget *advanced = gtk_expander_new(_("Advanced"));
  GtkWidget *subbox   = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);




  label = side_gtk_label_new(_("Select your GTK 2.0 theme:"));
  gtk_box_pack_start(GTK_BOX(subbox), label, FALSE, FALSE, 0);

  cb = gtk_combo_box_new();
  gtk_box_pack_start(GTK_BOX(subbox), cb, FALSE, FALSE, 0);

  gtk_container_add(GTK_CONTAINER(advanced), subbox);
  gtk_box_pack_start(GTK_BOX(box), advanced, TRUE, TRUE, 0);



  list = gtk_list_store_new(1, G_TYPE_STRING, -1);

  sorted_list = gtk_tree_model_sort_new_with_model(GTK_TREE_MODEL(list));
  gtk_tree_sortable_set_sort_column_id(GTK_TREE_SORTABLE(sorted_list), 0, GTK_SORT_ASCENDING);



  //setup GTK2 themes
  target_pos = -1;
  theme = get_current_theme2();

  //GLOBAL
  target_pos = scan_dir_for_themes(list, iter, "/usr/share/themes/", theme, "gtk-2.0");

  if(target_pos == -1)
    target_pos = scan_dir_for_themes(list, iter, userthemes, theme, "gtk-2.0");
  else
    scan_dir_for_themes(list, iter, userthemes, theme, "gtk-2.0");

  gtk_combo_box_set_model(GTK_COMBO_BOX(cb), GTK_TREE_MODEL(sorted_list));

  tgt = g_strdup_printf("%d", target_pos);

  gtk_tree_model_get_iter_from_string(GTK_TREE_MODEL(list), &iter, tgt);

  g_free(tgt);

  gtk_tree_model_sort_convert_child_iter_to_iter(GTK_TREE_MODEL_SORT(sorted_list), &si, &iter);

  gtk_combo_box_set_active_iter(GTK_COMBO_BOX(cb), &si);




  render = gtk_cell_renderer_text_new();
  gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(cb), render, TRUE);

  gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(cb), render,"text", 0,NULL);
  data.gtk2 = cb;



  apply = gtk_button_new_with_label(_("Apply"));
  g_signal_connect(G_OBJECT(apply), "button_press_event", G_CALLBACK(apply_clicked), NULL);
  gtk_box_pack_end(GTK_BOX(box), apply, FALSE, FALSE, 5);
  return box;
}

void callback(gpointer d)
{
  counter = 0;
  GtkBox *container = GTK_BOX(d);
  GtkWidget *cont = build_gtk_settings();
  gtk_box_pack_start(container, cont, TRUE, TRUE, 0);
}

SiDESettingsPluginDescription *identify(gpointer data)
{
  side_gtk_plugin_desc.label = _("GTK Theme");
  side_gtk_plugin_desc.hover = _("Setup GTK Themes");
  side_gtk_plugin_desc.icon  = "preferences-desktop-theme";
  side_gtk_plugin_desc.title = _("GTK Theme");
  side_gtk_plugin_desc.cmd = "settings.gtk";
  side_gtk_plugin_desc.category = SIDE_SETTINGS_CATEGORY_APPERANCE;
  return &side_gtk_plugin_desc;
}
