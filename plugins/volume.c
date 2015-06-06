/*
This file is licensed under the MIT-License
Copyright (c) 2015 Marius Messerschmidt
For more details view file 'LICENSE'
*/

/*
  Interfaces with ALSA to set master volume
*/

/*

Little Notice:
Gtk puts the GtkScale Widget upside down for whatever reason...
Maybe this is usefull for some apps, but a volume slide should
be 100 if it is at the top and 0 if it is at the bottom! Not upside
down...


To workaround you can use: val = 100 - val

*/



#include <gtk/gtk.h>

#include <time.h>
#include <gmodule.h>
#include <alsa/asoundlib.h>
#include <alsa/control.h>
#include <glib.h>
#include <side/plugin.h>

GtkWidget *volume_button, *box, *s_mute;

void create_volume_button(void);

GtkWidget *fwin;
gboolean check_focus(gpointer data)
{
    if(gtk_window_is_active(GTK_WINDOW(fwin)))
        return TRUE;
    else
        {
            gtk_widget_destroy(fwin);
            return FALSE;
        }

}



gboolean set_mute(GtkToggleButton *widget, gpointer user_data)
{
  if(gtk_toggle_button_get_active(widget))
    gtk_adjustment_set_value(GTK_ADJUSTMENT(user_data), 100);

  return FALSE;

}

void set_volume(GtkAdjustment *adjustment, gpointer user_data)
{
  int value = (int) 100 - gtk_adjustment_get_value(adjustment);
  g_debug("Setting master volume to: %d", value);

  if(value == 0)
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(s_mute), TRUE);
  else
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(s_mute), FALSE);

  long min, max;
  snd_mixer_t *mix;
  snd_mixer_selem_id_t *sid;
  const char *card = "default";
  const char *selem_name = "Master";

  snd_mixer_open(&mix, 0);
  snd_mixer_attach(mix, card);
  snd_mixer_selem_register(mix, NULL, NULL);
  snd_mixer_load(mix);

  snd_mixer_selem_id_alloca(&sid);
  snd_mixer_selem_id_set_index(sid, 0);
  snd_mixer_selem_id_set_name(sid, selem_name);
  snd_mixer_elem_t* elem = snd_mixer_find_selem(mix, sid);

  snd_mixer_selem_get_playback_volume_range(elem, &min, &max);
  snd_mixer_selem_set_playback_volume_all(elem, value * max / 100);

  snd_mixer_close(mix);

}

gboolean show_mixer(GtkWidget *widget, GdkEvent  *event, gpointer user_data)
{
  GtkWidget *win = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title(GTK_WINDOW(win), "SIDE Mixer");
  gtk_container_set_border_width(GTK_CONTAINER(win), 10);
  gtk_window_resize(GTK_WINDOW(win), 150, 300);
  GdkScreen *screen = gdk_screen_get_default();
  gtk_window_move(GTK_WINDOW(win), gdk_screen_get_width(screen), gdk_screen_get_height(screen) - 335);

  fwin = win;

  g_timeout_add(100, check_focus, win);

  GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
  GtkWidget *dsc = gtk_label_new("master");

  GtkAdjustment *adj = gtk_adjustment_new(50, 0, 100, 5, 0, 0);
  g_signal_connect(G_OBJECT(adj), "value-changed", G_CALLBACK(set_volume), NULL);
  GtkWidget     *scl = gtk_scale_new(GTK_ORIENTATION_VERTICAL, adj);
  gtk_scale_set_draw_value(GTK_SCALE(scl), FALSE);

  s_mute = gtk_toggle_button_new_with_label("mute");

  g_signal_connect(G_OBJECT(s_mute), "toggled", G_CALLBACK(set_mute), (gpointer) adj);


  gtk_box_pack_end(GTK_BOX(box), s_mute, FALSE, FALSE, 5);


  gtk_container_add(GTK_CONTAINER(box), dsc);
  gtk_box_pack_end(GTK_BOX(box), scl, TRUE, TRUE, 5);





  gtk_container_add(GTK_CONTAINER(win), box);
  gtk_widget_show_all(win);


  snd_mixer_t *mix;
  snd_mixer_selem_id_t *sid;
  const char *card = "default";
  const char *selem_name = "Master";

  long val, max, min;

  snd_mixer_open(&mix, 0);
  snd_mixer_attach(mix, card);
  snd_mixer_selem_register(mix, NULL, NULL);
  snd_mixer_load(mix);

  snd_mixer_selem_id_alloca(&sid);
  snd_mixer_selem_id_set_index(sid, 0);
  snd_mixer_selem_id_set_name(sid, selem_name);
  snd_mixer_elem_t* elem = snd_mixer_find_selem(mix, sid);

  snd_mixer_selem_get_playback_volume_range(elem, &min, &max);

  snd_mixer_selem_get_playback_volume(elem, SND_MIXER_SCHN_MONO  , &val);
  gtk_adjustment_set_value(adj, 100 - (int)  100 * val / max);


  snd_mixer_close(mix);



  return FALSE;
}

gboolean redraw_volume_button(GtkWidget *widget, GdkEvent  *event, gpointer user_data)
{
  g_debug("captured destuction of volume button -> re-create");
  create_volume_button(); //draw it again!!
  return FALSE;
}

void create_volume_button(void)
{
  volume_button = gtk_button_new_with_label(NULL);
  gtk_button_set_relief(GTK_BUTTON(volume_button), GTK_RELIEF_NONE);
  gtk_button_set_image (GTK_BUTTON(volume_button), gtk_image_new_from_icon_name("audio-volume-high", GTK_ICON_SIZE_SMALL_TOOLBAR));
  gtk_box_pack_end(GTK_BOX(box), volume_button, FALSE, FALSE, 5);

  g_signal_connect(G_OBJECT(volume_button), "destroy", G_CALLBACK(redraw_volume_button), NULL);
  g_signal_connect(G_OBJECT(volume_button), "button-press-event", G_CALLBACK(show_mixer), NULL);
}

//MODLOADER
G_MODULE_EXPORT void plugin_call(GtkWidget *root)
{
  g_print("------------------------------\n-> SIDE Volume loading...\n------------------------------\n"); //notify the user...
  box = side_plugin_get_root_box(root);
  create_volume_button();
}
