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

void set_volume(GtkAdjustment *adjustment, gpointer user_data)
{
  int value = (int) gtk_adjustment_get_value(adjustment);
  g_debug("Setting master volume to: %d", value);

/*
  if(value == 0)
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(s_mute), TRUE);
  else
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(s_mute), FALSE);
*/

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


GtkWidget *build_audio_settigns(void)
{
  GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);

  gtk_container_set_border_width(GTK_CONTAINER(box), 25);
  gtk_box_pack_start(GTK_BOX(box), gtk_label_new(_("Master Volume")), FALSE, FALSE, 0);

  GtkAdjustment *adj = gtk_adjustment_new(50, 0, 100, 5, 0, 0);
  g_signal_connect(G_OBJECT(adj), "value-changed", G_CALLBACK(set_volume), NULL);
  GtkWidget     *scl = gtk_scale_new(GTK_ORIENTATION_HORIZONTAL, adj);
  gtk_scale_set_draw_value(GTK_SCALE(scl), TRUE);

  gtk_box_pack_start(GTK_BOX(box), scl, FALSE, FALSE, 15);



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
