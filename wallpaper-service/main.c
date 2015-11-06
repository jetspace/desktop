/*
This file is licensed under the MIT-License
Copyright (c) 2015 Marius Messerschmidt
For more details view file 'LICENSE'
*/

#define _XOPEN_SOURCE 700



#include <gtk/gtk.h>
#include <string.h>
#include <glib/gi18n.h>
#include "../shared/run.h"
#include "../shared/plugins.h"
#include "../shared/context.h"


GtkWidget *window, *ev_box, *pic, *grid;

gboolean key_press(GtkWidget *w, GdkEventButton *e, GtkWidget *menu);

void update_wallpaper(GSettings *s, gchar *key, GtkWidget *box)
{
    char *pic_path = strdup(g_variant_get_string(g_settings_get_value(s, "picture-uri"), NULL));
    pic_path = strtok(pic_path, "//");
    pic_path = strtok(NULL, "\0");

    //SACLE PRPOS
    GdkScreen *sc = gdk_screen_get_default();
    gint w = gdk_screen_get_width(sc);
    gint h = gdk_screen_get_height(sc);

    GdkPixbuf *src = gdk_pixbuf_new_from_file(pic_path, NULL);
    GdkPixbuf *pb = gdk_pixbuf_scale_simple(src, w, h, GDK_INTERP_BILINEAR);

    //SCALE AND SET
    gtk_image_set_from_pixbuf(GTK_IMAGE(pic), pb);
    g_object_unref(pb);
    g_object_unref(src);
}

void plugin_callback(GSettings *s, gchar *key, gpointer data)
{
  char *plugin_string = strdup(g_variant_get_string(g_settings_get_value(s, "ignored-plugins"), NULL));
  update_plugins(plugin_string, grid);
  free(plugin_string);
}

int main(int argc, char **argv)
{
  textdomain("side");
  gtk_init(&argc, &argv);

  GtkCssProvider *p = gtk_css_provider_new();
  GdkDisplay *display;
  GdkScreen *screen;
  display = gdk_display_get_default ();
  screen = gdk_display_get_default_screen (display);
  gtk_style_context_add_provider_for_screen (screen, GTK_STYLE_PROVIDER (p), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
  gtk_css_provider_load_from_data(p, "GtkImage \n{\nbackground-color: transparent;\n-gtk-image-effect: none;\n}", -1, NULL);

  GSettings *plugin_config;
  plugin_config = g_settings_new("org.jetspace.desktop.wallpaper");
  g_signal_connect(G_OBJECT(plugin_config), "changed", G_CALLBACK(plugin_callback), NULL);


  GSettings *gnome_conf;
  gnome_conf = g_settings_new("org.gnome.desktop.background");
  g_signal_connect(G_OBJECT(gnome_conf), "changed", G_CALLBACK(update_wallpaper), NULL);


  //Setup Window
  window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_decorated(GTK_WINDOW(window), FALSE);
  gtk_window_move(GTK_WINDOW(window), 0, 0);
  gtk_window_set_type_hint(GTK_WINDOW(window), GDK_WINDOW_TYPE_HINT_DESKTOP);
  gtk_window_maximize(GTK_WINDOW(window));

  ev_box = gtk_event_box_new();

  GtkWidget *menu = gtk_menu_new();
  add_context_menu_desktop(menu, ev_box);


  g_signal_connect(G_OBJECT(window), "button_press_event", G_CALLBACK(key_press), menu);
  gtk_widget_set_events(ev_box, GDK_BUTTON_PRESS_MASK);

  //Setup GtkImage
  pic = gtk_image_new_from_file(""); //create empty box
  update_wallpaper(gnome_conf, "picture-uri", NULL);
  gtk_container_add(GTK_CONTAINER(ev_box), pic);

  grid = gtk_fixed_new();
  gtk_container_add(GTK_CONTAINER(window), grid);

  gtk_fixed_put(GTK_FIXED(grid), ev_box, 0, 0);

  //now, load plugins...
  load_plugins_wallpaper("/usr/lib/jetspace/wallpaper/plugins/", grid);


  gtk_window_resize(GTK_WINDOW(window), gdk_screen_get_width(screen), gdk_screen_get_height(screen));

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
