/*
This file is licensed under the MIT-License
Copyright (c) 2015 Marius Messerschmidt
For more details view file 'LICENSE'
*/

#include <gmodule.h>
#include <gtk/gtk.h>
#include <sys/types.h>
#include <dirent.h>
#include "../shared/listed.h"

typedef void (*CallPlugin) (GtkWidget *root);

struct plugins {
  GModule *module;
  CallPlugin call;
  CallPlugin enable;
  CallPlugin disable;
  char *name;
  gboolean is_working;
};

struct plugins *all_plugins;
int n_plugins = 0;

GtkWidget *root_box;

void load_plugins_wallpaper(char *path, GtkWidget *root)
{
  root_box = root;
  char module_path[2000];

  //SCAN VARS
  DIR *d;
  struct dirent *de;

  d = opendir(path);
  if(d == NULL)
    {
      g_error("MODULE PATH NOT FOUND");
      return;
    }

    //read blacklist
    GSettings *gmods = g_settings_new("org.jetspace.desktop.wallpaper");
    char *ptr = strdup(g_variant_get_string(g_settings_get_value(gmods, "ignored-plugins"), NULL));
    char **mods = malloc(sizeof(ptr));

    char *mp = strtok(ptr, ";");

    int x = 0;
    while(mp != NULL)
      {
        mods[x] = strdup(mp);
        x++;
        mp = strtok(NULL, ";");
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
      all_plugins = realloc(all_plugins, sizeof(struct plugins) * n_plugins);


      all_plugins[n_plugins -1].module = g_module_open(module_path, G_MODULE_BIND_LAZY);
      g_module_make_resident(all_plugins[n_plugins -1].module); //users are able to create callbacks, because the module would not be unloaded...


      if(all_plugins[n_plugins -1].module == NULL)
        {
          g_error("Loading Module failed: %s", g_module_error());
          all_plugins[n_plugins -1].is_working = FALSE;
        }

      all_plugins[n_plugins -1].name = malloc(strlen(de->d_name)+1);
      strncpy(all_plugins[n_plugins -1].name, de->d_name, strlen(de->d_name)+1);

      if(g_module_symbol(all_plugins[n_plugins -1].module, "plugin_call", (gpointer *) &all_plugins[n_plugins -1].call) == FALSE)
        {
          g_warning("Can't load symbol 'plugin_call': %s", g_module_error());
          g_warning("Please note, that this is might caused by an outdated plugin!");
          all_plugins[n_plugins -1].is_working = FALSE;
          continue;
        }
      if(g_module_symbol(all_plugins[n_plugins -1].module, "enable_plugin", (gpointer *) &all_plugins[n_plugins -1].enable) == FALSE)
        {
          g_warning("Can't load symbol 'enable_plugin': %s", g_module_error());
          g_warning("Please note, that this is might caused by an outdated plugin!");
          all_plugins[n_plugins -1].is_working = FALSE;
          continue;
        }
      if(g_module_symbol(all_plugins[n_plugins -1].module, "disable_plugin", (gpointer *) &all_plugins[n_plugins -1].disable) == FALSE)
        {
          g_warning("Can't load symbol 'disable_plugin': %s", g_module_error());
          g_warning("Please note, that this is might caused by an outdated plugin!");
          all_plugins[n_plugins -1].is_working = FALSE;
          continue;
        }

      all_plugins[n_plugins -1].is_working = TRUE;

      //init func
      all_plugins[n_plugins -1].call(root);


      if(is_listed(mods, x, de->d_name) == TRUE)
        continue; //skip ignored mods...

      //if enabled, run
        all_plugins[n_plugins -1].enable(root);

    }
}

void load_plugins(char *path, GtkWidget *root)
{
  root_box = root;
  char module_path[2000];

  //SCAN VARS
  DIR *d;
  struct dirent *de;

  d = opendir(path);
  if(d == NULL)
    {
      g_error("MODULE PATH NOT FOUND");
      return;
    }

    //read blacklist
    GSettings *gmods = g_settings_new("org.jetspace.desktop.panel");
    char *ptr = strdup(g_variant_get_string(g_settings_get_value(gmods, "ignored-plugins"), NULL));
    char **mods = malloc(sizeof(ptr));

    char *mp = strtok(ptr, ";");

    int x = 0;
    while(mp != NULL)
      {
        mods[x] = strdup(mp);
        x++;
        mp = strtok(NULL, ";");
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
      all_plugins = realloc(all_plugins, sizeof(struct plugins) * n_plugins);


      all_plugins[n_plugins -1].module = g_module_open(module_path, G_MODULE_BIND_LAZY);
      g_module_make_resident(all_plugins[n_plugins -1].module); //users are able to create callbacks, because the module would not be unloaded...


      if(all_plugins[n_plugins -1].module == NULL)
        {
          g_error("Loading Module failed: %s", g_module_error());
          all_plugins[n_plugins -1].is_working = FALSE;
        }

      all_plugins[n_plugins -1].name = malloc(strlen(de->d_name)+1);
      strncpy(all_plugins[n_plugins -1].name, de->d_name, strlen(de->d_name)+1);

      if(g_module_symbol(all_plugins[n_plugins -1].module, "plugin_call", (gpointer *) &all_plugins[n_plugins -1].call) == FALSE)
        {
          g_warning("Can't load symbol 'plugin_call': %s", g_module_error());
          g_warning("Please note, that this is might caused by an outdated plugin!");
          all_plugins[n_plugins -1].is_working = FALSE;
          continue;
        }
      if(g_module_symbol(all_plugins[n_plugins -1].module, "enable_plugin", (gpointer *) &all_plugins[n_plugins -1].enable) == FALSE)
        {
          g_warning("Can't load symbol 'enable_plugin': %s", g_module_error());
          g_warning("Please note, that this is might caused by an outdated plugin!");
          all_plugins[n_plugins -1].is_working = FALSE;
          continue;
        }
      if(g_module_symbol(all_plugins[n_plugins -1].module, "disable_plugin", (gpointer *) &all_plugins[n_plugins -1].disable) == FALSE)
        {
          g_warning("Can't load symbol 'disable_plugin': %s", g_module_error());
          g_warning("Please note, that this is might caused by an outdated plugin!");
          all_plugins[n_plugins -1].is_working = FALSE;
          continue;
        }

      all_plugins[n_plugins -1].is_working = TRUE;

      //init func
      all_plugins[n_plugins -1].call(root);


      if(is_listed(mods, x, de->d_name) == TRUE)
        continue; //skip ignored mods...

      //if enabled, run
        all_plugins[n_plugins -1].enable(root);

    }
}

void update_plugins(char *ignored_plugins, GtkWidget *root_box)
{
  char **mods = malloc(sizeof(ignored_plugins));
  char *mp = strtok(ignored_plugins, ";");

  int x = 0;
  while(mp != NULL)
    {
      mods[x] = strdup(mp);
      x++;
      mp = strtok(NULL, ";");
    }



  for(int y = 0; y < n_plugins; y++)
  {
    if(!all_plugins[y].is_working)
      continue;

    if(is_listed(mods, x, all_plugins[y].name) == TRUE)
    {
      all_plugins[y].disable(root_box);
    }
    else
    {
      all_plugins[y].enable(root_box);
    }
  }

  free(mods);

}
