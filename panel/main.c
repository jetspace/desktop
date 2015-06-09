/*
This file is licensed under the MIT-License
Copyright (c) 2015 Marius Messerschmidt
For more details view file 'LICENSE'
*/

#define _XOPEN_SOURCE 700

#include "../shared/strut.h"
#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <stdlib.h>
#include <ctype.h>
#include <side/apps.h>
#include <side/log.h>


#include <errno.h>


#define PANEL_HEIGHT 35
#define SEARCH_APP "side-search &"

#include "../shared/strdup.h"
#include "../shared/context.h"
#include "../shared/run.h"
#include "../shared/plugins.h"
#include "../shared/windows.h"
#include "../shared/transparent.h"
#include "../shared/css.h"


GtkWidget *app_menu_button;
GtkWidget *search_entry;



//callbacks
gboolean button_event(GtkWidget *w, GdkEventButton *e, GtkWidget *menu);
gboolean clicked_item(GtkWidget *w, GdkEventButton *e, gpointer p);
gboolean app_menu(GtkWidget *w, GdkEventButton *e, GtkWidget *menu);
gboolean update_icons(GSettings *s, gchar *key, GtkWidget *box);
gboolean update_apps(gpointer data);
gboolean run_app(GtkWidget *w, GdkEvent *e, gpointer *p);


gboolean check_name(char *text);


gint total_elements = 0;
gint total_apps = 0;

typedef struct
{
  char icon[100];
  char exec[100];
  GtkWidget *button;
}PanelEntry;

typedef struct
{
  char *icon;
  char *exec;
  gboolean terminal;
  GtkWidget *item;
}Apps;

Apps        *apps;
PanelEntry  *elements;

void add_new_element(GtkWidget *box, char *icon, char *exec, char *tooltip,  gint state);
void setup_panel(GtkWidget *box, char *app_list);
void create_app_menu(GtkWidget *box);
void running_apps(GtkWidget *box);

int main(int argc, char **argv)
{
  gtk_init(&argc, &argv);

  GSettings *apps = g_settings_new("org.jetspace.desktop.panel");

  GdkDisplay *display;
  GdkScreen *screen;
  display = gdk_display_get_default ();
  screen = gdk_display_get_default_screen (display);


  use_css(apps);



  side_log_set_log_level_from_enviroment();
  side_log_debug("Loglevel loaded from enviroment");

  //GdkScreen *screen = gdk_screen_get_default();

  //setup elements
  elements = malloc(sizeof(PanelEntry));
  apps = malloc(sizeof(Apps));

  //Setup Main Window
  GtkWidget *panel, *event, *menu;
  panel = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_decorated(GTK_WINDOW(panel), FALSE);
  gtk_window_set_title(GTK_WINDOW(panel), "");
  gtk_window_set_type_hint(GTK_WINDOW (panel), GDK_WINDOW_TYPE_HINT_DOCK);
  gtk_window_resize(GTK_WINDOW(panel), gdk_screen_get_width(screen), PANEL_HEIGHT);
  gtk_window_move(GTK_WINDOW(panel), 0, gdk_screen_get_height(screen) - PANEL_HEIGHT);
  gtk_container_set_border_width(GTK_CONTAINER(panel), 1);
  gtk_window_stick(GTK_WINDOW(panel));


  //setup context menu
  menu = gtk_menu_new();
  add_context_menu_pannel(menu, box);

  //Event box
  event = gtk_event_box_new();
  g_signal_connect(G_OBJECT(event), "button_press_event", G_CALLBACK(button_event), menu);
  gtk_widget_set_events(event, GDK_BUTTON_PRESS_MASK);

  //add gtk box
  GtkWidget *box;
  box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
  gtk_container_add(GTK_CONTAINER(event), box);
  gtk_container_add(GTK_CONTAINER(panel), event);

  gtk_widget_set_name(GTK_WIDGET(panel), "SiDEPanel");
  gtk_widget_set_name(GTK_WIDGET(box), "SiDEPanelBox");



  GdkGeometry hints;
  hints.max_width = gdk_screen_get_width(screen);
  gtk_window_set_geometry_hints(GTK_WINDOW(panel), GTK_WIDGET(box), &hints, GDK_HINT_MAX_SIZE);


  //get app list
  apps = g_settings_new("org.jetspace.desktop.panel");
  char *app_list = strdup(g_variant_get_string(g_settings_get_value(apps, "apps"), NULL));

  GSettings *menuS = g_settings_new("org.jetspace.desktop.panel");

  if(g_variant_get_boolean(g_settings_get_value(menuS, "show-app-menu")))
    create_app_menu(box);



  setup_panel(box, app_list);
  g_signal_connect(G_OBJECT(apps), "changed", G_CALLBACK(update_icons), box);

  if(g_variant_get_boolean(g_settings_get_value(menuS, "show-window-list")))
  {
      running_apps(box);
      g_timeout_add(g_variant_get_int32(g_settings_get_value(menuS, "window-list-refresh-rate")), update_apps, NULL);
  }

  //call plugin loader
  load_plugins("/usr/lib/jetspace/panel/plugins/", panel);

 if(g_variant_get_boolean(g_settings_get_value(apps, "use-custom-theme")))
    {
        gtk_widget_set_app_paintable(panel, TRUE);
        screen_changed(panel, NULL, NULL);
        gtk_widget_set_app_paintable(box, TRUE);
        screen_changed(box, NULL, NULL);
    }



  gtk_widget_show_all(panel);
  set_struts(panel, STRUT_BOTTOM, PANEL_HEIGHT);
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

void add_new_element(GtkWidget *box, char *icon, char *exec, char *tooltip,  gint state)
{
  if(state != 1)
    return;


  total_elements++;
  elements = realloc(elements, sizeof(PanelEntry) * total_elements);

  elements[total_elements -1].button = gtk_button_new_from_icon_name(icon, GTK_ICON_SIZE_SMALL_TOOLBAR);
  strncpy(elements[total_elements -1].icon, icon, 100);
  strncpy(elements[total_elements -1].exec, exec, 100);


  side_log_debug("Adding new element:");
  side_log_debug(icon);

  gtk_button_set_relief (GTK_BUTTON(elements[total_elements -1].button), GTK_RELIEF_NONE);
  g_signal_connect(G_OBJECT(elements[total_elements -1].button), "button_press_event", G_CALLBACK(clicked_item), NULL);
  gtk_widget_set_tooltip_text(elements[total_elements -1].button, tooltip);
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

  side_log_warning("[PANEL] - Unable to find button -> Memory may corrupted!");
  return FALSE;
}

void setup_panel(GtkWidget *box, char *app_list)
{
  GList *ch, *iter;

  ch = gtk_container_get_children(GTK_CONTAINER(box));
  for(iter = ch; iter != NULL; iter = g_list_next(iter))
    if(GTK_WIDGET(iter->data) != app_menu_button)
      gtk_widget_destroy(GTK_WIDGET(iter->data));
  g_list_free(ch);


  //phrase app list
  char *e, *i, *a; //exec icon active
  i = strdup(strtok(app_list, ":"));
  e = strdup(strtok(NULL, ":"));
  a = strdup(strtok(NULL, ";"));
  while(i != NULL && e != NULL)
    {
      add_new_element(box, i, e, e ,atoi(a));
      i = strtok(NULL, ":");
      e = strtok(NULL, ":");
      a = strtok(NULL, ";");
    }


}

gboolean update_icons(GSettings *s, gchar *key, GtkWidget *box)
{
    char *apps = strdup(g_variant_get_string(g_settings_get_value(s, "apps"), NULL));
    elements =  realloc(elements, 0);
    total_elements = 0;
    setup_panel(box, apps);
    gtk_widget_show_all(box);

}

GtkWidget *running_box = NULL;
GtkWidget *top_box = NULL;

gboolean update_apps(gpointer data)
{
    running_apps(top_box);
    return TRUE;
}

struct OpenWindows {

    Window win;
    GtkWidget *button;
};

static gboolean toggle_win(GtkWidget *wid, GdkEvent *e, gpointer p)
{
    char *querry = strdup(gtk_button_get_label(GTK_BUTTON(wid)));

    Display *d = XOpenDisplay(NULL);
    unsigned long len;
    Window *list = list_windows(d, &len);;

    char *ptr = NULL;
    for (int i = 0; i < (int) len; i++)
    {
        ptr = get_window_name(d, list[i]);
            if(ptr == NULL)
                continue;
            if(strcmp(ptr, querry) == 0)
            {
                unhide(d, list[i]);
            }
    }

    free(querry);
    XCloseDisplay(d);
}

void running_apps(GtkWidget *box)
{

    if(running_box != NULL)
        {
            //clear list
            GList *ch, *iter;
            ch = gtk_container_get_children(GTK_CONTAINER(running_box));
            for(iter = ch; iter != NULL; iter = g_list_next(iter))
                gtk_widget_destroy(GTK_WIDGET(iter->data));
            g_list_free(ch);
        }
    else
        {
            running_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 1);
            gtk_container_add(GTK_CONTAINER(box), running_box);
            top_box = box;
        }

    Display *d = XOpenDisplay(NULL);
    Window *list;
    unsigned long len;
    list = list_windows(d, &len);

    char *ptr = NULL;
    for (int i = 0; i < (int) len; i++)
    {
        ptr = get_window_name(d, list[i]);
        if(ptr != NULL && strlen(ptr) > 0 && is_minimized(d, list[i]))
        {
            GtkWidget *button = gtk_button_new_with_label(ptr);
            gtk_widget_set_name(button, "SiDEPanelHiddenApp");
            g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(toggle_win), NULL);
            gtk_container_add(GTK_CONTAINER(running_box), button);

        }
    }
    gtk_widget_set_size_request(running_box, 400, -1);
    gtk_widget_show_all(running_box);
    XCloseDisplay(d);
}


void create_app_menu(GtkWidget *box)
{
    GtkWidget *menu;
    app_menu_button = gtk_button_new_with_label("Applications");
    gtk_widget_set_name(app_menu_button, "SiDEPanelAppMenuButton");
    gtk_container_add(GTK_CONTAINER(box), app_menu_button);

    menu = gtk_menu_new();

    GtkWidget *multimedia = gtk_menu_new();
    GtkWidget *development = gtk_menu_new();
    GtkWidget *education = gtk_menu_new();
    GtkWidget *game = gtk_menu_new();
    GtkWidget *graphics = gtk_menu_new();
    GtkWidget *network = gtk_menu_new();
    GtkWidget *office = gtk_menu_new();
    GtkWidget *science = gtk_menu_new();
    GtkWidget *settings = gtk_menu_new();
    GtkWidget *sys = gtk_menu_new();
    GtkWidget *utility = gtk_menu_new();
    GtkWidget *other = gtk_menu_new();


    GtkWidget *multimedia_entry = gtk_menu_item_new_with_label("Multimedia");
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), multimedia_entry);
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(multimedia_entry), multimedia);

    GtkWidget *development_entry = gtk_menu_item_new_with_label("Development");
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), development_entry);
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(development_entry), development);

    GtkWidget *education_entry = gtk_menu_item_new_with_label("Education");
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), education_entry);
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(education_entry), education);

    GtkWidget *game_entry = gtk_menu_item_new_with_label("Game");
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), game_entry);
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(game_entry), game);

    GtkWidget *graphics_entry = gtk_menu_item_new_with_label("Graphics");
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), graphics_entry);
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(graphics_entry), graphics);

    GtkWidget *network_entry = gtk_menu_item_new_with_label("Network");
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), network_entry);
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(network_entry), network);

    GtkWidget *office_entry = gtk_menu_item_new_with_label("Office");
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), office_entry);
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(office_entry), office);

    GtkWidget *science_entry = gtk_menu_item_new_with_label("Science");
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), science_entry);
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(science_entry), science);

    GtkWidget *settings_entry = gtk_menu_item_new_with_label("Settings");
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), settings_entry);
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(settings_entry), settings);

    GtkWidget *sys_entry = gtk_menu_item_new_with_label("System");
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), sys_entry);
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(sys_entry), sys);

    GtkWidget *utility_entry = gtk_menu_item_new_with_label("Utility");
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), utility_entry);
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(utility_entry), utility);


    GtkWidget *other_entry = gtk_menu_item_new_with_label("Other");
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), other_entry);
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(other_entry), other);


    search_entry = gtk_menu_item_new_with_label("Search...");
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), search_entry);
    g_signal_connect(G_OBJECT(search_entry), "activate", G_CALLBACK(run_app), NULL);

    side_apps_load();

    AppEntry ent;

      do
      {
          ent = side_apps_get_next_entry();
          if(ent.valid == FALSE)
            break;

          if(ent.show == FALSE)
            continue;

          if(ent.exec_length == 0)
            continue;

          if(!check_name(ent.app_name))
            continue;
          if(!check_name(ent.exec))
            continue;

          side_log_debug("Adding new appmenu entry:");
          side_log_debug(ent.app_name);

          total_apps++;
          apps = realloc(apps, sizeof(Apps) * total_apps);
          if(!apps)
            {
              side_log_error("OUT OF MEMORY");
            }



          apps[total_apps - 1].exec = malloc(ent.exec_length + 1);

          if(apps[total_apps -1].exec == NULL)
          {
            side_log_error("Can't alloc command!");
            continue;
          }

          strncpy(apps[total_apps - 1].exec, ent.exec, ent.exec_length);
          apps[total_apps - 1].exec[ent.exec_length] = '\0';
          apps[total_apps - 1].item = gtk_menu_item_new_with_label(ent.app_name);
          apps[total_apps - 1].terminal = ent.terminal;

          if(apps[total_apps - 1].item == NULL || !GTK_IS_WIDGET(apps[total_apps - 1].item))
              {
                  side_log_warning("Can't add new item: Skipping");
                  continue;
              }

          switch(ent.sub)
          {
              case APP_TYPE_MULTIMEDIA:
                gtk_menu_shell_append(GTK_MENU_SHELL(multimedia), apps[total_apps -1].item);
                gtk_widget_show(apps[total_apps -1].item);
              break;
              case APP_TYPE_DEVELOPMENT:
                gtk_menu_shell_append(GTK_MENU_SHELL(development), apps[total_apps -1].item);
                gtk_widget_show(apps[total_apps -1].item);
              break;
              case APP_TYPE_EDUCATION:
                gtk_menu_shell_append(GTK_MENU_SHELL(education), apps[total_apps -1].item);
                gtk_widget_show(apps[total_apps -1].item);
              break;
              case APP_TYPE_GAME:
                gtk_menu_shell_append(GTK_MENU_SHELL(game), apps[total_apps -1].item);
                gtk_widget_show(apps[total_apps -1].item);
              break;
              case APP_TYPE_GRAPHICS:
                gtk_menu_shell_append(GTK_MENU_SHELL(graphics), apps[total_apps -1].item);
                gtk_widget_show(apps[total_apps -1].item);
              break;
              case APP_TYPE_NETWORK:
                gtk_menu_shell_append(GTK_MENU_SHELL(network), apps[total_apps -1].item);
                gtk_widget_show(apps[total_apps -1].item);
              break;
              case APP_TYPE_OFFICE:
                gtk_menu_shell_append(GTK_MENU_SHELL(office), apps[total_apps -1].item);
                gtk_widget_show(apps[total_apps -1].item);
              break;
              case APP_TYPE_SCIENCE:
                gtk_menu_shell_append(GTK_MENU_SHELL(science), apps[total_apps -1].item);
                gtk_widget_show(apps[total_apps -1].item);
              break;
              case APP_TYPE_SETTINGS:
                gtk_menu_shell_append(GTK_MENU_SHELL(settings), apps[total_apps -1].item);
                gtk_widget_show(apps[total_apps -1].item);
              break;
              case APP_TYPE_SYSTEM:
                gtk_menu_shell_append(GTK_MENU_SHELL(sys), apps[total_apps -1].item);
                gtk_widget_show(apps[total_apps -1].item);
              break;
              case APP_TYPE_UTILITY:
                gtk_menu_shell_append(GTK_MENU_SHELL(utility), apps[total_apps -1].item);
                gtk_widget_show(apps[total_apps -1].item);
              break;
              default:
                gtk_menu_shell_append(GTK_MENU_SHELL(other), apps[total_apps -1].item);
                gtk_widget_show(apps[total_apps -1].item);
              break;


         }



          g_signal_connect(G_OBJECT(apps[total_apps -1].item), "activate", G_CALLBACK(run_app), NULL);

          side_log_debug("Adding compleate");
      }while(ent.valid == TRUE );



    side_apps_close();
    gtk_widget_show_all(menu);

    g_signal_connect(G_OBJECT(app_menu_button), "button_press_event", G_CALLBACK(app_menu), menu);

}

gboolean app_menu(GtkWidget *w, GdkEventButton *e, GtkWidget *menu)
{
  gtk_menu_popup(GTK_MENU(menu), NULL, NULL, NULL, NULL, e->button, e->time);
}

gboolean check_name(char *text)
{
  if(text ==  NULL)
    return FALSE;

  if(strlen(text)  == 0)
    return FALSE;


  unsigned int x;
  for(x = 0; x < strlen(text); x++)
    if(!isprint(text[x]))
      return FALSE;




  return TRUE;
}

gboolean run_app(GtkWidget *w, GdkEvent *e, gpointer *p)
{

  if(w == search_entry)
    {
      system(SEARCH_APP);
      return FALSE;
    }

  for(int x = 0; x < total_apps; x++)
    if(apps[x].item == w)
      {
        //kill all errors
        unsigned int y;
        for(y = 0; y < strlen(apps[x].exec); y++)
          if(!isprint(apps[x].exec[y]))
            apps[x].exec[y] = '\0';

        char buffer[strlen(apps[x].exec)];
        memset(buffer, 0, sizeof(buffer));
        strcat(buffer, strtok(apps[x].exec, "%%\n"));
        strcat(buffer, " &\0");

        GSettings *term = g_settings_new("org.gnome.desktop.default-applications.terminal");
        char *ptr = strdup(g_variant_get_string(g_settings_get_value(term, "exec"), NULL));

        if(apps[x].terminal)
          {
            char buffer2[strlen(buffer) + strlen(ptr) + 10];
            memset(buffer2,0, sizeof(buffer2));
            strcat(buffer2, ptr);
            strcat(buffer2, " -e ");
            strcat(buffer2, buffer);
            system(buffer2);
          }
        else
          system(buffer);
        return FALSE;
      }
  return FALSE;
}
