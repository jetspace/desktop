/*
This file is licensed under the MIT-License
Copyright (c) 2015 Marius Messerschmidt
For more details view file 'LICENSE'
*/


#include <gtk/gtk.h>
#include <stdlib.h>

GtkWidget *win;

void help(void)
{
  g_print("SIDE - popup utility\n");
  g_print("USAGE: side-notify-popup $ID $ICON $HEADING $BODY\n");
  g_print("It will display a notification to the user!\n");

}

gboolean destroy(GtkWidget *w, GdkEvent *e, gpointer *p)
{
  gtk_main_quit();
  return FALSE;
}

double opacity = 0.85;

gboolean unload(gpointer data)
{
  opacity = opacity - 0.01;
  if(opacity > 0.1)
    gtk_widget_set_opacity(win,  opacity);
  else
    {
      gtk_widget_destroy(win);
      return destroy(NULL, NULL, NULL);
    }

  return TRUE;
}

gboolean button_event(GtkWidget *w, GdkEventButton *e, GtkWidget *menu)
{
  g_timeout_add(3, unload, NULL);
  return FALSE;
}

int main(int argc, char **argv)
{
  if(argc < 5)
    {
      help();
      return -1;
    }

  gtk_init(&argc, &argv);

  char *head = argv[3];
  char *body = argv[4];
  char *icon = argv[2];
  int   id   = atoi(argv[1]);


  GdkScreen *screen = gdk_screen_get_default();



  win = gtk_window_new(GTK_WINDOW_POPUP);
  gtk_window_set_decorated(GTK_WINDOW(win), FALSE);
  gtk_container_set_border_width(GTK_CONTAINER(win), 10);
  g_signal_connect(G_OBJECT(win), "delete-event", G_CALLBACK(destroy), NULL);
  gtk_window_stick(GTK_WINDOW(win));
  gtk_widget_set_opacity(win,  opacity);

  GtkWidget *event = gtk_event_box_new();
  g_signal_connect(G_OBJECT(event), "button_press_event", G_CALLBACK(button_event), NULL);
  gtk_widget_set_events(event, GDK_BUTTON_PRESS_MASK);

  gtk_container_add(GTK_CONTAINER(win), event);

  GtkWidget *head_label = gtk_label_new(head);
  GtkWidget *layout_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
  GtkWidget *text_label = gtk_label_new(body);
  gtk_label_set_use_markup(GTK_LABEL(text_label), TRUE);
  gtk_label_set_markup(GTK_LABEL(text_label), body);
  gtk_label_set_line_wrap(GTK_LABEL(text_label), TRUE);
  gtk_label_set_line_wrap_mode(GTK_LABEL(text_label), PANGO_WRAP_WORD);

  gtk_box_pack_start(GTK_BOX(layout_box), head_label, TRUE, FALSE, 2);
  gtk_box_pack_start(GTK_BOX(layout_box), text_label, TRUE, FALSE, 2);

  GtkWidget *second_layout = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
  GtkWidget *image = gtk_image_new_from_icon_name(icon, GTK_ICON_SIZE_DIALOG);

  gtk_container_add(GTK_CONTAINER(second_layout), image);
  gtk_container_add(GTK_CONTAINER(second_layout), layout_box);

  gtk_container_add(GTK_CONTAINER(event), second_layout);

  gtk_widget_show_all(win);
  gtk_window_move(GTK_WINDOW(win), (gdk_screen_get_width(screen) - gdk_window_get_width(gtk_widget_get_window(GTK_WIDGET(win)))) - 50, 50);

  gtk_main();
}
