/*
This file is licensed under the MIT-License
Copyright (c) 2015 Marius Messerschmidt
For more details view file 'LICENSE'
*/

#include <gtk/gtk.h>
#include <stdlib.h>
#include <glib/gi18n.h>
#include <side/widgets.h>
#include <side/plugin.h>
#include "../shared/info.h"

GtkWidget *main_menu(void);

typedef struct
{
  GtkWidget *header;
  GtkWidget *box;
}SiDESettingsExplorer;

SiDESettingsExplorer se_data;


gboolean destroy(GtkWidget *w, GdkEvent *e, gpointer *p)
{
  gtk_main_quit();
  return FALSE;
}

gboolean about_dialog(GtkWidget *w, GdkEvent *e, gpointer p)
{
  GtkWidget *dialog = gtk_about_dialog_new();
  gtk_about_dialog_set_program_name(GTK_ABOUT_DIALOG(dialog), "SiDE");
  gtk_about_dialog_set_version(GTK_ABOUT_DIALOG(dialog), VERSION);
  gtk_about_dialog_set_copyright(GTK_ABOUT_DIALOG(dialog), COPYRIGHT);
  gtk_about_dialog_set_comments(GTK_ABOUT_DIALOG(dialog), DISCRIPTION);
  gtk_about_dialog_set_license_type(GTK_ABOUT_DIALOG(dialog), GTK_LICENSE_MIT_X11);
  gtk_about_dialog_set_authors(GTK_ABOUT_DIALOG(dialog), AUTHORS);
  gtk_about_dialog_set_logo(GTK_ABOUT_DIALOG(dialog), gdk_pixbuf_scale_simple(gdk_pixbuf_new_from_file("/usr/share/icons/jetspace/JetSpace.png", NULL), 100, 100, GDK_INTERP_BILINEAR));
  gtk_about_dialog_set_artists(GTK_ABOUT_DIALOG(dialog), ARTISTS);
  gtk_about_dialog_set_website(GTK_ABOUT_DIALOG(dialog), "http://jetspace.github.io");
  gtk_dialog_run(GTK_DIALOG(dialog));
  gtk_widget_destroy(dialog);

}

gboolean go_back(GtkWidget *w, GdkEvent *e, gpointer p)
{
  GList *ch, *iter;
  ch = gtk_container_get_children(GTK_CONTAINER(se_data.box));
  for(iter = ch; iter != NULL; iter = g_list_next(iter))
  {
   gtk_widget_destroy(GTK_WIDGET(iter->data));
  }
  g_list_free(ch);

  gtk_container_add(GTK_CONTAINER(se_data.box), main_menu());
  gtk_widget_show_all(se_data.box);
}

gboolean activated_item (GtkIconView *iv, GtkTreePath *path, gpointer p)
{
  gtk_icon_view_unselect_path(iv, path);
  GtkTreeModel *tm = gtk_icon_view_get_model(iv);
  char *exec;
  GtkTreeIter iter;
  gtk_tree_model_get_iter(tm, &iter, path);
  gtk_tree_model_get(tm, &iter, 1, &exec, -1);

  if(strcmp(exec, "side-about") == 0)
    about_dialog(NULL, NULL, NULL);
  else if(strncmp(exec, "CB:",3) == 0)
  {
    GList *ch, *iter;
    ch = gtk_container_get_children(GTK_CONTAINER(se_data.box));
    for(iter = ch; iter != NULL; iter = g_list_next(iter))
    {
     gtk_widget_destroy(GTK_WIDGET(iter->data));
    }
    g_list_free(ch);

    strtok(exec, ":");
    int id = atoi(strtok(NULL, "\0"));

    exec_callback(id, se_data.box, se_data.header);
    gtk_widget_show_all(se_data.box);
  }
  else
    system(exec);


}
gboolean load_plugins = TRUE;
GtkWidget *main_menu(void)
{

  gtk_header_bar_set_subtitle(GTK_HEADER_BAR(se_data.header), _("Main Menu"));

  GtkWidget *label_a, *label_i, *label_s;

  GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);

  label_a = side_gtk_label_new(_("Appearance:"));

  label_i = side_gtk_label_new(_("Information:"));

  label_s = side_gtk_label_new(_("System:"));

  //Create Icon Views

  GtkListStore *list;
  GtkListStore *a, *s;
  GtkTreeIter iter;
  GtkIconTheme *theme = gtk_icon_theme_get_default();

  //Appearance
    GtkWidget *iconview = gtk_icon_view_new();
    GtkWidget *scroll = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_min_content_height(GTK_SCROLLED_WINDOW(scroll), 90);
    gtk_container_add(GTK_CONTAINER(scroll), iconview);

    gtk_box_pack_start(GTK_BOX(box), label_a, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(box), scroll, TRUE, TRUE, 0);

    list = gtk_list_store_new(4, G_TYPE_STRING, G_TYPE_STRING, GDK_TYPE_PIXBUF, G_TYPE_STRING,-1);
    a=list;

    gtk_list_store_append(list, &iter);
    gtk_list_store_set(list, &iter, 0, _("Panel"), 1, "side-panel-settings", 2,gtk_icon_theme_load_icon(theme, "preferences-desktop", 32,GTK_ICON_LOOKUP_FORCE_SIZE ,NULL), -1);


    gtk_icon_view_set_model(GTK_ICON_VIEW(iconview), GTK_TREE_MODEL(list));
    gtk_icon_view_set_text_column(GTK_ICON_VIEW(iconview), 0);
    gtk_icon_view_set_tooltip_column(GTK_ICON_VIEW(iconview), 3);
    gtk_icon_view_set_pixbuf_column(GTK_ICON_VIEW(iconview), 2);

    gtk_icon_view_set_activate_on_single_click(GTK_ICON_VIEW(iconview), TRUE);
    g_signal_connect(G_OBJECT(iconview), "item-activated", G_CALLBACK(activated_item), NULL);




  //System
    iconview = gtk_icon_view_new();
    scroll = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_min_content_height(GTK_SCROLLED_WINDOW(scroll), 90);
    gtk_container_add(GTK_CONTAINER(scroll), iconview);

    gtk_box_pack_start(GTK_BOX(box), label_s, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(box), scroll, TRUE, TRUE, 0);

    list = gtk_list_store_new(4, G_TYPE_STRING, G_TYPE_STRING, GDK_TYPE_PIXBUF, G_TYPE_STRING,-1);
    s=list;

    gtk_list_store_append(list, &iter);
    gtk_list_store_set(list, &iter, 0, _("Session"), 1, "side-session-settings", 2,gtk_icon_theme_load_icon(theme, "preferences-system", 32,GTK_ICON_LOOKUP_FORCE_SIZE ,NULL), -1);

    gtk_icon_view_set_model(GTK_ICON_VIEW(iconview), GTK_TREE_MODEL(list));
    gtk_icon_view_set_text_column(GTK_ICON_VIEW(iconview), 0);
    gtk_icon_view_set_tooltip_column(GTK_ICON_VIEW(iconview), 3);
    gtk_icon_view_set_pixbuf_column(GTK_ICON_VIEW(iconview), 2);

    gtk_icon_view_set_activate_on_single_click(GTK_ICON_VIEW(iconview), TRUE);
    g_signal_connect(G_OBJECT(iconview), "item-activated", G_CALLBACK(activated_item), NULL);

    //Infrormation
    iconview = gtk_icon_view_new();
    scroll = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_min_content_height(GTK_SCROLLED_WINDOW(scroll), 90);
    gtk_container_add(GTK_CONTAINER(scroll), iconview);

    gtk_box_pack_start(GTK_BOX(box), label_i, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(box), scroll, TRUE, TRUE, 0);

    list = gtk_list_store_new(4, G_TYPE_STRING, G_TYPE_STRING, GDK_TYPE_PIXBUF, G_TYPE_STRING,-1);

    gtk_list_store_append(list, &iter);
    gtk_list_store_set(list, &iter, 0, _("About SiDE"), 1, "side-about", 2,gtk_icon_theme_load_icon(theme, "dialog-information", 32,GTK_ICON_LOOKUP_FORCE_SIZE ,NULL), -1);

    gtk_icon_view_set_model(GTK_ICON_VIEW(iconview), GTK_TREE_MODEL(list));
    gtk_icon_view_set_text_column(GTK_ICON_VIEW(iconview), 0);
    gtk_icon_view_set_tooltip_column(GTK_ICON_VIEW(iconview), 3);
    gtk_icon_view_set_pixbuf_column(GTK_ICON_VIEW(iconview), 2);

    gtk_icon_view_set_activate_on_single_click(GTK_ICON_VIEW(iconview), TRUE);
    g_signal_connect(G_OBJECT(iconview), "item-activated", G_CALLBACK(activated_item), NULL);

    if(load_plugins)
    {
      load_side_settings_plugins(a,s,list, "/usr/lib/jetspace/settings/", FALSE);
      load_plugins = FALSE;
    }
    else
      load_side_settings_plugins(a,s,list, "/usr/lib/jetspace/settings/", TRUE);
    return box;
}


int main(int argc, char **argv)
{

  textdomain("side");

  gtk_init(&argc, &argv);
  side_set_application_mode(SIDE_APPLICATION_MODE_SETTINGS); // set application to be a settings app

  GtkWidget *win;

  win = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_resize(GTK_WINDOW(win), 600, 300);
  gtk_window_set_title(GTK_WINDOW(win), _("SiDE Settings"));
  gtk_container_set_border_width(GTK_CONTAINER(win), 12);
  g_signal_connect(G_OBJECT(win), "delete-event", G_CALLBACK(destroy), NULL);

  se_data.box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);


  // Header Bar (CSD)
  se_data.header = gtk_header_bar_new();
  gtk_header_bar_set_has_subtitle(GTK_HEADER_BAR(se_data.header), TRUE);
  gtk_header_bar_set_show_close_button(GTK_HEADER_BAR(se_data.header), TRUE);
  gtk_header_bar_set_title(GTK_HEADER_BAR(se_data.header), "SiDE Settings");
  gtk_window_set_titlebar (GTK_WINDOW(win), se_data.header);

  GtkWidget *allsettings = gtk_button_new_with_label(_("All Settings"));
  gtk_header_bar_pack_start(GTK_HEADER_BAR(se_data.header), allsettings);
  g_signal_connect(G_OBJECT(allsettings), "clicked", G_CALLBACK(go_back), NULL);


  gtk_container_add(GTK_CONTAINER(se_data.box), main_menu());

  gtk_container_add(GTK_CONTAINER(win), se_data.box);
  gtk_widget_show_all(win);

  gtk_main();

}
