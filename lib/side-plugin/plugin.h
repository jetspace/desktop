/*
This file is licensed under the MIT-License
Copyright (c) 2015 Marius Messerschmidt
For more details view file 'LICENSE'
*/

#ifndef _SIDE_PLUGIN_LIBRARY
#define _SIDE_PLUGIN_LIBRARY

#include <gtk/gtk.h>
#include <gmodule.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

  //PANEL

//Will return the 2nd child of root
GtkWidget *side_plugin_get_root_box(GtkWidget *root);

  //MODLOADER

  /*
    Please note, this functions will not do any testing of the file!
    You should only use load_plugins_from_dir() if you have checked the
    DIR before!

    It will call the function plugin_call, with the data pointer as argument...

  */

//plugin call typedef
typedef void (*CallPlugin) (gpointer data);


//loads the plugin, only this one(!)
gboolean load_plugin(char *path, gpointer data, gboolean resident);
//loads all plugins in  path/*
gboolean load_plugins_from_dir(char *path, gpointer data, gboolean resident);

//version checker

enum {

  COMPATIBLE_SINCE = 0, //check if version number is greater or equal
  COMPATIBLE_UNTIL,     //check if version number is smaller or equal
  ONLY_FOR_VERSION      //check if version number is equal
};
gboolean check_version(int type, char *target);
//type is used with one of the ENUM above, target is the version ("0.60", etc)

#endif
