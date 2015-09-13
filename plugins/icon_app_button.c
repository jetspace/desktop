/*
This file is licensed under the MIT-License
Copyright (c) 2015 Marius Messerschmidt
For more details view file 'LICENSE'
*/
/*
  This replaces the default app menu button with a icon one

  I wrote this to achive an more icon design like look xD
*/


#include <gtk/gtk.h>

#include <time.h>
#include <gmodule.h>
#include <alsa/asoundlib.h>
#include <alsa/control.h>
#include <glib.h>
#include <side/plugin.h>
#include <glib/gi18n.h>

gboolean icon_app_button_can_be_activated = FALSE;
gboolean icon_app_button_enabled = FALSE;

GtkWidget *icon_button;


//MODLOADER
G_MODULE_EXPORT void plugin_call(GtkWidget *root)
{
  if(!check_version(COMPATIBLE_SINCE, "0.64"))
  {
    g_warning("icon_app_button plugin is not compatible!");
    return;
  }

  icon_app_button_can_be_activated = TRUE;

  g_print(":: Loading MaterialAppButton"); //notify the user...
  GtkWidget *panel_root_box = side_plugin_get_root_box(root);

  //find app button...
  GList *ch, *iter;
  ch = gtk_container_get_children(GTK_CONTAINER(panel_root_box));
  for(iter = ch; iter != NULL; iter = g_list_next(iter))
  {
    if(GTK_IS_BUTTON(iter->data))
    {
      if(strcmp(gtk_button_get_label(GTK_BUTTON(iter->data)), _("Applications")) == 0)
      {
        icon_button = GTK_WIDGET(iter->data);
        break;
      }
    }
  }
  //g_list_free(ch);

  gtk_image_new_from_icon_name("gtk-home", GTK_ICON_SIZE_LARGE_TOOLBAR);
}

/*
  Based on two functions: patch_app_button and unpatch_app_button
  Should be self explaining...
*/

void patch_app_button(void)
{
  gtk_button_set_image(GTK_BUTTON(icon_button), gtk_image_new_from_icon_name("gtk-home", GTK_ICON_SIZE_LARGE_TOOLBAR));
  gtk_button_set_label(GTK_BUTTON(icon_button), NULL);
  gtk_widget_show_all(GTK_WIDGET(icon_button));
}

void unpatch_app_button(void)
{
  gtk_button_set_image(GTK_BUTTON(icon_button), NULL);
  gtk_button_set_label(GTK_BUTTON(icon_button), _("Applications"));
  gtk_widget_show_all(GTK_WIDGET(icon_button));
}

G_MODULE_EXPORT void enable_plugin(GtkWidget *root)
{
  if(!icon_app_button_can_be_activated || icon_app_button_enabled)
    return;

  patch_app_button();
  icon_app_button_enabled = TRUE;
}

G_MODULE_EXPORT void disable_plugin(GtkWidget *root)
{
  if(!icon_app_button_can_be_activated || !icon_app_button_enabled)
    return;

  icon_app_button_enabled = FALSE;
  unpatch_app_button();
}
