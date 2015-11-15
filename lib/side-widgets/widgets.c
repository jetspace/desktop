/*
This file is licensed under the MIT-License
Copyright (c) 2015 Marius Messerschmidt
For more details view file 'LICENSE'
*/
#include "widgets.h"
int mode;

void side_set_application_mode(int type)
{
  mode = type;
}

GtkWidget *side_gtk_label_new(char *text)
{
  GtkWidget *label;
  switch (mode)
  {
    case SIDE_APPLICATION_MODE_SETTINGS:
      label = gtk_label_new(text);
      gtk_label_set_xalign(GTK_LABEL(label), 0.0);
      return label;
    break;

    default:
      label = gtk_label_new(text);
    break;
  }

  return label;
}
