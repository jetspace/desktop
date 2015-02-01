/*
This file is licensed under the MIT-License
Copyright (c) 2015 Marius Messerschmidt
For more details view file 'LICENSE'
*/

#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <stdlib.h>

#include "../shared/strdup.h"
#include "../shared/context.h"
#include "../shared/run.h"
#include "../shared/plugins.h"


#define PANEL_HEIGHT 35


//callbacks
gboolean button_event(GtkWidget *w, GdkEventButton *e, GtkWidget *menu);
gboolean clicked_item(GtkWidget *w, GdkEventButton *e, gpointer p);


gint total_elements = 0;

typedef struct
{
  char icon[100];
  char exec[100];
  GtkWidget *button;
}PanelEntry;

PanelEntry *elements;

void add_new_element(GtkWidget *box, char *icon, char *exec, gint state);

int main(int argc, char **argv)
{
  gtk_init(&argc, &argv);

  GdkScreen *screen = gdk_screen_get_default();

  //setup elements
  elements = malloc(sizeof(PanelEntry));

  //Setup Main Window
  GtkWidget *panel, *event, *menu;
  panel = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_decorated(GTK_WINDOW(panel), FALSE);
  gtk_window_set_type_hint(GTK_WINDOW (panel), GDK_WINDOW_TYPE_HINT_DOCK);
  gtk_window_resize(GTK_WINDOW(panel), gdk_screen_get_width(screen), PANEL_HEIGHT);
  gtk_window_move(GTK_WINDOW(panel), 0, gdk_screen_get_height(screen) - PANEL_HEIGHT);
  gtk_container_set_border_width(GTK_CONTAINER(panel), 1);
  //TODO:
    //prevent overlapping


  //setup context menu
  menu = gtk_menu_new();
  add_context_menu_pannel(menu, box);

  //Event box
  event = gtk_event_box_new();
  g_signal_connect(G_OBJECT(event), "button_press_event", G_CALLBACK(button_event), menu);
  gtk_widget_set_events(event, GDK_BUTTON_PRESS_MASK);
  //gtk_widget_realize(event);

  //add gtk box
  GtkWidget *box;
  box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
  gtk_container_add(GTK_CONTAINER(event), box);
  gtk_container_add(GTK_CONTAINER(panel), event);

  //get app list
  GSettings *apps = g_settings_new("org.jetspace.desktop.panel");
  char *app_list = strdup(g_variant_get_string(g_settings_get_value(apps, "apps"), NULL));

  //phrase app list
  char *e, *i, *a; //exec icon active
  i = strtok(app_list, ":");
  e = strtok(NULL, ":");
  a = strtok(NULL, ";");
  while(i != NULL && e != NULL)
    {
      add_new_element(box, i, e, atoi(a));
      i = strtok(NULL, ":");
      e = strtok(NULL, ":");
      a = strtok(NULL, ";");
    }



  //call plugin loader
  load_plugins("/usr/lib/jetspace/panel/plugins/", panel);
  gtk_widget_show_all(panel);
  gtk_main();
  return 0;
}

gboolean button_event(GtkWidget *w, GdkEventButton *e, GtkWidget *menu)
{
  if((e->button == 3) && (e->type ==GDK_BUTTON_PRESS))
    {
      gtk_menu_popup(GTK_MENU(menu), NULL, NULL, NULL, NULL, e->button, e->time);
    }

  return FALSE;
}

void add_new_element(GtkWidget *box, char *icon, char *exec, gint state)
{
  if(state != 1)
    return;


  total_elements++;
  elements = realloc(elements, sizeof(PanelEntry) * total_elements);

  elements[total_elements -1].button = gtk_button_new_from_icon_name(icon, GTK_ICON_SIZE_SMALL_TOOLBAR);
  strncpy(elements[total_elements -1].icon, icon, 100);
  strncpy(elements[total_elements -1].exec, exec, 100);

  gtk_button_set_relief (GTK_BUTTON(elements[total_elements -1].button), GTK_RELIEF_NONE);
  g_signal_connect(G_OBJECT(elements[total_elements -1].button), "button_press_event", G_CALLBACK(clicked_item), NULL);

  gtk_container_add(GTK_CONTAINER(box), elements[total_elements -1].button);

}

gboolean clicked_item(GtkWidget *w, GdkEventButton *e, gpointer p)
{
  for (int x = 0; x < total_elements; x++)
    {
      if(elements[x].button == w)
        {
          system(elements[x].exec);
          return FALSE;
        }
    }

  g_warning("[PANEL] - Unable to find button -> Memory may corrupted!");
  return FALSE;
}
