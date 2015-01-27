/*
Simple plugin for JetSpace Panel
It will place the panel at the top, not on the bottom

To compill run 'make plugin'
to remove run 'make no_plugin'
*/
#include <gtk/gtk.h>
#include <glib.h>
#include <gmodule.h>

G_MODULE_EXPORT void plugin_call(GtkWidget *root)
{
  g_warning("WARNING: YOU HAVE INSTALLED THE EXAMPLE PLUGIN(!!)");  //notice to the user
  gtk_window_move(GTK_WINDOW(root), 0,0);                           //Set window to the top
}
