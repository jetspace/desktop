/*
This file is licensed under the MIT-License
Copyright (c) 2015 Marius Messerschmidt
For more details view file 'LICENSE'
*/

#ifndef _HAVE_RUN_DIALOG
#define _HAVE_RUN_DIALOG

#include <gtk/gtk.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

GtkWidget *dialog, *box, *run_panel, *label, *path, *icon, *button;

gboolean execute(GtkWidget *widget, GdkEvent *event, GtkWidget *data)
{

  const char *ptr = gtk_entry_get_text(GTK_ENTRY(path));

  if(strcmp(ptr, "r") == 0)
  {
    system("side-session --restart");
    gtk_main_quit();
  }

  char *dup = strdup(ptr);
  char cmd[strlen(ptr) + 5];
  snprintf(cmd, strlen(ptr) +5,  "%s &", dup); //atach a & to run it independant!
  int status = system(cmd);
  free(dup);
  gtk_widget_destroy(dialog);
  return FALSE;
}

gboolean path_key(GtkWidget *widget, GdkEventKey *event, gpointer data)
{//Execute when hit [ENTER]
  if(event->keyval == GDK_KEY_Return)
    execute(NULL, NULL, NULL); //it gets its data from somewere else ;-)

  return FALSE;


}

//will show an run dialog
//app_call is the calling app -> will be parsed somedays....
void run_dialog (gchar *app_call)
{


  dialog = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title(GTK_WINDOW(dialog), _("Run"));
  gtk_window_set_resizable(GTK_WINDOW(dialog), FALSE);
  gtk_window_set_icon_name(GTK_WINDOW(dialog), "system-run");
  gtk_container_set_border_width(GTK_CONTAINER(dialog), 15);

  label = gtk_label_new(_("Enter a Programm to run:"));

  path = gtk_entry_new();

  icon = gtk_image_new_from_icon_name("system-run", GTK_ICON_SIZE_DIALOG);

  button = gtk_button_new_with_label(_("Run"));
  g_signal_connect (G_OBJECT(button), "button-press-event", G_CALLBACK(execute), NULL);
  g_signal_connect (G_OBJECT(path), "key-press-event", G_CALLBACK(path_key), NULL);


  box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 2);
  run_panel = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 15);

  gtk_box_pack_end (GTK_BOX(box), button, TRUE, FALSE, 0);
  gtk_box_pack_end (GTK_BOX(box), path, TRUE, FALSE, 0);
  gtk_box_pack_end (GTK_BOX(box), run_panel, TRUE, FALSE, 0);

  gtk_box_pack_end (GTK_BOX(run_panel), label, TRUE, FALSE, 0);
  gtk_box_pack_end (GTK_BOX(run_panel), icon, TRUE, FALSE, 0);


  gtk_container_add(GTK_CONTAINER(dialog), box);


  gtk_widget_show_all(dialog);




}


#endif
