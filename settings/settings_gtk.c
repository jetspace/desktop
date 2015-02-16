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

GtkWidget *win, *box, *label, *label2, *cb, *cb2,  *apply;

gboolean write_gtk_settings(GtkWidget *widget, GdkEvent  *event, gpointer user_data)
{
  char *buffer = malloc(strlen(getenv("HOME")) + strlen(".gtkrc-2.0"));
  strcpy(buffer, getenv("HOME"));
  strcat(buffer, "/.gtkrc-2.0");

  FILE *f;
  if((f = fopen(buffer, "w")) == NULL)
    {
      GtkWidget *d = gtk_message_dialog_new (GTK_WINDOW(win),
                        GTK_DIALOG_MODAL,
                        GTK_MESSAGE_ERROR,
                        GTK_BUTTONS_CLOSE,
                        "Failed to write GTK2 settings: \n %s \n %s",
                        strerror(errno), buffer);
      gtk_dialog_run(GTK_DIALOG(d));
      gtk_widget_destroy(d);
      return FALSE;

    }
  fprintf(f , "# --- WRITTEN BY SIDE-GTK-SETTINGS --- \n\n");
  fprintf(f , "include \"/usr/share/themes/%s/gtk-2.0/gtkrc\"\n\n", gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(cb)));
  fprintf(f , "# ------------------------------------ \n");
  fclose(f);

  buffer = malloc(strlen(getenv("HOME")) + strlen("/.config/gtk-3.0/settings.ini"));
  strcpy(buffer, getenv("HOME"));
  strcat(buffer, "/.config/gtk-3.0/settings.ini");

  if((f = fopen(buffer, "w")) == NULL)
    {
      GtkWidget *d = gtk_message_dialog_new (GTK_WINDOW(win),
                        GTK_DIALOG_MODAL,
                        GTK_MESSAGE_ERROR,
                        GTK_BUTTONS_CLOSE,
                        "Failed to write GTK3 settings: \n %s \n %s",
                        strerror(errno), buffer);
      gtk_dialog_run(GTK_DIALOG(d));
      gtk_widget_destroy(d);
      return FALSE;

    }
  fprintf(f , "[Settings]\n");
  fprintf(f , "gtk-theme-name=%s\n", gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(cb2)));
  fprintf(f , "gtk-application-prefer-dark-theme=0\n");
  fclose(f);


  return FALSE;
}

gboolean destroy(GtkWidget *w, GdkEvent *e, gpointer *p)
{
  gtk_main_quit();
  return FALSE;
}

int main(int argc, char **argv)
{
  gtk_init(&argc, &argv);

  win = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_resize(GTK_WINDOW(win), 400, 300);
  gtk_window_set_title(GTK_WINDOW(win), "Settings - GTK");
  gtk_container_set_border_width(GTK_CONTAINER(win), 10);
  g_signal_connect(G_OBJECT(win), "delete-event", G_CALLBACK(destroy), NULL);

  box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);

  label = gtk_label_new("Select your GTK 2.0 theme:");
  gtk_container_add(GTK_CONTAINER(box), label);

  cb = gtk_combo_box_text_new();
  gtk_container_add(GTK_CONTAINER(box), cb);

  label2 = gtk_label_new("Select your GTK 3.0 theme:");
  gtk_container_add(GTK_CONTAINER(box), label2);

  cb2 = gtk_combo_box_text_new();
  gtk_container_add(GTK_CONTAINER(box), cb2);

  DIR *d = opendir("/usr/share/themes");
  struct dirent *e;
  struct stat st;

  //setup GTK2 themes
  while((e = readdir(d)) != NULL)
    {
      stat(e->d_name, &st);
      if(S_ISDIR(st.st_mode) && e->d_name[0] != '.')//Switch to subdir
         {
           char *buffer = malloc(strlen("/usr/share/themes/ ") + strlen(e->d_name));
           char *theme = strdup(e->d_name); //backup theme name if we have a match later
           strcpy(buffer, "/usr/share/themes/");
           strcat(buffer, e->d_name);
           DIR *sub = opendir(buffer);
           free(buffer);
           while((e = readdir(sub)) != NULL)
            {
              if(strncmp(e->d_name, "gtk-2.0", 7) == 0) //contains a GTK2 theme
                {
                  gtk_combo_box_text_prepend_text(GTK_COMBO_BOX_TEXT(cb), theme);
                }
              if(strncmp(e->d_name, "gtk-3.0", 7) == 0) //contains a GTK3 theme
                {
                  gtk_combo_box_text_prepend_text(GTK_COMBO_BOX_TEXT(cb2), theme);
                }
            }


          free(theme);

         }
      else
         continue;
    }

  apply = gtk_button_new_with_label("Apply");
  g_signal_connect(G_OBJECT(apply), "button_press_event", G_CALLBACK(write_gtk_settings), NULL);
  gtk_box_pack_end(GTK_BOX(box), apply, FALSE, FALSE, 5);


  gtk_container_add(GTK_CONTAINER(win), box);
  gtk_widget_show_all(win);
  gtk_main();
  return 0;
}
