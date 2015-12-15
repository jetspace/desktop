#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <glib/gi18n.h>



enum {
  NAV_SOURCE_SIDEBAR = 0,
  NAV_SOURCE_ENTRY,
  NAV_SOURCE_CLICKED,
  NAV_SOURCE_ARG,
  NAV_SOURCE_UNKNOWN,
  //SPECIAL ONES:
  NAV_GO_HOME
}NavSources;

enum {
  SORT_BY_NAME = 0,
  SORT_BY_DATE
};

enum {
  ICON_OK = 0,
  ICON_INVALID
};

typedef struct {

  GSettings *settings;

  GtkWidget *win;
  GtkWidget *header;
  GtkWidget *entry;
  GtkWidget *home;
  GtkWidget *up;
  GtkWidget *paned;
  GtkWidget *places;
  GtkWidget *listbox;
  GtkWidget *scroll;
  GtkWidget *placeholder;
  GtkWidget *settingsbutton;

  // Sort by toggle
  GtkWidget *sort_by_name;
  GtkWidget *sort_by_last_change;
  GtkWidget *sort_folders_first;

  //SEARCH
  GtkWidget *show_search;
  GtkWidget *search;
  GtkWidget *search_button;

  char *path;
  gboolean folders_first;
  int sort_by;

}SiDEFilesProto;

void navigate(SiDEFilesProto *sf, short source, char *target);
void toggle_entry_icon(SiDEFilesProto *sf, short type);

#include "filemanager_list.h"

void destroy(GtkWidget *w, GdkEvent *e, gpointer data)
{
  gtk_main_quit();
}

void toggle_entry_icon(SiDEFilesProto *sf, short type)
{
  switch(type)
  {
    case ICON_OK:
    gtk_entry_set_icon_from_icon_name(GTK_ENTRY(sf->entry), GTK_ENTRY_ICON_PRIMARY, "folder");
    break;

    case ICON_INVALID:
    gtk_entry_set_icon_from_icon_name(GTK_ENTRY(sf->entry), GTK_ENTRY_ICON_PRIMARY, "dialog-error");
    break;
  }
}

void activated(GtkListBox *b, GtkListBoxRow *row, gpointer data)
{
  SiDEFilesProto *sf =data;
  GtkWidget *box = gtk_bin_get_child(GTK_BIN(row));
  GList *ch, *iter;
  ch = gtk_container_get_children(GTK_CONTAINER(box));
  const char *file;
  for(iter = ch; iter != NULL; iter = g_list_next(iter))
  {
    if(strcmp(gtk_widget_get_name(iter->data), "name") == 0)
      file = gtk_label_get_text(GTK_LABEL(iter->data));
  }

  if(strcmp(gtk_widget_get_name(box), "dir") == 0)
  {
    char *f = g_strdup(file);
    navigate(sf, NAV_SOURCE_CLICKED, f);
    free(f);
  }
  else
  {
    char *cmd = g_strdup_printf("side-open \"%s%s\" &", sf->path, file);
    system(cmd);
    free(cmd);
  }
}

void navbar (GtkEntry *e, gpointer data)
{
  navigate(data, NAV_SOURCE_ENTRY, NULL);
}

void navigate(SiDEFilesProto *sf, short source, char *target)
{
  if(source == NAV_GO_HOME)
  {
    struct passwd *pw = getpwuid(getuid());
    target = pw->pw_dir;
    strcat(target, "/");
  }
  else if(source == NAV_SOURCE_CLICKED)
  {
    target = g_strdup_printf("%s%s/", sf->path, target);
  }
  else if(source == NAV_SOURCE_SIDEBAR)
  {
    target = g_file_get_parse_name(gtk_places_sidebar_get_location(GTK_PLACES_SIDEBAR(sf->places)));
    if(target[strlen(target) -1] != '/')
      strcat(target, "/");
  }
  else if(source == NAV_SOURCE_ARG)
  {
    if(target[strlen(target) -1] != '/')
      strcat(target, "/");
  }
  else if(source == NAV_SOURCE_ENTRY)
  {
    target = g_strdup(gtk_entry_get_text(GTK_ENTRY(sf->entry)));
    if(target[strlen(target) -1] != '/')
      strcat(target, "/");
  }
  sf->path = target;

  gtk_entry_set_text(GTK_ENTRY(sf->search), ""); // new page == clear search

  reload_files(sf);
  // Set Entry to Target:
  if(source != NAV_SOURCE_ENTRY)
    gtk_entry_set_text(GTK_ENTRY(sf->entry), target);
}

void sort_by_changed(GtkWidget *tb, gpointer data)
{
  SiDEFilesProto *sf = data;
  if(tb == sf->sort_by_name)
    sf->sort_by = SORT_BY_NAME;
  else if(tb == sf->sort_by_last_change)
    sf->sort_by = SORT_BY_DATE;

  sf->folders_first = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(sf->sort_folders_first));
  g_settings_set_boolean(sf->settings, "folders-first", sf->folders_first);

  g_settings_set_int(sf->settings, "sort-by", sf->sort_by);
  gtk_list_box_invalidate_sort(GTK_LIST_BOX(sf->listbox));
}

void location_nav(GtkPlacesSidebar *sb, GObject *location, GtkPlacesOpenFlags flags, gpointer data)
{
  navigate(data, NAV_SOURCE_SIDEBAR, NULL);
}

void go_up(GtkWidget *w, GdkEvent *e, gpointer data)
{
  SiDEFilesProto *sf = data;
  char *text = g_strdup(gtk_entry_get_text(GTK_ENTRY(sf->entry)));

  if(strcmp(text, "/") == 0 || strlen(text) == 1)
    return;

  int slash = 0;

  /* Count '/' */
  for(int x = 0; x < strlen(text); x++)
    if(text[x] == '/')
      slash++;

  int kill_at = 0;

  for(int x = 0; x < strlen(text); x++)
  {
    if(text[x] == '/')
      slash--;

    if(slash == 1)
    {
      kill_at = x;
      break;
    }
  }

  if(kill_at == 0)
  {
    navigate(sf, NAV_SOURCE_ARG, "/");
  }
  else
  {
    char *target = malloc(kill_at * sizeof(char));
    strncpy(target, text, kill_at +1);
    target[kill_at +1] = '\0';
    gtk_entry_set_text(GTK_ENTRY(sf->entry), target);
    navigate(sf, NAV_SOURCE_ENTRY, NULL);
    g_free(target);
  }



  g_free(text);
}

void go_home(GtkWidget *w, GdkEvent *e, gpointer data)
{
    SiDEFilesProto *sf =data;
    navigate(sf, NAV_GO_HOME, NULL);
}

void toggle_search(GtkWidget *w, GdkEvent *e, gpointer data)
{
  SiDEFilesProto *sf = data;
  gtk_revealer_set_reveal_child(GTK_REVEALER(sf->show_search), !gtk_revealer_get_reveal_child(GTK_REVEALER(sf->show_search)));
  gtk_entry_set_text(GTK_ENTRY(sf->search), "");
  reload_files(sf);
}

void search_update(GtkSearchEntry *se, gpointer data)
{
  SiDEFilesProto *sf = data;
  reload_files(sf);
}

void key_pressed(GtkWidget *w, GdkEvent *e, gpointer data)
{
  SiDEFilesProto *sf = data;
  if(gtk_widget_has_focus(sf->entry))
  {
    gtk_widget_event(sf->entry,e);
    return;
  }
  if(isprint(e->key.keyval))
    gtk_revealer_set_reveal_child(GTK_REVEALER(sf->show_search), TRUE);

  gtk_search_entry_handle_event(GTK_SEARCH_ENTRY(sf->search), e);
}

/*
 * MAIN
 */


int main(int argc, char **argv)
{
  textdomain("side");
  gtk_init(&argc, &argv);

  SiDEFilesProto *sf = malloc(sizeof(SiDEFilesProto));

  sf->settings = g_settings_new("org.jetspace.desktop.filemanager");
  sf->sort_by = g_settings_get_int(sf->settings, "sort-by");
  sf->folders_first = g_settings_get_boolean(sf->settings, "folders-first");


  //header
  sf->header = gtk_header_bar_new();
  gtk_header_bar_set_has_subtitle(GTK_HEADER_BAR(sf->header), TRUE);
  gtk_header_bar_set_show_close_button(GTK_HEADER_BAR(sf->header), TRUE);
  gtk_header_bar_set_title(GTK_HEADER_BAR(sf->header), _("SiDE File Manager"));

  //WINDOW
  sf->win = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_resize(GTK_WINDOW(sf->win), 700, 500);
  gtk_window_set_title(GTK_WINDOW(sf->win), _("SiDE File Manager"));
  gtk_window_set_titlebar(GTK_WINDOW(sf->win), sf->header);


  //Paned
  sf->paned = gtk_paned_new(GTK_ORIENTATION_HORIZONTAL);
  gtk_container_add(GTK_CONTAINER(sf->win), sf->paned);

  //Places Bar
  sf->places = gtk_places_sidebar_new();
  gtk_paned_add1(GTK_PANED(sf->paned), sf->places);
  gtk_places_sidebar_set_local_only(GTK_PLACES_SIDEBAR(sf->places), TRUE);

  //BOX
  GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
  gtk_container_set_border_width(GTK_CONTAINER(box), 20);
  gtk_paned_add2(GTK_PANED(sf->paned), box);

  //Entry

  GtkWidget *entry_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 4);
  sf->up = gtk_button_new_from_icon_name("go-up", GTK_ICON_SIZE_BUTTON);

  sf->home = gtk_button_new_from_icon_name("go-home", GTK_ICON_SIZE_BUTTON);

  sf->entry = gtk_entry_new();
  gtk_entry_set_has_frame(GTK_ENTRY(sf->entry), TRUE);
  gtk_entry_set_placeholder_text(GTK_ENTRY(sf->entry), _("Path"));
  gtk_entry_set_icon_from_icon_name(GTK_ENTRY(sf->entry), GTK_ENTRY_ICON_PRIMARY, "folder");
  gtk_entry_set_input_purpose(GTK_ENTRY(sf->entry), GTK_INPUT_PURPOSE_URL);

  gtk_box_pack_end(GTK_BOX(entry_box), sf->entry, TRUE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(entry_box), sf->home, FALSE, FALSE, 0);
  gtk_box_pack_start(GTK_BOX(entry_box), sf->up, FALSE, FALSE, 0);

  gtk_box_pack_start(GTK_BOX(box), entry_box, FALSE, FALSE, 0);

  //LISTBOX
  sf->listbox = gtk_list_box_new();
  gtk_list_box_set_selection_mode(GTK_LIST_BOX(sf->listbox), GTK_SELECTION_SINGLE);
  gtk_list_box_set_activate_on_single_click(GTK_LIST_BOX(sf->listbox), TRUE);
  sf->scroll = gtk_scrolled_window_new(NULL, NULL);
  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(sf->scroll), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
  gtk_scrolled_window_set_min_content_height(GTK_SCROLLED_WINDOW(sf->scroll), 200);
  gtk_container_add(GTK_CONTAINER(sf->scroll), sf->listbox);
  gtk_box_pack_start(GTK_BOX(box), sf->scroll, TRUE, TRUE, 0);
  gtk_list_box_set_sort_func(GTK_LIST_BOX(sf->listbox), filelist_sort, sf, NULL);

  // PLACEHOLDER

  sf->placeholder = gtk_label_new("");
  gtk_label_set_markup(GTK_LABEL(sf->placeholder), _("<span size=\"xx-large\">No Files</span>"));
  gtk_widget_show_all(sf->placeholder);
  gtk_list_box_set_placeholder(GTK_LIST_BOX(sf->listbox), sf->placeholder);


  // SEARCH

  sf->search = gtk_search_entry_new();
  sf->show_search = gtk_revealer_new();
  gtk_revealer_set_transition_duration(GTK_REVEALER(sf->show_search), 200);
  gtk_revealer_set_transition_type(GTK_REVEALER(sf->show_search),GTK_REVEALER_TRANSITION_TYPE_SLIDE_RIGHT);


  gtk_container_add(GTK_CONTAINER(sf->show_search), sf->search);

  sf->search_button = gtk_button_new_from_icon_name("edit-find", GTK_ICON_SIZE_BUTTON);
  gtk_header_bar_pack_start(GTK_HEADER_BAR(sf->header), sf->search_button);
  gtk_header_bar_pack_start(GTK_HEADER_BAR(sf->header), sf->show_search);

  // SETTINGS BUTTON
  sf->settingsbutton = gtk_menu_button_new();
  gtk_header_bar_pack_end(GTK_HEADER_BAR(sf->header), sf->settingsbutton);
  GtkWidget *popover = gtk_popover_new(sf->settingsbutton);
  gtk_menu_button_set_popover(GTK_MENU_BUTTON(sf->settingsbutton), popover);


  GtkWidget *popoverbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 2);
  gtk_container_set_border_width(GTK_CONTAINER(popoverbox), 10);
  gtk_container_add(GTK_CONTAINER(popover), popoverbox);

  GtkWidget *sort_by_label = gtk_label_new("");
  gtk_label_set_markup(GTK_LABEL(sort_by_label), _("<b>Sort By</b>"));
  gtk_box_pack_start(GTK_BOX(popoverbox), sort_by_label, FALSE, FALSE, 0);

  sf->sort_by_name = gtk_radio_button_new_with_label(NULL, _("Name"));
  gtk_box_pack_start(GTK_BOX(popoverbox), sf->sort_by_name, FALSE, FALSE, 0);

  sf->sort_by_last_change = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(sf->sort_by_name), _("Last Change"));
  gtk_box_pack_start(GTK_BOX(popoverbox), sf->sort_by_last_change, FALSE, FALSE, 0);

  sf->sort_folders_first = gtk_check_button_new_with_label(_("Folders first"));
  gtk_box_pack_start(GTK_BOX(popoverbox), sf->sort_folders_first, FALSE, FALSE,  0);
  if(sf->folders_first)
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(sf->sort_folders_first), TRUE);

  switch(sf->sort_by)
  {
    case SORT_BY_NAME:
      gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(sf->sort_by_name), TRUE);
    break;
    case SORT_BY_DATE:
      gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(sf->sort_by_last_change), TRUE);
    break;
  }


  gtk_widget_show_all(popoverbox);


  //SIGNALS
  g_signal_connect(sf->win, "destroy", G_CALLBACK(destroy), sf);
  g_signal_connect(sf->win, "key-press-event", G_CALLBACK(key_pressed), sf);
  g_signal_connect(sf->listbox, "row-activated", G_CALLBACK(activated), sf);
  g_signal_connect(sf->places, "open-location", G_CALLBACK(location_nav), sf);
  g_signal_connect(sf->entry, "activate", G_CALLBACK(navbar), sf);
  g_signal_connect(sf->up, "clicked", G_CALLBACK(go_up), sf);
  g_signal_connect(sf->home, "clicked", G_CALLBACK(go_home), sf);
  g_signal_connect(sf->search_button, "clicked", G_CALLBACK(toggle_search), sf);
  g_signal_connect(sf->search, "search-changed", G_CALLBACK(search_update), sf);
  g_signal_connect(sf->sort_by_name, "toggled", G_CALLBACK(sort_by_changed), sf);
  g_signal_connect(sf->sort_by_last_change, "toggled", G_CALLBACK(sort_by_changed), sf);
  g_signal_connect(sf->sort_folders_first, "toggled", G_CALLBACK(sort_by_changed), sf);

  //NAVIGATE
  if(argc < 2)
    navigate(sf, NAV_GO_HOME, NULL);
  else
    navigate(sf, NAV_SOURCE_ARG, argv[1]);

  gtk_widget_show_all(sf->win);
  gtk_main();

}
