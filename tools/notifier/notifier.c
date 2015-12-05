#include <gtk/gtk.h>
#include <glib.h>
#include <string.h>
#include <stdlib.h>
#include <libnotify/notify.h>
#include <glib/gi18n.h>
#include <locale.h>

GMainLoop *ml;

void closed (NotifyNotification *n, gpointer p)
{
  g_main_loop_quit(ml);
}

void action_cb(NotifyNotification *n, char *action, gpointer user_data)
{
  if(strcmp(action, "open") == 0)
  {
    char cmd[2000];
    snprintf(cmd, 2000, "side-files %s &", (char *) user_data);
    system(cmd);
  }
}

void new_storage_device(char *dev, char *mp)
{
  char txt[2000];
  snprintf(txt, 2000, _("Added %s to %s"), dev, mp);
  NotifyNotification *n = notify_notification_new(_("New Mass-Storage Device"), txt, "drive-removable-media");
  notify_notification_set_category(n, "device.added");
  notify_notification_set_urgency(n, NOTIFY_URGENCY_NORMAL);

  // actions
  notify_notification_add_action(n, "open", _("Open"), action_cb, mp, NULL);

  notify_notification_show(n, NULL);

  g_signal_connect(G_OBJECT(n), "closed", G_CALLBACK(closed), NULL);

}

void theme_not_found(void)
{
  NotifyNotification *n = notify_notification_new(_("Error"),_("SiDE could not find session theme"), "dialog-error");
  notify_notification_set_category(n, "side.error");
  notify_notification_set_urgency(n, NOTIFY_URGENCY_CRITICAL);

  notify_notification_show(n, NULL);

  g_signal_connect(G_OBJECT(n), "closed", G_CALLBACK(closed), NULL);
}

int main(int argc, char **argv)
{
  setlocale(LC_ALL, "");
  textdomain("side");
  notify_init("side-notifier");

  if(argc == 4)
  {
    if(strcmp(argv[1], "--new-storage-device") == 0)
      new_storage_device(argv[2], argv[3]);
  }
  else if(argc == 2)
  {
    if(strcmp(argv[1], "--theme-not-found") == 0)
      theme_not_found();
  }
  else
  {
    puts("Please use one of the following modes:");
    puts("--new-storage-device");
    puts("--theme-not-found");
    return(1);
  }

  ml = g_main_loop_new(NULL, FALSE);
  g_main_loop_run (ml);
  notify_uninit();
  return 0;
}
