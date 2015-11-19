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

  g_warning("Function outdated");

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
  g_warning("Function outdated");
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
gboolean check_version(int type, char *target)
{
  int result = strcmp(VERSION, target);

  switch(type)
  {
    case COMPATIBLE_SINCE:
    return result >= 0;
    break;

    case COMPATIBLE_UNTIL:
    return result <= 0;
    break;

    case ONLY_FOR_VERSION:
    return result == 0;
    break;

  }
  return FALSE;
}

SettingPlugins *SettingPluginsStore;
int n_plugins = 0;

void load_side_settings_plugins(GtkListStore *app,GtkListStore *sys,GtkListStore *inf, char *path, gboolean onlyOld)
{

  GtkIconTheme *theme = gtk_icon_theme_get_default();
  if(!onlyOld)
  {
    //SCAN VARS
    DIR *d;
    struct dirent *de;
    char module_path[2000];

    d = opendir(path);
    if(d == NULL)
      {
        g_error("MODULE PATH NOT FOUND");
        return;
      }

    while((de = readdir(d)) != NULL)
      {
        strcpy(module_path, path);
        strcat(module_path, de->d_name);

        if(g_module_supported() == FALSE)
          {
            g_error("Modules are not available");
          }

        if(strlen(de->d_name) < 4)
          continue; //skip files without .so


        g_print("-----\nModule path: %s\n-----\n", module_path);

        n_plugins++;
        SettingPluginsStore = realloc(SettingPluginsStore, sizeof(SiDESettingsPluginDescription) * n_plugins);


        SettingPluginsStore[n_plugins -1].module = g_module_open(module_path, G_MODULE_BIND_LAZY);
        g_module_make_resident(SettingPluginsStore[n_plugins -1].module); //users are able to create callbacks, because the module would not be unloaded...


        if(SettingPluginsStore[n_plugins -1].module == NULL)
          {
            g_error("Loading Module failed: %s", g_module_error());
          }


        if(g_module_symbol(SettingPluginsStore[n_plugins -1].module, "identify", (gpointer *) &SettingPluginsStore[n_plugins -1].identify) == FALSE)
          {
            g_warning("Can't load symbol 'identify': %s", g_module_error());
            g_warning("Please note, that this is might caused by an outdated plugin!");
            continue;
          }

        if(g_module_symbol(SettingPluginsStore[n_plugins -1].module, "callback", (gpointer *) &SettingPluginsStore[n_plugins -1].callback) == FALSE)
          {
            g_warning("Can't load symbol 'callback': %s", g_module_error());
            g_warning("Please note, that this is might caused by an outdated plugin!");
            continue;
          }



        //init func
        SettingPluginsStore[n_plugins -1].identity = SettingPluginsStore[n_plugins -1].identify(NULL);
      }
    }

      for(int x = 0; x < n_plugins; x++)
      {
        GtkTreeIter iter;

        char *ex = g_strdup_printf("CB:%d", x);

        switch(SettingPluginsStore[x].identity->category)
        {
          case 0:
          gtk_list_store_append(app, &iter);
          gtk_list_store_set(app, &iter, 0, SettingPluginsStore[x].identity->label,1,ex,2, gtk_icon_theme_load_icon(theme, SettingPluginsStore[x].identity->icon, 32,GTK_ICON_LOOKUP_FORCE_SIZE ,NULL), -1);
          break;

          case 1:
          gtk_list_store_append(sys, &iter);
          gtk_list_store_set(sys, &iter, 0, SettingPluginsStore[x].identity->label,1,ex,2, gtk_icon_theme_load_icon(theme, SettingPluginsStore[x].identity->icon, 32,GTK_ICON_LOOKUP_FORCE_SIZE ,NULL), -1);
          break;

          case 2:
          gtk_list_store_append(inf, &iter);
          gtk_list_store_set(inf, &iter, 0, SettingPluginsStore[x].identity->label,1,ex,2, gtk_icon_theme_load_icon(theme, SettingPluginsStore[x].identity->icon, 32,GTK_ICON_LOOKUP_FORCE_SIZE ,NULL), -1);
          break;

        }
        free(ex);
      }

}

void exec_callback(int id, GtkWidget *box)
{
  SettingPluginsStore[id].callback(GTK_BOX(box));
}
