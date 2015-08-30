#include <gtk/gtk.h>
#ifndef _SIDE_NOTIFY_POPUP
#define _SIDE_NOTIFY_POPUP

int number_of_notifications = 0;
typedef struct notification
{
  GtkWidget *win;
  int id;
  float opacity;
  gboolean active;
}Notification;

Notification *notifys;


gboolean unload(gpointer data)
{
  int w =  GPOINTER_TO_INT(data);
  GtkWidget *win = notifys[w].win;
  notifys[w].opacity = notifys[w].opacity - 0.01;
  if(notifys[w].opacity > 0.1)
    gtk_widget_set_opacity(win,  notifys[w].opacity);
  else
    {
      gtk_widget_destroy(win);
      notifys[w].active = FALSE;
      return FALSE;
    }
  return TRUE;
}

int lookup_notify_by_window(GtkWidget *w)
{
  for (int x = 0; x < number_of_notifications; x++)
    if(notifys[x].win == w)
      return x;

  return -1;
}

gboolean clicked_notification(GtkWidget *w, GdkEventButton *e, GtkWidget *menu)
{
  g_timeout_add(3, unload,  GINT_TO_POINTER(lookup_notify_by_window(w)));
  return FALSE;
}

void cleanup_notify_stack(gboolean debug)
{
  int final_count = 0;
  for(int x=0; x < number_of_notifications; x++)
  {
    if(notifys[x].active)
      final_count++;
  }
  Notification *buffer = malloc(sizeof(Notification) * final_count);
  int y = 0;
  for(int x = 0; x < number_of_notifications; x++)
  {
    if(notifys[x].active)
    {
      buffer[y] = notifys[x];
      y++;
    }
  }

  free(notifys);
  number_of_notifications = final_count;
  notifys = buffer;

}


void side_notify_popup_create(int id,char *app,char *icon,char *sum,char *body, gboolean debug)
{
  number_of_notifications += 1;
  notifys = realloc(notifys, sizeof(Notification) * number_of_notifications);
  if(debug)
  {
    g_print(" :: allocating new notification (ID %d)\n", id);
    g_print("  :> Structure size = %ld\n", sizeof(notifys[0]) * number_of_notifications);
  }
  notifys[number_of_notifications -1].id = id;
  notifys[number_of_notifications -1].opacity = 1.0;
  notifys[number_of_notifications -1].active = TRUE;

  notifys[number_of_notifications -1].win = gtk_window_new(GTK_WINDOW_POPUP);
  GdkScreen *screen = gdk_screen_get_default();
  gtk_window_set_decorated(GTK_WINDOW(notifys[number_of_notifications -1].win), FALSE);
  gtk_container_set_border_width(GTK_CONTAINER(notifys[number_of_notifications -1].win), 10);
  gtk_window_stick(GTK_WINDOW(notifys[number_of_notifications -1].win));

  GtkWidget *event = gtk_event_box_new();
  g_signal_connect(G_OBJECT(notifys[number_of_notifications -1].win), "button-press-event", G_CALLBACK(clicked_notification), NULL);
  gtk_widget_set_events(event, GDK_BUTTON_PRESS_MASK);
  gtk_container_add(GTK_CONTAINER(notifys[number_of_notifications -1].win), event);

  GtkWidget *head_label = gtk_label_new(sum);
  GtkWidget *layout_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
  GtkWidget *text_label = gtk_label_new(body);
  gtk_label_set_use_markup(GTK_LABEL(text_label), TRUE);
  gtk_label_set_markup(GTK_LABEL(text_label), body);
  gtk_label_set_line_wrap(GTK_LABEL(text_label), TRUE);
  gtk_label_set_line_wrap_mode(GTK_LABEL(text_label), PANGO_WRAP_WORD);
  gtk_box_pack_start(GTK_BOX(layout_box), head_label, FALSE, FALSE, 2);
  gtk_box_pack_start(GTK_BOX(layout_box), text_label, FALSE, FALSE, 2);

  GtkWidget *second_layout = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
  GtkWidget *image = gtk_image_new_from_icon_name(icon, GTK_ICON_SIZE_DIALOG);
  gtk_box_pack_start(GTK_BOX(second_layout), image, TRUE, TRUE, 0);
  gtk_box_pack_end(GTK_BOX(second_layout), layout_box, FALSE, FALSE, 0);

  gtk_container_add(GTK_CONTAINER(event), second_layout);
  gtk_widget_show_all(notifys[number_of_notifications -1].win);
  gtk_window_move(GTK_WINDOW(notifys[number_of_notifications -1].win), (gdk_screen_get_width(screen) - gdk_window_get_width(gtk_widget_get_window(GTK_WIDGET(notifys[number_of_notifications -1].win)))) - 50, 50);

  //do some cleanup
  cleanup_notify_stack(debug);

}


#endif
