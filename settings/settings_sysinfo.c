/*
This file is licensed under the MIT-License
Copyright (c) 2015 Marius Messerschmidt
For more details view file 'LICENSE'
*/

#include <jetspace/configkit.h>
#include <gtk/gtk.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/sysinfo.h>
#include <pwd.h>
#include <glib/gi18n.h>
#include <side/plugin.h>
#include "../shared/info.h"

long side_sysinfo_get_ram(struct sysinfo *inf)
{
  return inf->totalram * inf->mem_unit / 1024 / 1024;
}

char *side_sysinfo_get_uptime(struct sysinfo *inf)
{
  char *ret;
  int h = inf->uptime / 3600;
  int temp = inf->uptime % 3600;
  int m = temp / 60;
  int s = temp % 60;
  ret = g_strdup_printf("%02d:%02d:%02d", h, m, s);
  return ret;
}

char *side_sysinfo_get_cpu(void)
{
  FILE *f = fopen("/proc/cpuinfo", "r");
  char buffer[2000];

  while(fgets(buffer, 2000, f) != NULL)
  {
    if(strncmp(buffer, "model name", 10) == 0)
    {
      fclose(f);
      strtok(buffer, ":");
      return strtok(NULL, "\n\0");
    }
  }

  fclose(f);
  return _("Unknown CPU");
}

GtkWidget *build_sysinfo_settigns(void)
{
  GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);



  // Build layout:
  GtkWidget *box_mem   = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
  GtkWidget *mem_label = gtk_label_new(_("Memory:"));
  GtkWidget *mem_data  = gtk_label_new("");
  gtk_box_pack_start(GTK_BOX(box_mem), mem_label, FALSE, FALSE, 0);
  gtk_box_pack_end(GTK_BOX(box_mem), mem_data, FALSE, FALSE, 0);

  GtkWidget *box_uptime   = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
  GtkWidget *uptime_label = gtk_label_new(_("Uptime:"));
  GtkWidget *uptime_data  = gtk_label_new("");
  gtk_box_pack_start(GTK_BOX(box_uptime), uptime_label, FALSE, FALSE, 0);
  gtk_box_pack_end(GTK_BOX(box_uptime), uptime_data, FALSE, FALSE, 0);

  GtkWidget *box_cpu   = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
  GtkWidget *cpu_label = gtk_label_new(_("Processor:"));
  GtkWidget *cpu_data  = gtk_label_new("");
  gtk_box_pack_start(GTK_BOX(box_cpu), cpu_label, FALSE, FALSE, 0);
  gtk_box_pack_end(GTK_BOX(box_cpu), cpu_data, FALSE, FALSE, 0);

  GtkWidget *box_side   = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
  GtkWidget *side_label = gtk_label_new(_("SiDE Version:"));
  GtkWidget *side_data  = gtk_label_new(VERSION);
  gtk_box_pack_start(GTK_BOX(box_side), side_label, FALSE, FALSE, 0);
  gtk_box_pack_end(GTK_BOX(box_side), side_data, FALSE, FALSE, 0);


  //put together
  gtk_box_pack_start(GTK_BOX(box), gtk_image_new_from_pixbuf(gdk_pixbuf_scale_simple(gdk_pixbuf_new_from_file("/usr/share/icons/jetspace/JetSpace.png", NULL), 100, 100, GDK_INTERP_BILINEAR)), FALSE, FALSE, 10);
  gtk_box_pack_start(GTK_BOX(box), box_uptime, FALSE, FALSE, 5);
  gtk_box_pack_start(GTK_BOX(box), box_side, FALSE, FALSE, 5);
  gtk_box_pack_start(GTK_BOX(box), box_mem, FALSE, FALSE, 5);
  gtk_box_pack_start(GTK_BOX(box), box_cpu, FALSE, FALSE, 5);

  //prepare
  struct sysinfo info;
  sysinfo(&info);


  //fill values
  char *temp;
  temp = g_strdup_printf("%ld MiB", side_sysinfo_get_ram(&info));
  gtk_label_set_text(GTK_LABEL(mem_data), temp);
  g_free(temp);

  temp = side_sysinfo_get_uptime(&info);
  gtk_label_set_text(GTK_LABEL(uptime_data), temp);
  g_free(temp);

  gtk_label_set_text(GTK_LABEL(cpu_data), side_sysinfo_get_cpu());

  return box;
}

void callback(gpointer d)
{
  GtkBox *container = GTK_BOX(d);
  GtkWidget *cont = build_sysinfo_settigns();
  gtk_box_pack_start(container, cont, TRUE, TRUE, 0);
}

SiDESettingsPluginDescription side_sysinfo_plugin_desc;

SiDESettingsPluginDescription *identify(gpointer data)
{
  side_sysinfo_plugin_desc.label = _("System Information");
  side_sysinfo_plugin_desc.hover = _("View System data like Memory or CPU");
  side_sysinfo_plugin_desc.icon  = "dialog-information";
  side_sysinfo_plugin_desc.title = _("System Information");
  side_sysinfo_plugin_desc.cmd = "settings.sysinfo";
  side_sysinfo_plugin_desc.category = 2;
  return &side_sysinfo_plugin_desc;
}
