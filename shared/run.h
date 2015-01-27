#ifndef _HAVE_RUN_DIALOG
#define _HAVE_RUN_DIALOG

#include <gtk/gtk.h>
#include <string.h>
#include <stdlib.h>

GtkWidget *dialog, *box, *panel, *label, *path, *pic, *button;

gboolean execute(GtkWidget *widget, GdkEvent *event, GtkWidget *data)
{

  const char *ptr = gtk_entry_get_text(GTK_ENTRY(path));
  char *dup = strdup(ptr);
  char cmd[strlen(ptr) + 5];
  snprintf(cmd, strlen(ptr) +5,  "%s &", dup); //atach a & to run it interpendant!
  gtk_widget_destroy(dialog);
  system(cmd);
  return FALSE;
}

gboolean path_key(GtkWidget *widget, GdkEventKey *event, gpointer data)
{//Execute when hit [ENTER]
  if(event->keyval == GDK_KEY_Return)
    execute(NULL, NULL, NULL); //it gets its data from somewere else ;-)

  return FALSE;


}

//will show an run dialog
//app_call is the calling app -> will be phrased somedays....
void run_dialog (gchar *app_call)
{


  dialog = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title(GTK_WINDOW(dialog), "Run...");
  gtk_window_set_resizable(GTK_WINDOW(dialog), FALSE);
  gtk_window_set_icon_name(GTK_WINDOW(dialog), "system-run");
  gtk_container_set_border_width(GTK_CONTAINER(dialog), 15);

  label = gtk_label_new("Enter a Programm to run:");

  path = gtk_entry_new();

  pic = gtk_image_new_from_icon_name("system-run", GTK_ICON_SIZE_DIALOG);

  button = gtk_button_new_with_label("Execute");
  g_signal_connect (G_OBJECT(button), "button-press-event", G_CALLBACK(execute), NULL);
  g_signal_connect (G_OBJECT(path), "key-press-event", G_CALLBACK(path_key), NULL);


  box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 2);
  panel = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 15);

  gtk_box_pack_end (GTK_BOX(box), button, TRUE, FALSE, 0);
  gtk_box_pack_end (GTK_BOX(box), path, TRUE, FALSE, 0);
  gtk_box_pack_end (GTK_BOX(box), panel, TRUE, FALSE, 0);

  gtk_box_pack_end (GTK_BOX(panel), label, TRUE, FALSE, 0);
  gtk_box_pack_end (GTK_BOX(panel), pic, TRUE, FALSE, 0);


  gtk_container_add(GTK_CONTAINER(dialog), box);


  gtk_widget_show_all(dialog);




}


#endif
