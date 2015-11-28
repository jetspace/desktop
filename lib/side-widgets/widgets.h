/*
This file is licensed under the MIT-License
Copyright (c) 2015 Marius Messerschmidt
For more details view file 'LICENSE'
*/

#ifndef _SIDE_WIDGETS
#define _SIDE_WIDGETS
#include <gtk/gtk.h>
#include <string.h>
#include <stdio.h>

/* -= MODES =- */
enum
{
  SIDE_APPLICATION_MODE_SETTINGS = 0,
  SIDE_APPLICATION_MODE_PANEL,
  SIDE_APPLICATION_MODE_WALLPAPER,
  SIDE_APPLICATION_MODE_TOOL,
  SIDE_APPLICATION_MODE_POPUP,
  SIDE_APPLICATION_MODE_OTHER
};

extern int mode; // Contains the mode of the application

extern void side_set_application_mode(int type);

// WIDGETS
extern GtkWidget *side_gtk_label_new(char *text);
extern GtkWidget *side_gtk_menu_new(void);
extern GtkWidget *side_gtk_image_new_from_file(char *file);


#endif
