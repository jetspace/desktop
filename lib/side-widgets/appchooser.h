/*
This file is licensed under the MIT-License
Copyright (c) 2015 Marius Messerschmidt
For more details view file 'LICENSE'
*/

#ifndef SIDE_APP_CHOOSER_HEADER
#define SIDE_APP_CHOOSER_HEADER

#define _POSIX_SOURCE

#include <time.h>
#include <glib-object.h>
#include <gtk/gtk.h>


#define SIDE_APP_CHOOSER_TYPE                   (side_app_chooser_get_type())
#define SIDE_APP_CHOOSER(obj)                   (G_TYPE_CHECK_INSTANCE_CAST((obj), SIDE_APP_CHOOSER_TYPE, SiDEAppChooser))
#define SIDE_APP_CHOOSER_CLASS(klass)           (G_TYPE_CHECK_CLASS_CAST((klass), SIDE_APP_CHOOSER_CLASS, SiDEAppChooserClass))
#define SIDE_IS_APP_CHOOSER(obj)                (G_TYPE_CHECK_INSTANCE_TYPE((obj), SIDE_APP_CHOOSER_TYPE))
#define SIDE_APP_CHOOSER_IS_CLASS(klass)        (G_TYPE_CHECK_CLASS_TYPE((klass), SIDE_APP_CHOOSER_CLASS))

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

typedef struct _SiDEAppChooserResult
{
  char *exec;
  char *name;
  char *icon;
}SiDEAppChooserResult;

GtkWidget *side_app_chooser_new(void);

//type
GType side_app_chooser_get_type(void);

char *side_app_chooser_get_name(SiDEAppChooser *ac);
char *side_app_chooser_get_icon(SiDEAppChooser *ac);
char *side_app_chooser_get_exec(SiDEAppChooser *ac);

#endif
