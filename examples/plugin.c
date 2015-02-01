/*
Simple plugin for JetSpace Panel
It will place the panel at the top, not on the bottom, and it will show a basic callback...

To compill run 'make plugin'
to remove run 'make no_plugin'

LICENSE: PUBLIC DOMAIN (CC0)
*/
#include <gtk/gtk.h>
#include <glib.h>
#include <gmodule.h>

gboolean test_cb(GtkWidget *w, GdkEvent *e, gpointer p)
{
  g_print("Plugin Was Pressed");
  return TRUE;
}

G_MODULE_EXPORT void plugin_call(GtkWidget *root)
{
  g_warning("WARNING: YOU HAVE INSTALLED THE EXAMPLE PLUGIN(!!)");  //notice to the user
  gtk_window_move(GTK_WINDOW(root), 0,0);                           //Set window to the top

  GtkWidget *button;

  if(GTK_IS_BIN(root)) {
    GtkWidget *child = gtk_bin_get_child(GTK_BIN(gtk_bin_get_child(GTK_BIN(root))));
    button = gtk_button_new_with_label("Press Me (ONLY APPEARS WITH DEMO PLUGIN - DONT USE IT)!");
    gtk_container_add(GTK_CONTAINER(child), button);

  }
  g_signal_connect(G_OBJECT(button), "button_press_event", G_CALLBACK(test_cb), NULL);
}
