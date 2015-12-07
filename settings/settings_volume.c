/*
This file is licensed under the MIT-License
Copyright (c) 2015 Marius Messerschmidt
For more details view file 'LICENSE'
*/

#include <gtk/gtk.h>
#include <string.h>
#include <unistd.h>
#include <glib/gi18n.h>
#include <side/plugin.h>
#include <alsa/asoundlib.h>
#include <alsa/control.h>
#include "../shared/info.h"

int pre_mute_level = 50; //TODO!!

void set_volume(GtkAdjustment *adjustment, gpointer data)
{
  int value = (int) gtk_adjustment_get_value(adjustment);
  g_debug("Setting master volume to: %d", value);


  if(value == 0)
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(data), TRUE);
  else
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(data), FALSE);


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

gboolean set_mute(GtkToggleButton *widget, gpointer user_data)
{
  if(gtk_toggle_button_get_active(widget))
    {
         pre_mute_level = gtk_adjustment_get_value(GTK_ADJUSTMENT(user_data));
         gtk_adjustment_set_value(GTK_ADJUSTMENT(user_data), 0);
    }
 else
    gtk_adjustment_set_value(GTK_ADJUSTMENT(user_data), pre_mute_level);

  return FALSE;

}


GtkWidget *build_audio_settigns(void)
{
  GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);

  gtk_container_set_border_width(GTK_CONTAINER(box), 25);
  gtk_box_pack_start(GTK_BOX(box), gtk_label_new(_("Master Volume")), FALSE, FALSE, 0);

  GtkAdjustment *adj = gtk_adjustment_new(50, 0, 100, 1, 1, 1);
  GtkWidget     *scl = gtk_scale_new(GTK_ORIENTATION_HORIZONTAL, adj);
  gtk_scale_set_draw_value(GTK_SCALE(scl), TRUE);
  gtk_scale_set_value_pos(GTK_SCALE(scl), GTK_POS_LEFT);
  gtk_scale_set_digits(GTK_SCALE(scl),0);

  // Add Scale Marks
  gtk_scale_add_mark(GTK_SCALE(scl), 25, GTK_POS_BOTTOM, "<small>25</small>");
  gtk_scale_add_mark(GTK_SCALE(scl), 50, GTK_POS_BOTTOM, "50");
  gtk_scale_add_mark(GTK_SCALE(scl), 75, GTK_POS_BOTTOM, "<small>75</small>");

  gtk_box_pack_start(GTK_BOX(box), scl, FALSE, FALSE, 15);

  GtkWidget *cb_mute = gtk_check_button_new_with_label(_("Mute"));
  gtk_box_pack_start(GTK_BOX(box), cb_mute, FALSE, FALSE,5);



  g_signal_connect(G_OBJECT(adj), "value-changed", G_CALLBACK(set_volume), cb_mute);
  g_signal_connect(G_OBJECT(cb_mute), "toggled", G_CALLBACK(set_mute), adj);

  // SET VOLUME TO adjustment
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
  gtk_adjustment_set_value(adj, 100 * val / max +1);


  snd_mixer_close(mix);

  return box;
}

void callback(gpointer d)
{
  GtkBox *container = GTK_BOX(d);
  GtkWidget *cont = build_audio_settigns();
  gtk_box_pack_start(container, cont, TRUE, TRUE, 0);
}

SiDESettingsPluginDescription side_audio_plugin_desc;

SiDESettingsPluginDescription *identify(gpointer data)
{
  side_audio_plugin_desc.label = _("Audio");
  side_audio_plugin_desc.hover = _("Setup Audio");
  side_audio_plugin_desc.icon  = "multimedia-volume-control";
  side_audio_plugin_desc.title = _("Audio");
  side_audio_plugin_desc.cmd = "settings.audio";
  side_audio_plugin_desc.category = SIDE_SETTINGS_CATEGORY_MULTIMEDIA;
  return &side_audio_plugin_desc;
}
