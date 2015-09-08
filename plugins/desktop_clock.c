/*
This file is licensed under the MIT-License
Copyright (c) 2015 Marius Messerschmidt
For more details view file 'LICENSE'
*/

/* Will display the time embeded in the wallpaper (like desktop clock)*/

#include <gtk/gtk.h>
#include <glib.h>
#include <time.h>
#include <gmodule.h>
#include <side/plugin.h>
#include <glib/gi18n.h>

#define TIME_STYLE_24H_SEC  "%H:%M:%S"
#define TIME_STYLE_24H      "%H:%M"
#define TIME_STYLE_12H_SEC  "%I:%M:%S %p"
#define TIME_STYLE_12H      "%I:%M %p"

gboolean clock_can_be_activated = FALSE;
gboolean clock_enabled = FALSE;

void show_clock(void);
gboolean redraw(GtkWidget *widget, GdkEvent  *event, gpointer user_data);
gboolean update_time(gpointer data);

GtkWidget *side_desktop_clock, *box;
GtkWidget *s_24, *s_sec, *s_utc;

gboolean update_time(gpointer data)
{
  if(!clock_enabled)
    return FALSE;

  GSettings *time_style;
  time_style = g_settings_new("org.jetspace.desktop.panel");

  time_t t = time(&t);

  struct tm *t_m;

  if(!g_variant_get_boolean(g_settings_get_value(time_style, "utc-time")))
     t_m = localtime(&t);
  else
     t_m = gmtime(&t);
  char time_str[25];



  gint style = g_variant_get_int32(g_settings_get_value(time_style, "time-code"));

  switch(style)
  { //generate style string depending on gsettings
    case 1:
      strftime(time_str, 25, TIME_STYLE_24H_SEC , t_m);
    break;

    case 2:
      strftime(time_str, 25, TIME_STYLE_24H , t_m);
    break;

    case 3:
      strftime(time_str, 25, TIME_STYLE_12H_SEC , t_m);
    break;

    case 4:
      strftime(time_str, 25, TIME_STYLE_12H , t_m);
    break;
  }


  gtk_label_set_text(GTK_LABEL(side_desktop_clock), time_str);
  return TRUE;
}
//will be called when clock is destroyed
gboolean redraw(GtkWidget *widget, GdkEvent  *event, gpointer user_data)
{
  if(!clock_enabled)
    return FALSE;


  g_debug("captured destuction of the clock -> re-create");
  show_clock(); //draw it again!!
  update_time(NULL); //don't show placeholder...
  return FALSE;
}

void show_clock(void)
{
  side_desktop_clock = gtk_label_new("");


  gtk_fixed_put(GTK_FIXED(box), side_desktop_clock, 0,0);
  g_signal_connect(G_OBJECT(side_desktop_clock), "destroy", G_CALLBACK(redraw), NULL);
}

//MODLOADER
G_MODULE_EXPORT void plugin_call(GtkWidget *root)
{
  if(!check_version(COMPATIBLE_SINCE, "0.82"))
  {
    g_warning("Desktop clock is not compatible!");
    return;
  }

  clock_can_be_activated = TRUE;
  g_print("------------------------------\n-> SIDE Desktop clock loading...\n------------------------------\n"); //notify the user...
  box = root;
}

G_MODULE_EXPORT void enable_plugin(GtkWidget *root)
{
  if(!clock_can_be_activated || clock_enabled)
    return;

  show_clock();
  g_timeout_add(1000, update_time, NULL);
  clock_enabled = TRUE;
}

G_MODULE_EXPORT void disable_plugin(GtkWidget *root)
{
  if(!clock_can_be_activated || !clock_enabled)
    return;

  clock_enabled = FALSE;
  gtk_widget_destroy(side_desktop_clock);
}
