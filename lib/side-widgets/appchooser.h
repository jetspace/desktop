/*
This file is licensed under the MIT-License
Copyright (c) 2015 Marius Messerschmidt
For more details view file 'LICENSE'
*/

#ifndef SIDE_APPCHOOSER_HEADER
#define SIDE_APPCHOOSER_HEADER

#define _POSIX_SOURCE

#include <time.h>
#include <glib-object.h>
#include <gtk/gtk.h>

/*
  Constructs a new label which will display the time
  <time> in a nice formart

  PLANNED:

    -> Time since <time>
*/

#define SIDE_APPCHOOSER_TYPE                   (side_app_chooser_get_type())
#define SIDE_APPCHOOSER(obj)                   (G_TYPE_CHECK_INSTANCE_CAST((obj), SIDE_APPCHOOSER_TYPE, SiDEAppChooser))
#define SIDE_APPCHOOSER_CLASS(klass)           (G_TYPE_CHECK_CLASS_CAST((klass), SIDE_APPCHOOSER_CLASS, SiDEAppChooserClass))
#define SIDE_IS_APPCHOOSER(obj)                (G_TYPE_CHECK_INSTANCE_TYPE((obj), SIDE_APPCHOOSER_TYPE))
#define SIDE_APPCHOOSER_IS_CLASS(klass)        (G_TYPE_CHECK_CLASS_TYPE((klass), SIDE_APPCHOOSER_CLASS))

typedef struct _SiDEAppChooser SiDEAppChooser;
typedef struct _SiDEAppChooserClass SiDEAppChooserClass;

struct _SiDEAppChooser
{
  GtkDialog parent_instance;
};

struct _SiDEAppChooserClass
{
  GtkDialogClass parent_class;
};

GtkWidget *side_app_chooser_new(void);

//type
GType side_app_chooser_get_type(void);

#endif
