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

#define PANEL "dbus-launch side-panel &"
#define WALLPAPER "dbus-launch side-wallpaper-service &"

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

  //now go to endless mode, so the session won't fail
  g_print("switching to endless loop...\n");

  while(1);



  return SESSION;
}
