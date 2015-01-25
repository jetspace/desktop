/*
Will contain all settings and their dialogs!
*/

#include <gtk/gtk.h>
GtkWidget *win, *box, *apply, *entry_path, *label_term, *label_sum;

gboolean write_panel_settings(GtkWidget *w, GdkEvent *e, gpointer *p)
{
  GSettings *term_app = g_settings_new("org.gnome.desktop.default-applications.terminal");
  g_settings_set_value(term_app, "exec", g_variant_new_string(gtk_entry_get_text(GTK_ENTRY(entry_path))));


  gtk_widget_destroy(win);
  return FALSE;
}

gboolean panel_settings(GtkWidget *w, GdkEvent *e, gpointer *p)
{
  GSettings *term_app = g_settings_new("org.gnome.desktop.default-applications.terminal");
  char *ptr = strdup(g_variant_get_string(g_settings_get_value(term_app, "exec"), NULL));



  label_sum = gtk_label_new("On this page you can set up the Panel:\n\n");
  label_term= gtk_label_new("Select the default Terminal Application:");

  entry_path = gtk_entry_new();
  gtk_entry_set_text(GTK_ENTRY(entry_path), ptr);

  win = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_resize(GTK_WINDOW(win), 300, 500);
  gtk_window_set_title(GTK_WINDOW(win), "Settings - Panel");

  box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);

  apply = gtk_button_new_from_icon_name("document-save", GTK_ICON_SIZE_BUTTON);

  gtk_container_add(GTK_CONTAINER(box), label_sum);
  gtk_container_add(GTK_CONTAINER(box), label_term);
  gtk_container_add(GTK_CONTAINER(box), entry_path);
  gtk_container_add(GTK_CONTAINER(box), apply);

  g_signal_connect(G_OBJECT(apply), "button_press_event", G_CALLBACK(write_panel_settings), NULL);


  gtk_container_add(GTK_CONTAINER(win), box);



  gtk_widget_show_all(win);
  return FALSE;

}
