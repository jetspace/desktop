/*
This file is licensed under the MIT-License
Copyright (c) 2015 Marius Messerschmidt
For more details view file 'LICENSE'
*/

/*
This plugin will show a clock in the SIDE panel
Planned function are:
  -> Time 12h/24h
  -> Popup Calendar
  -> UTC support
*/

#include <gtk/gtk.h>
#include <glib.h>
#include <time.h>
#include <gmodule.h>
#include <side/plugin.h>

#define TIME_STYLE_24H_SEC  "%H:%M:%S"
#define TIME_STYLE_24H      "%H:%M"
#define TIME_STYLE_12H_SEC  "%I:%M:%S %p"
#define TIME_STYLE_12H      "%I:%M %p"


void show_clock(void);
gboolean redraw(GtkWidget *widget, GdkEvent  *event, gpointer user_data);
gboolean update_time(gpointer data);

GtkWidget *side_panel_clock, *box, *side_panel_clock_box;
GtkWidget *s_24, *s_sec, *s_utc;

gboolean update_time(gpointer data)
{
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


  gtk_label_set_text(GTK_LABEL(side_panel_clock), time_str);
  return TRUE;
}

gboolean write_settings_clock(GtkWidget *w, GdkEventButton *e, gpointer d)
{
    GSettings *time_style;
    time_style = g_settings_new("org.jetspace.desktop.panel");
    if (gtk_switch_get_active(GTK_SWITCH(s_24)))
      {
        if(gtk_switch_get_active(GTK_SWITCH(s_sec)))
          g_settings_set_value(time_style, "time-code", g_variant_new_int32(1));
        else
          g_settings_set_value(time_style, "time-code", g_variant_new_int32(2));
      }
    else
      {
        if(gtk_switch_get_active(GTK_SWITCH(s_sec)))
          g_settings_set_value(time_style, "time-code", g_variant_new_int32(3));
        else
          g_settings_set_value(time_style, "time-code", g_variant_new_int32(4));
      }
    if(gtk_switch_get_active(GTK_SWITCH(s_utc)))
      g_settings_set_value(time_style, "utc-time", g_variant_new_boolean(TRUE));
    else
      g_settings_set_value(time_style, "utc-time", g_variant_new_boolean(FALSE));

    g_settings_sync();


    return FALSE;
}


gboolean clock_settings(GtkWidget *w, GdkEventButton *e, gpointer d)
{
    GtkWidget *win        = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_container_set_border_width(GTK_CONTAINER(win), 10);
    GdkScreen *screen = gdk_screen_get_default();
    gtk_window_move(GTK_WINDOW(win), gdk_screen_get_width(screen) -460  , gdk_screen_get_height(screen) - 150);
    gtk_window_set_title(GTK_WINDOW(win), "Panel Clock Settings");

    GtkWidget *box        = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);

    GtkWidget *label      = gtk_label_new("Set up the time format:");
    gtk_container_add(GTK_CONTAINER(box), label);

    GtkWidget *sep        = gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
    gtk_container_add(GTK_CONTAINER(box), sep);


    GtkWidget *l_box1     = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 25);

    GtkWidget *l2         = gtk_label_new("24h time format?");
    gtk_container_add(GTK_CONTAINER(l_box1), l2);

    s_24                  = gtk_switch_new();
    gtk_container_add(GTK_CONTAINER(l_box1), s_24);


    gtk_container_add(GTK_CONTAINER(box), l_box1);

    GtkWidget *l_box2     = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 38);

    GtkWidget *l3         = gtk_label_new("Show Seconds?");
    gtk_container_add(GTK_CONTAINER(l_box2), l3);

    s_sec      = gtk_switch_new();
    gtk_container_add(GTK_CONTAINER(l_box2), s_sec);


    gtk_container_add(GTK_CONTAINER(box), l_box2);


    GtkWidget *l_box3     = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 76);

    GtkWidget *l4         = gtk_label_new("Use UTC?");
    gtk_container_add(GTK_CONTAINER(l_box3), l4);

    s_utc      = gtk_switch_new();
    gtk_container_add(GTK_CONTAINER(l_box3), s_utc);

    gtk_container_add(GTK_CONTAINER(box), l_box3);

    GtkWidget *apply      = gtk_button_new_with_label("Apply");
    gtk_container_add(GTK_CONTAINER(box), apply);

    //Read GSETTINGS
    GSettings *time_style;
    time_style = g_settings_new("org.jetspace.desktop.panel");
    gint style = g_variant_get_int32(g_settings_get_value(time_style, "time-code"));

    switch(style)
    {
      case 1:
        gtk_switch_set_active(GTK_SWITCH(s_24),  TRUE);
        gtk_switch_set_active(GTK_SWITCH(s_sec), TRUE);
      break;

      case 2:
        gtk_switch_set_active(GTK_SWITCH(s_24),  TRUE);
        gtk_switch_set_active(GTK_SWITCH(s_sec), FALSE);
      break;

      case 3:
        gtk_switch_set_active(GTK_SWITCH(s_24),  FALSE);
        gtk_switch_set_active(GTK_SWITCH(s_sec), TRUE);
      break;

      case 4:
        gtk_switch_set_active(GTK_SWITCH(s_24),  FALSE);
        gtk_switch_set_active(GTK_SWITCH(s_sec), FALSE);
      break;

    }

    if(g_variant_get_boolean(g_settings_get_value(time_style, "utc-time")))
      gtk_switch_set_active(GTK_SWITCH(s_utc),  TRUE);
    else
      gtk_switch_set_active(GTK_SWITCH(s_utc),  FALSE);

    g_signal_connect(G_OBJECT(apply), "button_press_event", G_CALLBACK(write_settings_clock), NULL);


    gtk_container_add(GTK_CONTAINER(win), box);
    gtk_widget_show_all(win);
    return FALSE;
}


gboolean show_clock_context(GtkWidget *w, GdkEventButton *e, gpointer d)
{
  if((e->button == 1) && (e->type == GDK_2BUTTON_PRESS))
    {
      g_debug("Popup Callender");
      GtkWidget *win = gtk_window_new(GTK_WINDOW_TOPLEVEL);
      gtk_window_set_title(GTK_WINDOW(win), "SIDE Panel Clock");
      gtk_container_set_border_width(GTK_CONTAINER(win), 10);
      GdkScreen *screen = gdk_screen_get_default();
      gtk_window_move(GTK_WINDOW(win), gdk_screen_get_width(screen), gdk_screen_get_height(screen) - 265);

      GtkWidget *cal = gtk_calendar_new();
      GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);

      GtkWidget *set = gtk_button_new_with_label("Clock Settings");
      g_signal_connect(G_OBJECT(set), "button-press-event", G_CALLBACK(clock_settings), NULL);

      gtk_container_add(GTK_CONTAINER(box), cal);
      gtk_container_add(GTK_CONTAINER(box), set);
      gtk_container_add(GTK_CONTAINER(win), box);

      gtk_widget_show_all(win);
    }

  return FALSE;
}


//will be called when clock is destroyed
gboolean redraw(GtkWidget *widget, GdkEvent  *event, gpointer user_data)
{
  g_debug("captured destuction of the clock -> re-create");
  show_clock(); //draw it again!!
  update_time(NULL); //don't show placeholder...
  return FALSE;
}

void show_clock(void)
{
  side_panel_clock = gtk_label_new("");

  side_panel_clock_box = gtk_event_box_new();
  gtk_widget_set_events(side_panel_clock_box, GDK_BUTTON_PRESS_MASK);
  gtk_container_add(GTK_CONTAINER(side_panel_clock_box), side_panel_clock);
  gtk_event_box_set_above_child(GTK_EVENT_BOX(side_panel_clock_box), TRUE);


  g_signal_connect(G_OBJECT(side_panel_clock_box), "button-press-event", G_CALLBACK(show_clock_context), NULL);

  gtk_box_pack_end(GTK_BOX(box), side_panel_clock_box, FALSE, FALSE, 5);
  g_signal_connect(G_OBJECT(side_panel_clock), "destroy", G_CALLBACK(redraw), NULL);
}

//MODLOADER
G_MODULE_EXPORT void plugin_call(GtkWidget *root)
{
  g_print("------------------------------\n-> SIDE Panel clock loading...\n------------------------------\n"); //notify the user...
  box = side_plugin_get_root_box(root);
  show_clock();
  g_timeout_add(1000, update_time, NULL);

}
