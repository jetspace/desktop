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
#include <side/widgets.h>

#include <jetspace/logkit.h>

#include <glib/gi18n.h>

#include <errno.h>


#define PANEL_HEIGHT 35
#define SEARCH_APP "side-search &"

#include "../shared/strdup.h"
#include "../shared/context.h"
#include "../shared/run.h"
#include "../shared/plugins.h"
#include "../shared/windows.h"
#include "../shared/transparent.h"


GtkWidget *app_menu_button;
GtkWidget *search_entry;
GtkWidget *panel;


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
  char *name;
  SubType sub;
  gboolean terminal;
  GtkWidget *item;
}Apps;

enum
{
  FLAG_DELETE = 0,
  FLAG_ADD,
  FLAG_UPDATE
};

typedef struct
{
  Window win;
  GtkWidget *button;
  short flag;
}PanelWindow;

Apps        *apps;
PanelEntry  *elements;

PanelWindow *windows = NULL;
int n_windows = 0;

void add_new_element(GtkWidget *box, char *icon, char *exec, char *tooltip,  gint state);
void setup_panel(GtkWidget *box, char *app_list);
void create_app_menu(GtkWidget *box);
void running_apps(GtkWidget *box);

GtkWidget *running_box = NULL;
GtkWidget *app_box;


unsigned int get_available_space(void)
{
  gint temp;
  unsigned int total_width=0;
  gtk_window_get_size(GTK_WINDOW(panel), &temp, NULL);
  total_width = temp;
  GList *ch, *iter;
  ch = gtk_container_get_children(GTK_CONTAINER(gtk_bin_get_child(GTK_BIN(gtk_bin_get_child(GTK_BIN(panel))))));

  for(iter = ch; iter != NULL; iter = g_list_next(iter))
  {
          if(GTK_IS_WIDGET(iter->data) && GTK_WIDGET(iter->data) != running_box)
          {
            GtkAllocation all;
            gtk_widget_get_allocation(GTK_WIDGET(iter->data), &all);
            total_width -= all.width;
          }
    }
  g_list_free(ch);
  return total_width;

}



int main(int argc, char **argv)
{
  jetspace_logkit_init(argc, argv);
  jetspace_logkit_enable_glib_handler();
  textdomain("side");

  gtk_init(&argc, &argv);
  side_set_application_mode(SIDE_APPLICATION_MODE_PANEL);

  GSettings *apps = g_settings_new("org.jetspace.desktop.panel");
  GSettings *session = g_settings_new("org.jetspace.desktop.session");

  GdkDisplay *display;
  GdkScreen *screen;
  display = gdk_display_get_default ();
  screen = gdk_display_get_default_screen (display);


  //remove effects from icons
  GtkCssProvider *p = gtk_css_provider_new();
  gtk_style_context_add_provider_for_screen (screen, GTK_STYLE_PROVIDER (p), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
  gtk_css_provider_load_from_data(p, "* \n{\n-gtk-image-effect: none;\n}", -1, NULL);





  jetspace_set_log_level_from_environment();
  jetspace_debug("Loglevel loaded from environment [%d]", jetspace_get_log_level());

  //GdkScreen *screen = gdk_screen_get_default();

  //setup elements
  elements = malloc(sizeof(PanelEntry));
  apps = malloc(sizeof(Apps));

  //Setup Main Window
  GtkWidget *event, *menu;
  panel = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_decorated(GTK_WINDOW(panel), FALSE);
  gtk_window_set_title(GTK_WINDOW(panel), "");
  gtk_window_set_type_hint(GTK_WINDOW (panel), GDK_WINDOW_TYPE_HINT_DOCK);
  gtk_window_resize(GTK_WINDOW(panel), gdk_screen_get_width(screen), PANEL_HEIGHT);
  gtk_window_move(GTK_WINDOW(panel), 0, gdk_screen_get_height(screen) - PANEL_HEIGHT);
  gtk_container_set_border_width(GTK_CONTAINER(panel), 1);
  gtk_window_stick(GTK_WINDOW(panel));


  GtkWidget *box;
  box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);

  //setup context menu
  menu = gtk_menu_new();
  add_context_menu_pannel(menu, box);

  //Event box
  event = gtk_event_box_new();
  gtk_container_add(GTK_CONTAINER(panel), event);
  g_signal_connect(G_OBJECT(event), "button_press_event", G_CALLBACK(button_event), menu);
  gtk_widget_set_events(event, GDK_BUTTON_PRESS_MASK);

  //add gtk box
  gtk_container_add(GTK_CONTAINER(event), box);

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

  app_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 2);
  gtk_box_pack_start(GTK_BOX(box),app_box, FALSE, FALSE, 0);

  setup_panel(box, app_list);
  g_signal_connect(G_OBJECT(apps), "changed", G_CALLBACK(update_icons), box);

  //call plugin loader
  load_plugins("/usr/lib/jetspace/panel/plugins/", panel);
  gtk_widget_realize(panel);
  gtk_widget_show_all(panel);

  if(g_variant_get_boolean(g_settings_get_value(menuS, "show-window-list")))
  {
      running_apps(box);
      g_timeout_add(g_variant_get_int32(g_settings_get_value(menuS, "window-list-refresh-rate")), update_apps, NULL);
  }



 if(g_variant_get_boolean(g_settings_get_value(session, "use-custom-theme")))
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

  //REPLACED gkt_button_new_from_icon_name to make SiDE available on older machines
  elements[total_elements -1].button = gtk_button_new();
  GtkWidget *img = gtk_image_new_from_icon_name(icon, GTK_ICON_SIZE_SMALL_TOOLBAR);
  gtk_button_set_image(GTK_BUTTON(elements[total_elements -1].button), img);

  strncpy(elements[total_elements -1].icon, icon, 100);
  strncpy(elements[total_elements -1].exec, exec, 100);


  jetspace_debug("Adding new element to panel: %50s |  %50s", icon, exec);

  gtk_button_set_relief (GTK_BUTTON(elements[total_elements -1].button), GTK_RELIEF_NONE);
  g_signal_connect(G_OBJECT(elements[total_elements -1].button), "button_press_event", G_CALLBACK(clicked_item), NULL);
  gtk_widget_set_tooltip_text(elements[total_elements -1].button, tooltip);
  gtk_widget_set_name(elements[total_elements -1].button, "SiDEPanelAppIcon");
  gtk_box_pack_start(GTK_BOX(app_box), elements[total_elements -1].button, FALSE, FALSE, 0);

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

  jetspace_warning("[PANEL] - Unable to find button -> Memory may corrupted!");
  return FALSE;
}

void setup_panel(GtkWidget *box, char *app_list)
{
  GList *ch, *iter;
  ch = gtk_container_get_children(GTK_CONTAINER(app_box));
  for(iter = ch; iter != NULL; iter = g_list_next(iter))
  {
    if(iter->data != app_menu_button)
    {
      for(int x = 0; x < total_elements; x++)
       {
        if(iter->data == elements[x].button)
        {
          if(GTK_IS_WIDGET(iter->data))
          {
            gtk_widget_destroy(iter->data);
          }
        }
       }
    }
  }
  g_list_free(ch);
  total_elements = 0;


  //phrase app list
  char *e, *i, *a; //exec icon active
  i = strdup(strtok(app_list, ":"));
  e = strdup(strtok(NULL, ":"));
  a = strdup(strtok(NULL, ";"));
  while(i != NULL && e != NULL)
    {
      add_new_element(box, i, e, g_strdup_printf(_("Path:\t%s\nIcon:\t%s"), e, i) ,atoi(a));
      i = strtok(NULL, ":");
      e = strtok(NULL, ":");
      a = strtok(NULL, ";");
    }


}

gboolean update_icons(GSettings *s, gchar *key, GtkWidget *box)
{
  if(strcmp(key, "apps") == 0)
  {
    char *app_str = strdup(g_variant_get_string(g_settings_get_value(s, "apps"), NULL));
    setup_panel(box, app_str);
    gtk_widget_show_all(box);
    free(app_str);
  }
  else if(strcmp(key, "ignored-plugins") == 0)
  {
    char *plugins = strdup(g_variant_get_string(g_settings_get_value(s, "ignored-plugins"), NULL));
    update_plugins(plugins, panel);
    gtk_widget_show_all(box);
    free(plugins);
  }
  else if(strcmp(key, "use-custom-theme") == 0 || strcmp(key, "custom-theme-path") == 0)
  {
    side_set_application_mode(SIDE_APPLICATION_MODE_PANEL);
  }

}


GtkWidget *top_box = NULL;

gboolean update_apps(gpointer data)
{
    running_apps(top_box);
    return TRUE;
}


static gboolean toggle_win(GtkWidget *wid, GdkEvent *e, gpointer p)
{
    unsigned int search = GPOINTER_TO_INT(p);

    Display *d = XOpenDisplay(NULL);
    unsigned long len;
    /*Window *list = list_windows(d, &len);;

    if(search > len)
    {
      jetspace_error("WINDOWLIST CAN'T FIND TARGET %d (unhide)", search);
    }*/

    unhide(d, windows[search].win);

    XCloseDisplay(d);
    return FALSE;
}

unsigned int win_width = 20;
unsigned int max_lenght = 0; // The maximum width of one app entry
long last_len = 0;

void running_apps(GtkWidget *box)
{

    if(running_box == NULL)
    {
      running_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 1);
      gtk_box_pack_start(GTK_BOX(box), running_box, TRUE, TRUE, 0);
      max_lenght = win_width;
    }

    gtk_widget_show_all(running_box);

    if(windows == NULL)
    {
      windows = malloc(1);
    }

    int hidden = 0;
    int space = get_available_space();

    if(space < 0)
      return;

    int total_space = space;
    Display *d = XOpenDisplay(NULL);
    Window *list;
    long len;
    list = list_windows(d, &len);

    for(int x = 0; x < n_windows; x++)
      windows[x].flag = FLAG_DELETE; // MARK ALL TO BE DELETED

    for(long c = 0; c < len; c++)
    {
      char *t = get_window_name(d, list[c]);
      if(t == NULL)
        continue;

      free(t);
      bool match = false;
      for(int x = 0; x < n_windows; x++)
      {
        if(windows[x].win == list[c])
        {
          windows[x].flag = FLAG_UPDATE; // DO NOT DELETE
          match = true;
          break;
        }
      }
      if(match)
        continue;

      n_windows++;
      windows = realloc(windows, sizeof(PanelWindow) * n_windows);
      windows[n_windows -1].win = list[c];
      windows[n_windows -1].flag = FLAG_ADD;
      windows[n_windows -1].button = NULL;

    }

    GSettings *sett = g_settings_new("org.jetspace.desktop.panel");
    gboolean onlyHidden = g_settings_get_boolean(sett, "window-list-only-hidden");
    win_width = g_settings_get_int(sett, "window-list-title-width");
    g_object_unref(G_OBJECT(sett));



    for(int x = 0; x < n_windows; x++)
    {
      if(windows[x].flag == FLAG_DELETE)
      {
        n_windows--;
        PanelWindow *temp = malloc(sizeof(PanelWindow) * n_windows);
        gtk_widget_destroy(windows[x].button);
        int counter = 0;
        for(int y = 0; y < n_windows + 1; y++)
        {
          if(windows[y].win == windows[x].win)
            continue;

          temp[counter].win = windows[y].win;
          temp[counter].flag = windows[y].flag;
          temp[counter].button = windows[y].button;
          counter++;
        }
        free(windows);
        windows = temp;
        running_apps(box);
      }

      char *title = get_window_name(d, windows[x].win);

      if(title == NULL)
        continue;
      else if(strlen(title) < 1)
        continue;


      if(windows[x].flag == FLAG_ADD)
      {
        windows[x].button = gtk_button_new_with_label("");
        gtk_widget_set_name(windows[x].button, "SiDEPanelHiddenApp");
        gtk_widget_show_all(windows[x].button);
        windows[x].flag = FLAG_UPDATE;
        g_signal_connect(G_OBJECT(windows[x].button), "clicked", G_CALLBACK(toggle_win), GINT_TO_POINTER(x));
        gtk_container_add(GTK_CONTAINER(running_box), windows[x].button);
      }

      if(windows[x].flag == FLAG_UPDATE)
      {
        char *t = malloc(max_lenght +5);
        for(unsigned int x = 0; x < max_lenght +5; x++)
          t[x] = '\0';

        for(unsigned int x = 0; x < max_lenght && x <= strlen(title); x++)
          t[x] = title[x];

        if(strlen(title) > max_lenght)
          strcat(t, " ...\0");

        if(windows[x].button != NULL)
        {
          gtk_button_set_label(GTK_BUTTON(windows[x].button), t);
          gtk_widget_show_all(windows[x].button);
        }
        free(t);



        int width = 0;
        gtk_widget_get_preferred_width(windows[x].button, &width, NULL);
        jetspace_debug("Free Space %d", space);
        space -= width;
        if(space <= 50)
        {
          if(max_lenght > 5)
          {
            max_lenght -= 5;
            XCloseDisplay(d);
            last_len = hidden;
            running_apps(box);
            return;
          }
          else
            jetspace_warning("New Widget can not be rendered: free space == %d", space);
        }
      }

    }

    XCloseDisplay(d);
    if(((unsigned int) space >= win_width + win_width/2 && max_lenght < win_width && hidden < last_len) || (space > (total_space / 4) && max_lenght < win_width))
    {
      max_lenght += 5;
      running_apps(box);
    }
    last_len = hidden;
}

static int sort_apps(const void *p1, const void *p2)
{
  Apps *app1 = (Apps *) p1;
  Apps *app2 = (Apps *) p2;
  return strcmp(app1->name, app2->name);

}

void create_app_menu(GtkWidget *box)
{
    GtkWidget *menu;
    app_menu_button = gtk_button_new_with_label(_("Applications"));
    gtk_widget_set_name(app_menu_button, "SiDEPanelAppMenuButton");
    gtk_box_pack_start(GTK_BOX(box), app_menu_button, FALSE, FALSE, 0);

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

    GtkWidget *entry_box;
    GtkWidget *category_icon;

    GtkIconTheme *icons = gtk_icon_theme_get_default();


    GtkWidget *multimedia_entry = gtk_menu_item_new();
    entry_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 2);
    gtk_container_add(GTK_CONTAINER(multimedia_entry), entry_box);
    category_icon = gtk_image_new_from_pixbuf(gtk_icon_theme_load_icon(icons, "applications-multimedia", 16, GTK_ICON_LOOKUP_FORCE_SIZE , NULL));
    gtk_box_pack_start(GTK_BOX(entry_box), category_icon, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(entry_box), gtk_label_new(_("Multimedia")), FALSE, FALSE, 10);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), multimedia_entry);
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(multimedia_entry), multimedia);

    GtkWidget *development_entry = gtk_menu_item_new();
    entry_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 2);
    gtk_container_add(GTK_CONTAINER(development_entry), entry_box);
    category_icon = gtk_image_new_from_pixbuf(gtk_icon_theme_load_icon(icons, "applications-development", 16, GTK_ICON_LOOKUP_FORCE_SIZE , NULL));
    gtk_box_pack_start(GTK_BOX(entry_box), category_icon, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(entry_box), gtk_label_new(_("Development")), FALSE, FALSE, 10);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), development_entry);
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(development_entry), development);

    GtkWidget *education_entry = gtk_menu_item_new();
    entry_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 2);
    gtk_container_add(GTK_CONTAINER(education_entry), entry_box);
    category_icon = gtk_image_new_from_pixbuf(gtk_icon_theme_load_icon(icons, "applications-science", 16, GTK_ICON_LOOKUP_FORCE_SIZE , NULL));
    gtk_box_pack_start(GTK_BOX(entry_box), category_icon, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(entry_box), gtk_label_new(_("Education")), FALSE, FALSE, 10);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), education_entry);
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(education_entry), education);

    GtkWidget *graphics_entry = gtk_menu_item_new();
    entry_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 2);
    gtk_container_add(GTK_CONTAINER(graphics_entry), entry_box);
    category_icon = gtk_image_new_from_pixbuf(gtk_icon_theme_load_icon(icons, "applications-graphics", 16, GTK_ICON_LOOKUP_FORCE_SIZE , NULL));
    gtk_box_pack_start(GTK_BOX(entry_box), category_icon, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(entry_box), gtk_label_new(_("Graphics")), FALSE, FALSE, 10);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), graphics_entry);
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(graphics_entry), graphics);

    GtkWidget *network_entry = gtk_menu_item_new();
    entry_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 2);
    gtk_container_add(GTK_CONTAINER(network_entry), entry_box);
    category_icon = gtk_image_new_from_pixbuf(gtk_icon_theme_load_icon(icons, "applications-internet", 16, GTK_ICON_LOOKUP_FORCE_SIZE , NULL));
    gtk_box_pack_start(GTK_BOX(entry_box), category_icon, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(entry_box), gtk_label_new(_("Network")), FALSE, FALSE, 10);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), network_entry);
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(network_entry), network);

    GtkWidget *office_entry = gtk_menu_item_new();
    entry_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 2);
    gtk_container_add(GTK_CONTAINER(office_entry), entry_box);
    category_icon = gtk_image_new_from_pixbuf(gtk_icon_theme_load_icon(icons, "applications-office", 16, GTK_ICON_LOOKUP_FORCE_SIZE , NULL));
    gtk_box_pack_start(GTK_BOX(entry_box), category_icon, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(entry_box), gtk_label_new(_("Office")), FALSE, FALSE, 10);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), office_entry);
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(office_entry), office);

    GtkWidget *settings_entry = gtk_menu_item_new();
    entry_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 2);
    gtk_container_add(GTK_CONTAINER(settings_entry), entry_box);
    category_icon = gtk_image_new_from_pixbuf(gtk_icon_theme_load_icon(icons, "applications-system", 16, GTK_ICON_LOOKUP_FORCE_SIZE , NULL));
    gtk_box_pack_start(GTK_BOX(entry_box), category_icon, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(entry_box), gtk_label_new(_("Settings")), FALSE, FALSE, 10);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), settings_entry);
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(settings_entry), settings);

    GtkWidget *sys_entry = gtk_menu_item_new();
    entry_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 2);
    gtk_container_add(GTK_CONTAINER(sys_entry), entry_box);
    category_icon = gtk_image_new_from_pixbuf(gtk_icon_theme_load_icon(icons, "applications-system", 16, GTK_ICON_LOOKUP_FORCE_SIZE , NULL));
    gtk_box_pack_start(GTK_BOX(entry_box), category_icon, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(entry_box), gtk_label_new(_("System")), FALSE, FALSE, 10);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), sys_entry);
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(sys_entry), sys);

    GtkWidget *utility_entry = gtk_menu_item_new();
    entry_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 2);
    gtk_container_add(GTK_CONTAINER(utility_entry), entry_box);
    category_icon = gtk_image_new_from_pixbuf(gtk_icon_theme_load_icon(icons, "applications-utilities", 16, GTK_ICON_LOOKUP_FORCE_SIZE , NULL));
    gtk_box_pack_start(GTK_BOX(entry_box), category_icon, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(entry_box), gtk_label_new(_("Utility")), FALSE, FALSE, 10);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), utility_entry);
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(utility_entry), utility);


    GtkWidget *other_entry = gtk_menu_item_new();
    entry_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 2);
    gtk_container_add(GTK_CONTAINER(other_entry), entry_box);
    category_icon = gtk_image_new_from_pixbuf(gtk_icon_theme_load_icon(icons, "applications-other", 16, GTK_ICON_LOOKUP_FORCE_SIZE , NULL));
    gtk_box_pack_start(GTK_BOX(entry_box), category_icon, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(entry_box), gtk_label_new(_("Other")), FALSE, FALSE, 10);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), other_entry);
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(other_entry), other);


    GtkWidget *search_entry = gtk_menu_item_new();
    entry_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 2);
    gtk_container_add(GTK_CONTAINER(search_entry), entry_box);
    category_icon = gtk_image_new_from_pixbuf(gtk_icon_theme_load_icon(icons, "system-search", 16, GTK_ICON_LOOKUP_FORCE_SIZE , NULL));
    gtk_box_pack_start(GTK_BOX(entry_box), category_icon, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(entry_box), gtk_label_new(_("Search...")), FALSE, FALSE, 10);
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

          if(ent.show_in_side == FALSE)
            continue;
          if(ent.exec_length == 0)
            continue;

          if(!check_name(ent.app_name))
            continue;
          if(!check_name(ent.exec))
            continue;

          jetspace_debug("Adding new appmenu entry: %50s", ent.app_name);

          total_apps++;
          apps = realloc(apps, sizeof(Apps) * total_apps);
          if(!apps)
            {
              jetspace_error("OUT OF MEMORY");
            }



          apps[total_apps - 1].exec = malloc(ent.exec_length + 1);
          apps[total_apps - 1].name = malloc(strlen(ent.app_name) + 1);

          if(apps[total_apps -1].exec == NULL)
          {
            jetspace_error("Can't alloc command!");
            continue;
          }

          strncpy(apps[total_apps - 1].exec, ent.exec, ent.exec_length);
          strncpy(apps[total_apps - 1].name, ent.app_name, strlen(ent.app_name));
          apps[total_apps - 1].exec[ent.exec_length] = '\0';
          apps[total_apps - 1].item = gtk_menu_item_new();
          GtkWidget *appbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 2);
          gtk_container_add(GTK_CONTAINER(apps[total_apps -1].item), appbox);

          GdkPixbuf *pb = gtk_icon_theme_load_icon(icons, ent.icon, 16, GTK_ICON_LOOKUP_FORCE_SIZE , NULL);
          if(pb == NULL)
          {
            jetspace_warning("Application %s does not have a valid icon: %s : using generic", ent.app_name, ent.icon);
            pb = gtk_icon_theme_load_icon(icons, "applications-other", 16, GTK_ICON_LOOKUP_FORCE_SIZE , NULL);
          }
          GtkWidget *icon = gtk_image_new_from_pixbuf(pb);
          gtk_box_pack_start(GTK_BOX(appbox), icon, FALSE, FALSE, 0);

          GtkWidget *label = gtk_label_new(ent.app_name);
          gtk_box_pack_start(GTK_BOX(appbox), label, FALSE, FALSE, 5);
          apps[total_apps - 1].terminal = ent.terminal;

          gtk_widget_set_tooltip_text(apps[total_apps - 1].item, ent.desc);

          if(apps[total_apps - 1].item == NULL || !GTK_IS_WIDGET(apps[total_apps - 1].item))
              {
                  jetspace_warning("Can't add new item: Skipping");
                  continue;
              }

          apps[total_apps -1].sub = ent.sub;

          //Sort Array 'apps'

          qsort(apps, total_apps, sizeof(Apps), sort_apps);


          }while(ent.valid == TRUE );
          for(int x = 0; x < total_apps; x++)
          { //todo
          switch(apps[x].sub)
          {
              case APP_TYPE_MULTIMEDIA:
                gtk_menu_shell_append(GTK_MENU_SHELL(multimedia), apps[x].item);
                gtk_widget_show(apps[x].item);
              break;
              case APP_TYPE_DEVELOPMENT:
                gtk_menu_shell_append(GTK_MENU_SHELL(development), apps[x].item);
                gtk_widget_show(apps[x].item);
              break;
              case APP_TYPE_EDUCATION:
                gtk_menu_shell_append(GTK_MENU_SHELL(education), apps[x].item);
                gtk_widget_show(apps[x].item);
              break;
              case APP_TYPE_GAME:
                gtk_menu_shell_append(GTK_MENU_SHELL(game), apps[x].item);
                gtk_widget_show(apps[x].item);
              break;
              case APP_TYPE_GRAPHICS:
                gtk_menu_shell_append(GTK_MENU_SHELL(graphics), apps[x].item);
                gtk_widget_show(apps[x].item);
              break;
              case APP_TYPE_NETWORK:
                gtk_menu_shell_append(GTK_MENU_SHELL(network), apps[x].item);
                gtk_widget_show(apps[x].item);
              break;
              case APP_TYPE_OFFICE:
                gtk_menu_shell_append(GTK_MENU_SHELL(office), apps[x].item);
                gtk_widget_show(apps[x].item);
              break;
              case APP_TYPE_SCIENCE:
                gtk_menu_shell_append(GTK_MENU_SHELL(science), apps[x].item);
                gtk_widget_show(apps[x].item);
              break;
              case APP_TYPE_SETTINGS:
                gtk_menu_shell_append(GTK_MENU_SHELL(settings), apps[x].item);
                gtk_widget_show(apps[x].item);
              break;
              case APP_TYPE_SYSTEM:
                gtk_menu_shell_append(GTK_MENU_SHELL(sys), apps[x].item);
                gtk_widget_show(apps[x].item);
              break;
              case APP_TYPE_UTILITY:
                gtk_menu_shell_append(GTK_MENU_SHELL(utility), apps[x].item);
                gtk_widget_show(apps[x].item);
              break;
              default:
                gtk_menu_shell_append(GTK_MENU_SHELL(other), apps[x].item);
                gtk_widget_show(apps[x].item);
              break;


         }



          g_signal_connect(G_OBJECT(apps[x].item), "activate", G_CALLBACK(run_app), NULL);

          jetspace_debug("Adding compleate");
      }



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
