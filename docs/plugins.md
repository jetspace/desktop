# Which apps can handle plugins?
 > At this time, only the panel is able to handle plugins.

# How to write plugins:

## Intro

A plugin will be loaded during the startup of the panel. If the users
disables you plugin, it won't be loaded.

_PLEASE NOTE:_ A plugin can only be refreshed with restarting the panel.

## Enviroment

* Plugins are placed in the following path:

    * `/usr/lib/jetspace/panel/plugins/`

* Plugins are SharedObjects
    * Build with:
        * `gcc -o your_plugin.so --shared -fPIC source.c $(pkg-config --libs --cflags glib-2.0 gmodule-2.0 gtk+-3.0) -lside-plugin`
        * You can ignore `gtk+-3.0` if you don't use any widgets.

For best testing, kill panel and launch it from Terminal, so you can refresh
plugins easily.

`killall side-panel`

and `side-panel`

You can exit the panel later with `CTRL` + `C`

## Source

You have to include the following headers:

`#include <gmodule.h>`
`#include <gtk.h> //only if you use widgets`

Then you need a 'modloader' function:

_It must be exactly like this:_

`G_MODULE_EXPORT void plugin_call(GtkWidget *root)`

`root` is the GtkWindow of the Panel.

## Tips

You can use the header `#include <side/plugin.h>` to get the following
function:

`side_plugin_get_root_box(root);`

It will return the main container, where all widgets are added.
