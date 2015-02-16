/*
This file is licensed under the MIT-License
Copyright (c) 2015 Marius Messerschmidt
For more details view file 'LICENSE'
*/

#include "plugin.h"

GtkWidget *side_plugin_get_root_box(GtkWidget *root)
{
  GtkWidget *box = gtk_bin_get_child(GTK_BIN(gtk_bin_get_child(GTK_BIN(root))));
  return box;
}
