/*
This file is licensed under the MIT-License
Copyright (c) 2015 Marius Messerschmidt
For more details view file 'LICENSE'
*/

#define _XOPEN_SOURCE 700


#include <gtk/gtk.h>
#include <string.h>
#include "../shared/run.h"

gboolean key_press(GtkWidget *widget, GdkEvent *event, gpointer data);

int main(int argc, char **argv)
{
  gtk_init(&argc, &argv);

  GSettings *gnome_conf;
  gnome_conf = g_settings_new("org.gnome.desktop.background");

  const char *ptr = g_variant_get_string(g_settings_get_value(gnome_conf, "picture-uri"), NULL);
  g_debug("[%s] - Loading Wallpaper %s", argv[0], ptr);

  GtkWidget *window, *pic, *ev_box;

  //Setup Window
  window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_decorated(GTK_WINDOW(window), FALSE);
  gtk_window_set_type_hint(GTK_WINDOW(window), GDK_WINDOW_TYPE_HINT_DESKTOP);
  gtk_window_maximize(GTK_WINDOW(window));

  //Eventbox

  ev_box = gtk_event_box_new();
  gtk_event_box_set_above_child(GTK_EVENT_BOX(ev_box), FALSE);


  //Setup GtkImage
  char *pic_path = strdup(ptr);
  pic_path = strtok(pic_path, "//");
  pic_path = strtok(NULL, "\0");
  pic = gtk_image_new_from_file(pic_path);
  gtk_container_add(GTK_CONTAINER(ev_box), pic);
  gtk_container_add(GTK_CONTAINER(window), ev_box);



  //connect Key press event
  g_signal_connect (G_OBJECT(ev_box), "key-press-event", G_CALLBACK(key_press), NULL);
  gtk_widget_set_events(ev_box, GDK_BUTTON_PRESS_MASK);
  gtk_widget_realize(ev_box);

  gtk_widget_show_all(window);
  gtk_main();
}


//Is not working right now...
gboolean key_press(GtkWidget *widget, GdkEvent *event, gpointer data)
{
  switch(event->key.keyval)
  {
    case GDK_KEY_r:
    //if(event->key.state == GDK_MOD1_MASK)
      run_dialog("desktop");
    break;


  }

  return FALSE;
}
