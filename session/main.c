/*
This file is licensed under the MIT-License
Copyright (c) 2015 Marius Messerschmidt
For more details view file 'LICENSE'
*/

#include <glib.h>
#include <gtk/gtk.h>
#include "../shared/info.h"
#include "../shared/strdup.h"
#include <string.h>
#include <stdlib.h>
#include <side/apps.h>


/*
This file contains the startup tool
to run the desktop enviroment you just have to type:

side-shell

*/

int SESSION = 0; //will contain the return value -> errors

gboolean panel     = TRUE;
gboolean wallpaper = TRUE;
gboolean wm        = TRUE;
gboolean autostart = TRUE;
gboolean notify    = TRUE;

#define PANEL "dbus-launch side-panel &"
#define WALLPAPER "dbus-launch side-wallpaper-service &"
#define NOTIFY "dbus-launch side-notifyd &"

void XDG_autostart(void)
{
  side_apps_load_dir("/etc/xdg/autostart/");
  AppEntry ent;
  ent.valid = TRUE;
  int count = 0;
  while(ent.valid == TRUE)
  {
    char *buff;
    ent = side_apps_get_next_entry();
    if(ent.show_in_side == false || ent.hidden == true)
      continue;
    buff = g_strdup_printf("%s &", ent.exec);
    system(buff);
    free(buff);
    count++;
  }
  side_apps_close();

  g_print("Loaded %d Autostart apps.\n", count);

}


void autorun(void)
{
  GSettings *s = g_settings_new("org.jetspace.desktop.session");
  char *str    = strdup(g_variant_get_string(g_settings_get_value(s, "autostart"), NULL));

  char *p = strtok(str, ";");

  while(p != NULL)
    {
      char *exec = malloc(strlen(p) + 3);
      strcpy(exec, p);
      strcat(exec, " &");
      g_debug("AUTOSTART: launchings %s\n", exec);
      system(exec);
      free(exec);
      p = strtok(NULL, ";");
    }
  free(p);
  free(str);

}




int main(int argc, char **argv)
{
  GSettings *session = g_settings_new("org.jetspace.desktop.session");

  if(argc > 1)
  {
    if(strcmp(argv[1], "--logout") == 0)
      {
        system("killall side-session");
        return 0;
      }
    if(strcmp(argv[1], "--reboot") == 0)
      {
        system(g_variant_get_string(g_settings_get_value(session, "reboot"), NULL));
        return 0;
      }
    if(strcmp(argv[1], "--shutdown") == 0)
      {
        system(g_variant_get_string(g_settings_get_value(session, "shutdown"), NULL));
        return 0;
      }
    if(strcmp(argv[1], "--restart") == 0)
      {
        system("killall side-panel && side-panel &");
        system("killall side-wallpaper-service && side-wallpaper-service &");
        system("killall side-notifyd && side-notifyd &");
        puts("restarted services!");
        exit(0);
      }
  }

  g_print("SIDE-session Version %s loading...\n", VERSION);

  if(notify)
    system(NOTIFY);

  if(wm)
    system(g_variant_get_string(g_settings_get_value(session, "wm"), NULL));

  if(panel)
    system(PANEL);

  if(wallpaper)
    system(WALLPAPER);

  if(autostart)
    autorun();
  if(g_variant_get_boolean(g_settings_get_value(session, "xdg-autostart")))
    XDG_autostart();

  //now go to endless mode, so the session won't fail
  g_print("switching to endless loop...\n");

  while(1);



  return SESSION;
}
