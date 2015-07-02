/*
This file is licensed under the MIT-License
Copyright (c) 2015 Marius Messerschmidt
For more details view file 'LICENSE'
*/

#include "plugin.h"
#include "../../shared/info.h"

GtkWidget *side_plugin_get_root_box(GtkWidget *root)
{
  GtkWidget *box = gtk_bin_get_child(GTK_BIN(gtk_bin_get_child(GTK_BIN(root))));
  return box;
}

gboolean load_plugin(char *path, gpointer data, gboolean resident)
{


    //MODULE VARS
    GModule *module = NULL;
    CallPlugin call = NULL;

  module = g_module_open(path, G_MODULE_BIND_LAZY);

  if(resident)
    g_module_make_resident(module); //users are able to create callbacks, because the module would not be unloaded...
  else
    g_debug("Skipping 'g_module_make_resident()' by user call");

  if(module == NULL)
    {
      g_warning("Loading Module failed: %s", g_module_error());
      return FALSE;
    }

  if(g_module_symbol(module, "plugin_call", (gpointer *) &call) == FALSE)
    {
      g_warning("Can't load symbol 'plugin_call': %s", g_module_error());
      return FALSE;
    }


  call(data);

  return TRUE;
}

gboolean load_plugins_from_dir(char *path, gpointer data, gboolean resident)
{
  DIR *d;
  struct dirent *de;
  char module_path[2000];

  d = opendir(path);
  if(d == NULL)
    {
      g_warning("MODULE PATH NOT FOUND");
      return FALSE;
    }
  while((de = readdir(d)) != NULL)
    {
      strncpy(module_path, path, 1800);
      strcat(module_path, de->d_name);

      g_debug("Loading %s\n", module_path);
      if(!load_plugin(module_path, data, resident))
        {
          g_warning("Loading plugins from %s failed!", path);
          return FALSE;
        }
    }

  return TRUE;
}


//Version checker
gboolean check_version(int type, int major, int minor)
{
  float ver = atof(VERSION);
  float VER = (float) major + ((float)minor/100);
  g_debug("%.2f comparing with: %.2f", ver, VER);

  switch (type)
  {
    case COMPATIBLE_SINCE:
    return VER <= ver;
    break;

    case COMPATIBLE_UNTIL:
    return VER >= ver;
    break;

    case ONLY_FOR_VERSION:
    return VER == ver;
  }

  return TRUE;
}
