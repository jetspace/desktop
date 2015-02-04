/*
This file is licensed under the MIT-License
Copyright (c) 2015 Marius Messerschmidt
For more details view file 'LICENSE'
*/

#define _XOPEN_SOURCE 700



#include <gtk/gtk.h>
#include <string.h>
#include "../shared/run.h"
#include "../shared/context.h"

GtkWidget *window, *ev_box, *pic;

gboolean key_press(GtkWidget *w, GdkEventButton *e, GtkWidget *menu);

gboolean update_wallpaper(GSettings *s, gchar *key, GtkWidget *box)
{
    char *pic_path = strdup(g_variant_get_string(g_settings_get_value(s, "picture-uri"), NULL));
    pic_path = strtok(pic_path, "//");
    pic_path = strtok(NULL, "\0");
    gtk_image_set_from_pixbuf(GTK_IMAGE(pic), gdk_pixbuf_new_from_file(pic_path, NULL));
}

int main(int argc, char **argv)
{
  gtk_init(&argc, &argv);

  GSettings *gnome_conf;
  gnome_conf = g_settings_new("org.gnome.desktop.background");

  g_signal_connect(G_OBJECT(gnome_conf), "changed", G_CALLBACK(update_wallpaper), box);


  //Setup Window
  window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_decorated(GTK_WINDOW(window), FALSE);
  gtk_window_set_type_hint(GTK_WINDOW(window), GDK_WINDOW_TYPE_HINT_DESKTOP);
  gtk_window_maximize(GTK_WINDOW(window));

  ev_box = gtk_event_box_new();

  GtkWidget *menu = gtk_menu_new();
  add_context_menu_desktop(menu, ev_box);


  g_signal_connect(G_OBJECT(ev_box), "button_press_event", G_CALLBACK(key_press), menu);
  gtk_widget_set_events(ev_box, GDK_BUTTON_PRESS_MASK);

  //Setup GtkImage
  pic = gtk_image_new_from_file(""); //create empty box
  update_wallpaper(gnome_conf, "picture-uri", NULL);
  gtk_container_add(GTK_CONTAINER(ev_box), pic);
  gtk_container_add(GTK_CONTAINER(window), ev_box);






  gtk_widget_show_all(window);
  gtk_main();
}


gboolean key_press(GtkWidget *w, GdkEventButton *e, GtkWidget *menu)
{
  if((e->button == 3) && (e->type == GDK_BUTTON_PRESS))
    {
      gtk_menu_popup(GTK_MENU(menu), NULL, NULL, NULL, NULL, e->button, e->time);
    }

  return FALSE;
}
