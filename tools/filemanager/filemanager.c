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

  GtkWidget *win;
  GtkWidget *header;
  GtkWidget *entry;
  GtkWidget *home;
  GtkWidget *up;
  GtkWidget *paned;
  GtkWidget *places;
  GtkWidget *listbox;
  GtkWidget *scroll;

  char *path;
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

  reload_files(sf);
  // Set Entry to Target:
  if(source != NAV_SOURCE_ENTRY)
    gtk_entry_set_text(GTK_ENTRY(sf->entry), target);
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


int main(int argc, char **argv)
{
  textdomain("side");
  gtk_init(&argc, &argv);

  SiDEFilesProto *sf = malloc(sizeof(SiDEFilesProto));
  sf->sort_by = SORT_BY_NAME;


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



  //SIGNALS
  g_signal_connect(sf->win, "destroy", G_CALLBACK(destroy), sf);
  g_signal_connect(sf->listbox, "row-activated", G_CALLBACK(activated), sf);
  g_signal_connect(sf->places, "open-location", G_CALLBACK(location_nav), sf);
  g_signal_connect(sf->entry, "activate", G_CALLBACK(navbar), sf);
  g_signal_connect(sf->up, "clicked", G_CALLBACK(go_up), sf);
  g_signal_connect(sf->home, "clicked", G_CALLBACK(go_home), sf);

  //NAVIGATE
  if(argc < 2)
    navigate(sf, NAV_GO_HOME, NULL);
  else
    navigate(sf, NAV_SOURCE_ARG, argv[1]);

  gtk_widget_show_all(sf->win);
  gtk_main();

}
