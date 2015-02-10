/*
This file is licensed under the MIT-License
Copyright (c) 2015 Marius Messerschmidt
For more details view file 'LICENSE'
*/

//Creates the X-Window System struts to protect windows

//CONF

enum
{
	STRUT_LEFT = 0,
	STRUT_RIGHT,
	STRUT_TOP,
	STRUT_BOTTOM
};

#include <gtk/gtk.h>
#include "../shared/config.h"



#ifdef WAYLAND
void set_struts(GtkWidget *panel, gint strut_pos, gulong val)
{
	g_warning("PLEASE NOTE: The Wayland version is HIGHLY experimental!");
	return;
}
#endif

#ifdef MIR
#error "Please note, MIR is not a target, but maybe (unlikely) we will add it somedays"
#endif

#ifdef X_WINDOW_SYSTEM

//includes for X11
#include <X11/X.h>
#include <gdk/gdkx.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>




void set_struts(GtkWidget *panel, gint strut_pos, gulong val)
{

	gtk_widget_realize(panel);

  Display *d = GDK_DISPLAY_XDISPLAY(gtk_widget_get_display(panel));

  Window win = gdk_x11_window_get_xid(gtk_widget_get_window(panel));



  Atom net_wm_strut = XInternAtom(d, "_NET_WM_STRUT", False);
  gulong struts[12] = {0,0,0,0,0,0,0,0,0,0,0,0};

  struts[strut_pos] = val;

  XChangeProperty(d, win, net_wm_strut, XA_CARDINAL, 32, PropModeReplace, (guchar *) &struts, 4);

  g_print("Struts set!");
}
#endif
