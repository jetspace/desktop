/*
This file is licensed under the MIT-License
Copyright (c) 2015 Marius Messerschmidt
For more details view file 'LICENSE'
*/
#include "widgets.h"
int mode;


void side_modify_css(char *str)
{
  GdkDisplay *display;
  GdkScreen *screen;
  display = gdk_display_get_default ();
  screen = gdk_display_get_default_screen (display);
  GtkCssProvider *provider;
  provider = gtk_css_provider_new ();
  gtk_style_context_add_provider_for_screen (screen, GTK_STYLE_PROVIDER (provider), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
  gtk_css_provider_load_from_data(provider, str, -1, NULL);
  g_object_unref (provider);
}

void side_set_application_mode(int type)
{
  GSettings *theme = g_settings_new("org.jetspace.desktop.session");
  GdkDisplay *display;
  GdkScreen *screen;
  display = gdk_display_get_default ();
  screen = gdk_display_get_default_screen (display);

  mode = type;

  if(g_variant_get_boolean(g_settings_get_value(theme, "use-custom-theme")))
  {
      GtkCssProvider *provider;
      provider = gtk_css_provider_new ();
      gtk_style_context_add_provider_for_screen (screen, GTK_STYLE_PROVIDER (provider), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
      gsize bytes, read;
      const gchar* t = g_strdup_printf( "%s%s",g_variant_get_string(g_settings_get_value(theme, "custom-theme-path"), NULL), "/side-session/gtk.css");

      if(access(t, F_OK) != 0)
      {
        system("side-notifier --theme-not-found &");
        return;
      }

      gtk_css_provider_load_from_path (provider,g_filename_to_utf8(t, strlen(t), &read, &bytes, NULL),NULL);
      g_object_unref (provider);
  }



}

GtkWidget *side_gtk_label_new(char *text)
{
  GtkWidget *label;
  switch (mode)
  {
    case SIDE_APPLICATION_MODE_SETTINGS:
      label = gtk_label_new(text);
      gtk_label_set_xalign(GTK_LABEL(label), 0.0);
      return label;
    break;

    default:
      label = gtk_label_new(text);
    break;
  }

  return label;
}
GtkWidget *side_gtk_menu_new(void)
{
  GtkWidget *menu;


  switch(mode)
  {
    case SIDE_APPLICATION_MODE_WALLPAPER:
      menu = gtk_menu_new();
      gtk_widget_set_name(menu, "SiDEContextMenuDesktop");
      gtk_widget_realize(menu);
    break;
    default:
      menu = gtk_menu_new();
    break;
  }

  return menu;
}

GtkWidget *side_gtk_image_new_from_file(char *file)
{
  GtkWidget *img;

  switch(mode)
  {
    case SIDE_APPLICATION_MODE_WALLPAPER:
      img = gtk_image_new_from_file(file);
      gtk_widget_set_name(img, "SiDEDesktopWallpaper");
    break;
    default:
      img = gtk_image_new_from_file(file);
    break;

  }

  return img;
}
