/*
This file is licensed under the MIT-License
Copyright (c) 2015 Marius Messerschmidt
For more details view file 'LICENSE'
*/
#define _XOPEN_SOURCE 500
#include <glib.h>
#include <gtk/gtk.h>
#include "../shared/info.h"
#include "../shared/strdup.h"
#include <string.h>
#include <stdlib.h>
#include <side/apps.h>
#include <unistd.h>


/*
This file contains the startup tool
to run the desktop enviroment you just have to type:

side-session

*/

int SESSION = 0; //will contain the return value -> errors

gboolean panel     = TRUE;
gboolean wallpaper = TRUE;
gboolean wm        = TRUE;
gboolean autostart = TRUE;

#define PANEL "dbus-launch side-panel &"
#define WALLPAPER "dbus-launch side-wallpaper-service &"

void XDG_autostart(void)
{
  //Variables
  int count = 0;
  AppEntry ent;

  side_apps_load_dir("/etc/xdg/autostart/");

  ent.valid = TRUE;

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

  char *d = g_strdup_printf("%s/.config/autostart/", g_get_home_dir());
  if(!g_file_test(d, G_FILE_TEST_EXISTS))
  {
    g_print("Loaded %d Autostart apps.\n (no autostart from .config)", count);
    return;
  }

  side_apps_load_dir(d);
  ent.valid = TRUE;
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
        system("killall side-panel");
        system("side-panel &");
        system("killall side-wallpaper-service");
        system("side-wallpaper-service &");
        puts("restarted services!");
        exit(0);
      }
  }

  g_print("SIDE-session Version %s loading...\n", VERSION);

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

  while(1)
  {
    usleep(100);
  }



  return SESSION;
}
