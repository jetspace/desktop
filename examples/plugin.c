/*
Simple plugin for JetSpace Panel
It will show a basic callback...

To compile run 'make plugin'
to remove run 'make no_plugin'

if you have any questions, write me at Marius  Messerschmidt <marius.messerschmidt@googlemail.com>

LICENSE: PUBLIC DOMAIN (CC0)
*/

#include <gtk/gtk.h>    //include GTK
#include <glib.h>       //include Glib
#include <gmodule.h>    //include GModule

#include <side/plugin.h>//Basic Plugin funcions

//  Callback for the button
//  This callback will be called when someone presses the button...
gboolean test_cb(GtkWidget *w, GdkEvent *e, gpointer p)
{
  g_print("Plugin Was Pressed!\n"); //Print to console
  return TRUE;  //return TRUE
}


//This is the Init function
/*
  It will be called at startup, but your plugin should only do some init stuff.
  It should _NOT_ do anything which affects the user, like displaying something...

  A good thing to do here is to check the version...
*/

gboolean example_can_be_activated = FALSE;
gboolean example_active = FALSE;

G_MODULE_EXPORT void plugin_call(GtkWidget *root)
{
  //print a  warning, because this is only an example
  g_warning("WARNING: YOU HAVE INSTALLED THE EXAMPLE PLUGIN(!!)");

  //You can use COMPATIBLE_SINCE, COMPATIBLE_UNTIL and ONLY_FOR_VERSION
  if(!check_version(COMPATIBLE_SINCE, "0.64"))
    return;


  example_can_be_activated = TRUE;

}
GtkWidget *button;

//called when user enables the plugin
//here you should do the visual stuff
G_MODULE_EXPORT void enable_plugin(GtkWidget *root)
{
  if(example_active || !example_can_be_activated)
    return; //return if already active OR is incompatible

  example_active = TRUE;

  if(GTK_IS_BIN(root))
  { //create button
    GtkWidget *child = gtk_bin_get_child(GTK_BIN(gtk_bin_get_child(GTK_BIN(root))));
    button = gtk_button_new_with_label("Press Me (ONLY APPEARS WITH DEMO PLUGIN - DONT USE IT)!");
    gtk_container_add(GTK_CONTAINER(child), button);//add to root box
  }

  g_signal_connect(G_OBJECT(button), "button_press_event", G_CALLBACK(test_cb), NULL); //connect callback to button

}

//called when the user disables the plugin
//you should remove everything your plugin did..
G_MODULE_EXPORT void disable_plugin(GtkWidget *root)
{
  if(!example_active || !example_can_be_activated)
    return; //return if already inactive OR is incompatible

  example_active = FALSE;

  gtk_widget_destroy(button); //remove button
}
