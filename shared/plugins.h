/*
This file is licensed under the MIT-License
Copyright (c) 2015 Marius Messerschmidt
For more details view file 'LICENSE'
*/

#include <gmodule.h>
#include <gtk/gtk.h>
#include <sys/types.h>
#include <dirent.h>
#include <glib.h>
#include "../shared/listed.h"
#include "../shared/config.h"

#ifdef LOAD_PYTHON_MODULES
#include <Python.h>
#endif

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

#ifdef LOAD_PYTHON_MODULES
    struct py_mods {
        PyObject *module;
        PyObject *enable;
        PyObject *disable;
        char *name;
    };
    struct py_mods *py_plugins;
    int n_py_plugins = 0;
#endif

GtkWidget *root_box;

#ifdef LOAD_PYTHON_MODULES
static void call_py_func(PyObject *obj)
{
    if(obj && PyCallable_Check(obj))
    {
        PyObject_CallObject(obj, NULL);
    }
}
#endif
void load_plugins(char *path, GtkWidget *root, char *settings)
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
    GSettings *gmods = g_settings_new(settings);
    char *ptr = strdup(g_variant_get_string(g_settings_get_value(gmods, "ignored-plugins"), NULL));
    char **mods = malloc(strlen(ptr));

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
      if(all_plugins == NULL)
      {
        g_warning("Memory corrupted while loading plugins!");
      }

      all_plugins[n_plugins -1].module = g_module_open(module_path, G_MODULE_BIND_LAZY);



      if(all_plugins[n_plugins -1].module == NULL)
        {
          g_warning("Loading Module failed: %s", g_module_error());
          all_plugins[n_plugins -1].is_working = FALSE;
          continue;
        }

      g_module_make_resident(all_plugins[n_plugins -1].module); //users are able to create callbacks, because the module would not be unloaded...

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

#ifdef LOAD_PYTHON_MODULES
    //Load Python
    Py_Initialize();
    PySys_SetPath(L"/usr/share/jetspace/desktop/panel/python/");

    gchar **toload = g_settings_get_strv(gmods, "python-modules");
    gchar *iter = toload[0];
    for(int i = 1; iter != NULL; i++)
    {
        g_warning("#PYMOD# \t >> %s", iter);

        PyObject *modname = PyUnicode_DecodeFSDefault(iter);

        n_py_plugins++;
        py_plugins = realloc(py_plugins, sizeof(*py_plugins) * n_py_plugins);

        py_plugins[n_py_plugins -1].module = PyImport_Import(modname);
        py_plugins[n_py_plugins -1].name = g_strdup(iter);

        py_plugins[n_py_plugins -1].enable = PyObject_GetAttrString(py_plugins[n_py_plugins -1].module, "enable");
        call_py_func(py_plugins[n_py_plugins -1].enable);
        py_plugins[n_py_plugins -1].disable = PyObject_GetAttrString(py_plugins[n_py_plugins -1].module, "disable");
        Py_DECREF(modname); // Drop ref on name
        iter = toload[i];
    }


#endif
}

void update_plugins(char *ignored_plugins, GtkWidget *root_box)
{
  char **mods = malloc(strlen(ignored_plugins));
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
