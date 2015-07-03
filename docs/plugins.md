# Which apps can handle plugins?
 > At this time, only the panel is able to handle plugins.

# How to write plugins:

## Intro

A plugin will be loaded during the startup of the panel.
But it can be activated / deactivated  during runtime.

_PLEASE NOTE:_ A plugin can only be reloaded with restarting the panel.

## Enviroment

* Plugins are placed in the following path:

    * `/usr/lib/jetspace/panel/plugins/`

* Plugins are SharedObjects
    * Build with:
        * `gcc -o your_plugin.so --shared -fPIC source.c $(pkg-config --libs --cflags glib-2.0 gmodule-2.0 gtk+-3.0) -lside-plugin`

For best testing, kill panel and launch it from Terminal, so you can refresh
plugins easily.

`killall side-panel`

and `side-panel`

You can exit the panel later with `CTRL` + `C`

## Source

You have to include the following headers:

`#include <gmodule.h>`
`#include <gtk.h>`


### init

Init is the first stage of a plugin. It will called on _ALL_ installed plugins, no matter
if it is enabled or not. So in this stage, you should only perform basic tasks, like checking the version.

Then you need a 'init' function:

_It must be exactly like this:_

`G_MODULE_EXPORT void plugin_call(GtkWidget *root)`

`root` is the GtkWindow of the Panel.

### enabled

`G_MODULE_EXPORT void enable_plugin(GtkWidget *root)`

Will be called when the user enables your plugin.

Here you should draw your widgets and/or modify something

### disabled

`G_MODULE_EXPORT void disable_plugin(GtkWidget *root)`

Will be called when the user disables your plugin.

Here you should hide/destroy all your widgets and/or restore the defaults.

## Tips

You can use the header `#include <side/plugin.h>` to get the following
functions:



### side_plugin_get_root_box(GtkWidget \*root)

`side_plugin_get_root_box(GtkWidget *root);`

It will return the main container, where all widgets are added.


### check_version(int type, char \*target)

`check_version(int type, char *target);`

Check if your plugin is compatible, it will return a gboolean (TRUE/FALSE).

For type use one of the following parameters:

 * COMPATIBLE_SINCE  -> Your plugin works with all versions since »target«
 * COMPATIBLE_UNTIL  -> Your plugin works with all versions before »target«
 * ONLY_FOR_VERSION  -> Your plugin only works with version »target«

 Target should be a string like `"0.50"` or `"0.60-rc2"`, etc.
