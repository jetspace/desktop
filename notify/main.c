/*
This file is licensed under the MIT-License
Copyright (c) 2015 Marius Messerschmidt
For more details view file 'LICENSE'
*/

#include <glib.h>
#include <gtk/gtk.h>
#include <string.h>
#include <stdio.h>
#include <dbus/dbus.h>
#include <stdlib.h>
#include "popup.h"
#include "../shared/info.h"

DBusError err;
DBusConnection* con;

gboolean cmd_line_output = FALSE;

int SiDE_NOTIFICATION_ID_COUNTER = 0;

static DBusMessage* create_notification(DBusMessage *msg)
{
  DBusMessage *r;
	DBusMessageIter arg;

  char *sum = NULL;
  char *body = NULL;
  char *app = NULL;
  char *icon = NULL;

  if(dbus_message_iter_init(msg, &arg));
    {

      dbus_message_iter_get_basic(&arg, &app);
  		dbus_message_iter_next(&arg);
      dbus_message_iter_next(&arg);

      dbus_message_iter_get_basic(&arg, &icon);
      dbus_message_iter_next(&arg);

      dbus_message_iter_get_basic(&arg, &sum);
      dbus_message_iter_next(&arg);
      dbus_message_iter_get_basic(&arg, &body);
    }

  if(cmd_line_output)
    {
      g_print("------------------------------------------------------>\n");
      g_print("SiDE-notifyd - New Notification\n");
      g_print("SiDE-ID: %d\n", SiDE_NOTIFICATION_ID_COUNTER);
      g_print("App    : %s\n", app);
      g_print("Icon   : %s\n", icon);
      g_print("Summary: %s\n", sum);
      g_print("Body   : %s\n", body);
      g_print("------------------------------------------------------>\n");
    }


  r = dbus_message_new_method_return(msg);
	dbus_message_iter_init_append(r, &arg);
	if (!dbus_message_iter_append_basic(&arg, DBUS_TYPE_UINT32, &SiDE_NOTIFICATION_ID_COUNTER))
		return NULL;

  if(SiDE_NOTIFICATION_ID_COUNTER < INT_MAX)
    SiDE_NOTIFICATION_ID_COUNTER++;
  else
    SiDE_NOTIFICATION_ID_COUNTER = 0;

  side_notify_popup_create(SiDE_NOTIFICATION_ID_COUNTER, app, icon, sum, body, cmd_line_output);

	return r;

}

static DBusHandlerResult bus(DBusConnection *con, DBusMessage *msg, void *data)
{
	DBusMessage *r;
	const char *member = dbus_message_get_member(msg);

  if (strcmp(member, "GetServerInformation") == 0)
  {//return server infos
      DBusMessageIter arg;
			r = dbus_message_new_method_return(msg);
      dbus_message_iter_init_append(r, &arg);
      char *n = "side-notifyd";
      char *v = "JetSpace";
      char *V = "1";
      char *p = "1.2";
      dbus_message_iter_append_basic(&arg, DBUS_TYPE_STRING, &n);
      dbus_message_iter_append_basic(&arg, DBUS_TYPE_STRING, &v);
      dbus_message_iter_append_basic(&arg, DBUS_TYPE_STRING, &V);
      dbus_message_iter_append_basic(&arg, DBUS_TYPE_STRING, &p);
		dbus_connection_send(con, r, NULL);
	}
  else if (strcmp(member, "Notify") == 0)
  {
      r = create_notification(msg);
      dbus_connection_send(con, r, NULL);
  }
  else if (strcmp(member, "CloseNotification") == 0)
  {//Remove popup

    DBusMessageIter arg;
    int id;
  	dbus_message_iter_init(msg, &arg);
    dbus_message_iter_get_basic(&arg, &id);

    if(cmd_line_output)
      {
        g_print("------------------------------------------------------>\n");
        g_print("Requesting delete of message: %d", id);
        g_print("------------------------------------------------------>\n");
      }

    r = dbus_message_new_method_return(msg);

  }
  else if (strcmp(member, "GetCapabilities") == 0)
  {
    DBusMessageIter arg;
    DBusMessageIter a;

  	r = dbus_message_new_method_return(msg);
  	dbus_message_iter_init_append(r, &arg);
  	dbus_message_iter_open_container(&arg, DBUS_TYPE_ARRAY, "s", &a);

    char *first = "body";
    dbus_message_iter_append_basic(&a, DBUS_TYPE_STRING, &first);
    char *second = "body-markup";
    dbus_message_iter_append_basic(&a, DBUS_TYPE_STRING, &second);

    dbus_message_iter_close_container(&arg, &a);

    dbus_connection_send(con, r, NULL);
    }

	return DBUS_HANDLER_RESULT_HANDLED;
}

gboolean refresh_bus(gpointer data)
{
  dbus_connection_read_write_dispatch(con, 1);
  return TRUE;
}

int main(int argc, char **argv)
{

  if(argc > 1)
    if(strncmp(argv[1], "--debug", 7) == 0)
      cmd_line_output = TRUE;

  DBusObjectPathVTable *bus_v = malloc(sizeof(DBusObjectPathVTable));
	bus_v->message_function = &bus;

  dbus_error_init(&err);
  con = dbus_bus_get(DBUS_BUS_SESSION, &err);
	if (dbus_error_is_set(&err))
  {
		g_warning("SiDE-notifyd: failed to connect to D-bus: %s\n", err.message);
		dbus_error_free(&err);
	}
	if (con == NULL)
    {
      g_error("SiDE-notifyd: failed to connect to D-bus!");
    }

  int x = dbus_bus_request_name(con, "org.freedesktop.Notifications", DBUS_NAME_FLAG_REPLACE_EXISTING, &err);
  if(x != DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER)
  {
    g_warning("Is there another notification deamon running?");
    return -1;
  }

  dbus_connection_register_object_path(con, "/org/freedesktop/Notifications", bus_v, NULL);

  gtk_init(&argc, &argv);

  g_timeout_add(250, refresh_bus, NULL);

  gtk_main();
}
