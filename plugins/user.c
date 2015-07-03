/*
This file is licensed under the MIT-License
Copyright (c) 2015 Marius Messerschmidt
For more details view file 'LICENSE'
*/

#include <gtk/gtk.h>
#include <glib.h>
#include <time.h>
#include <gmodule.h>
#include <side/plugin.h>

#include <string.h>

 #include <unistd.h>
 #include <sys/types.h>
 #include <pwd.h>

 gboolean user_plugin_can_be_activated = FALSE;
 gboolean user_plugin_enabled = FALSE;


GtkWidget *box, *user_button, *user_menu, *p_buff;

void show_user(void);

gboolean user_logout(GtkWidget *w, GdkEventButton *e, gpointer p)
{
  GtkWidget *d = gtk_message_dialog_new(GTK_WINDOW(p_buff),
                        GTK_DIALOG_MODAL,
                        GTK_MESSAGE_WARNING,
                        GTK_BUTTONS_YES_NO,
                        "Do you really want to logout?");

  gint r = gtk_dialog_run(GTK_DIALOG(d));

  if(r == GTK_RESPONSE_YES)
    system("side-session --logout");

  gtk_widget_destroy(d);
  return FALSE;
}

gboolean show_user_context(GtkWidget *w, GdkEventButton *e, gpointer p)
{
  gtk_menu_popup(GTK_MENU(user_menu), NULL, NULL, NULL, NULL, e->button, e->time);
  return FALSE;
}

gboolean redraw_user(GtkWidget *widget, GdkEvent  *event, gpointer user_data)
{
  if(!user_plugin_enabled)
    return FALSE;

  g_debug("captured destuction of the User item -> re-create");
  show_user(); //draw it again!!
  return FALSE;
}


void show_user(void)
{
  gint UID = getuid();
  struct passwd *p = getpwuid(UID);

  user_button = gtk_button_new_with_label(strtok(p->pw_gecos, ",\n\t\0"));
  gtk_button_set_relief(GTK_BUTTON(user_button), GTK_RELIEF_NONE);
  g_signal_connect(G_OBJECT(user_button), "button-press-event", G_CALLBACK(show_user_context), NULL);
  g_signal_connect(G_OBJECT(user_button), "destroy", G_CALLBACK(redraw_user), NULL);

  user_menu = gtk_menu_new();

  GtkWidget *logout = gtk_menu_item_new_with_label("Logout");
  gtk_menu_shell_append(GTK_MENU_SHELL(user_menu), logout);
  g_signal_connect(G_OBJECT(logout), "button-press-event", G_CALLBACK(user_logout), NULL);

  gtk_widget_show_all(user_menu);
  gtk_box_pack_end(GTK_BOX(box), user_button, FALSE, FALSE, 5);
}

//MODLOADER
G_MODULE_EXPORT void plugin_call(GtkWidget *root)
{
  if(!check_version(COMPATIBLE_SINCE, "0.64"))
  {
    g_warning("User plugin is not compatible!");
    return;
  }

  user_plugin_can_be_activated = TRUE;

  g_print("------------------------------\n-> SIDE User Plugin loading...\n------------------------------\n"); //notify the user...
  p_buff = root;
  box = side_plugin_get_root_box(root);

}

G_MODULE_EXPORT void enable_plugin(GtkWidget *root)
{
  if(!user_plugin_can_be_activated || user_plugin_enabled)
    return;

  show_user();
  user_plugin_enabled = TRUE;
}

G_MODULE_EXPORT void disable_plugin(GtkWidget *root)
{
  if(!user_plugin_can_be_activated || !user_plugin_enabled)
    return;

  user_plugin_enabled = FALSE;
  gtk_widget_destroy(user_button);
}
