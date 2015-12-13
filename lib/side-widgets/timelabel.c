/*
This file is licensed under the MIT-License
Copyright (c) 2015 Marius Messerschmidt
For more details view file 'LICENSE'
*/

#include "timelabel.h"

#define SIDE_TIMELABEL_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), SIDE_TIMELABEL_TYPE, SiDETimelabelPrivate))

typedef struct _SiDETimelabelPrivate SiDETimelabelPrivate;

struct _SiDETimelabelPrivate
{
	time_t t;
};

G_DEFINE_TYPE(SiDETimelabel, side_timelabel, GTK_TYPE_LABEL);


static void side_timelabel_class_init(SiDETimelabelClass *klass)
{
    g_type_class_add_private(klass, sizeof(SiDETimelabelPrivate));
}

static void side_timelabel_init(SiDETimelabel *timelabel)
{
  SiDETimelabelPrivate *p = SIDE_TIMELABEL_PRIVATE(timelabel);
  p->t = 0;
}

GtkWidget *side_timelabel_new(time_t timecode)
{
  SiDETimelabel *ret = g_object_new(side_timelabel_get_type(), NULL);
  side_timelabel_set_time(ret, timecode);
  return GTK_WIDGET(ret);
}

void side_timelabel_update(SiDETimelabel *timelabel)
{
  g_return_if_fail(SIDE_IS_TIMELABEL(timelabel));
  SiDETimelabelPrivate *p = SIDE_TIMELABEL_PRIVATE(timelabel);
  GtkLabel *label = GTK_LABEL(timelabel);

  char *timestr = g_malloc(40);
  struct tm tm_data;
  localtime_r(&p->t, &tm_data);

  time_t now = time(NULL);
  struct tm tm_now;
  localtime_r(&now, &tm_now);

  if(tm_data.tm_yday == tm_now.tm_yday && tm_data.tm_year == tm_now.tm_year)
  {// Same Day
      strftime(timestr, 40, "%H:%M", &tm_data);
  }
  else if(tm_data.tm_year == tm_now.tm_year)
  {// Same Year
      strftime(timestr, 40, "%d %b", &tm_data);
  }
  else
  {
    strftime(timestr, 40, "%b %Y", &tm_data);
  }


  gtk_label_set_text(label, timestr);
  g_free(timestr);
}

void side_timelabel_set_time(SiDETimelabel *timelabel, time_t timecode)
{
  g_return_if_fail(SIDE_IS_TIMELABEL(timelabel));
  SiDETimelabelPrivate *p = SIDE_TIMELABEL_PRIVATE(timelabel);
  p->t = timecode;
  side_timelabel_update(timelabel);
}

time_t side_timelael_get_time(SiDETimelabel *timelabel)
{
  g_return_val_if_fail(SIDE_IS_TIMELABEL(timelabel), 0);
  SiDETimelabelPrivate *p = SIDE_TIMELABEL_PRIVATE(timelabel);
  return p->t;
}
