/*
This file is licensed under the MIT-License
Copyright (c) 2015 Marius Messerschmidt
For more details view file 'LICENSE'
*/

#ifndef SIDE_TIMELABEL_HEADER
#define SIDE_TIMELABEL_HEADER

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

#define SIDE_TIMELABEL_TYPE                   (side_timelabel_get_type())
#define SIDE_TIMELABEL(obj)              (G_TYPE_CHECK_INSTANCE_CAST((obj), SIDE_TIMELABEL_TYPE, SiDETimelabel))
#define SIDE_TIMELABEL_CLASS(klass)           (G_TYPE_CHECK_CLASS_CAST((klass), SIDE_TIMELABEL_CLASS, SiDETimelabelClass))
#define SIDE_IS_TIMELABEL(obj)                (G_TYPE_CHECK_INSTANCE_TYPE((obj), SIDE_TIMELABEL_TYPE))
#define SIDE_TIMELABEL_IS_CLASS(klass)        (G_TYPE_CHECK_CLASS_TYPE((klass), SIDE_TIMELABEL_CLASS))

typedef struct _SiDETimelabel SiDETimelabel;
typedef struct _SiDETimelabelClass SiDETimelabelClass;

struct _SiDETimelabel
{
  GtkLabel parent_instance;
};

struct _SiDETimelabelClass
{
  GtkLabelClass parent_class;
};

GtkWidget *side_timelabel_new(time_t timecode);

//type
GType side_timelabel_get_type(void);

// functions

void side_timelabel_set_time(SiDETimelabel *timelabel, time_t timecode);
time_t side_timelael_get_time(SiDETimelabel *timelabel);


#endif
