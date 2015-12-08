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

typedef struct {

  GtkWidget *win;
  GtkWidget *header;
  GtkWidget *entry;
  GtkWidget *paned;
  GtkWidget *places;
  GtkWidget *listbox;
  GtkWidget *scroll;

  char *path;
  int sort_by;

}SiDEFilesProto;

void navigate(SiDEFilesProto *sf, short source, char *target);

#include "filemanager_list.h"

void destroy(GtkWidget *w, GdkEvent *e, gpointer data)
{
  gtk_main_quit();
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
    char *cmd = g_strdup_printf("side-open %s%s &", sf->path, file);
    system(cmd);
    free(cmd);
  }
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
    strcat(target, "/");
  }
  sf->path = target;

  reload_files(sf);
  // Set Entry to Target:
  gtk_entry_set_text(GTK_ENTRY(sf->entry), target);
}

void location_nav(GtkPlacesSidebar *sb, GObject *location, GtkPlacesOpenFlags flags, gpointer data)
{
  navigate(data, NAV_SOURCE_SIDEBAR, NULL);
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
  sf->entry = gtk_entry_new();
  gtk_box_pack_start(GTK_BOX(box), sf->entry, FALSE, FALSE, 0);
  gtk_entry_set_has_frame(GTK_ENTRY(sf->entry), TRUE);
  gtk_entry_set_placeholder_text(GTK_ENTRY(sf->entry), _("Path"));
  gtk_entry_set_icon_from_icon_name(GTK_ENTRY(sf->entry), GTK_ENTRY_ICON_PRIMARY, "folder");
  gtk_entry_set_input_purpose(GTK_ENTRY(sf->entry), GTK_INPUT_PURPOSE_URL);

  //LISTBOX
  sf->listbox = gtk_list_box_new();
  gtk_list_box_set_selection_mode(GTK_LIST_BOX(sf->listbox), GTK_SELECTION_MULTIPLE);
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

  //NAVIGATE
  navigate(sf, NAV_GO_HOME, NULL);

  gtk_widget_show_all(sf->win);
  gtk_main();

}
