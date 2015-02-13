/*
This file is licensed under the MIT-License
Copyright (c) 2015 Marius Messerschmidt
For more details view file 'LICENSE'
*/

#include <glib.h>
#include "../shared/info.h"
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

#define WINDOW_MANAGER "dbus-launch xfwm4 &"
#define PANEL "dbus-launch side-panel &"
#define WALLPAPER "dbus-launch side-wallpaper-service &"


int main(int argc, char **argv)
{
  g_print("SIDE-session Version %s loading...\n", VERSION);

  if(wm)
    system(WINDOW_MANAGER);

  if(panel)
    system(PANEL);

  if(wallpaper)
    system(WALLPAPER);

  //now go to endless mode, so the session won't fail
  g_print("switching to endless loop...\n");

  while(1);



  return SESSION;
}
