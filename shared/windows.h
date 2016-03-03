/*
This file is licensed under the MIT-License
Copyright (c) 2015 Marius Messerschmidt
For more details view file 'LICENSE'
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <errno.h>


#ifdef WAYLAND
Window *list_windows(Display *d, unsigned long *len)
{
    g_warning("WINDOW LISTS NOT IMPLEMENTED FOR WAYLAND!\nPLEASE NOTE: The Wayland version is HIGHLY experimental!");
    return;
}
#endif

#ifdef MIR
#error "Please note, MIR is not a target, but maybe (unlikely) we will add it somedays"
#endif

#ifdef X_WINDOW_SYSTEM
Window *list_windows(Display *d, unsigned long*len)
{
    Atom atom = XInternAtom(d, "_NET_CLIENT_LIST", False), type;
    int form;
    unsigned long remain;
    unsigned char *list;

    if(Success != XGetWindowProperty(d, XDefaultRootWindow(d), atom, 0, 1024, False, XA_WINDOW,&type,&form,len,&remain,&list))
        {
            g_warning("Failed accessing WindowList (1)");
            return (Window*)list;
        }

    return (Window*)list;
}

char *get_window_name(Display *d, Window w)
{
  Atom atom = XInternAtom(d, "_NET_WM_NAME", False), type;
  int form;
  unsigned long remain, len;
  unsigned char *list;

  if(Success != XGetWindowProperty(d, w, atom, 0, 1024, False, AnyPropertyType, &type,&form,&len,&remain,&list))
      {
          g_warning("Failed accessing WindowList (2)");
          return NULL;
      }

  if(list != NULL)
    return (char *)list;

  char *name = NULL;
  XFetchName(d, w, &name);
  return name;
}


gboolean is_minimized(Display *d, Window w)
{
    Atom atom = XInternAtom(d, "_NET_WM_STATE", True);
    Atom type;
    int formart;
    unsigned long n, bytes;
    unsigned char *properties = NULL;



    if(Success != XGetWindowProperty(d, w, atom, 0, (~0L), False, AnyPropertyType, &type , &formart ,&n,&bytes,&properties))
        {
            g_warning("Failed accessing WindowList (3)");
            return FALSE;
        }


    type = XInternAtom(d, "_NET_WM_STATE_HIDDEN", True);
    for (unsigned int x = 0; x < n; ++x)
        if( type ==  ((unsigned long *)(properties))[x])
            return TRUE;

    return FALSE;

}

/*
For more informations about this function, please see:
http://stackoverflow.com/q/2858263

Only windowmanagers _without_ "focus stealing prevention" are working...
*/

void unhide(Display *d, Window w)
{
  XEvent xev;
  Window root;
  xev.type = ClientMessage;
  xev.xclient.display = d;
  xev.xclient.window = w;
  xev.xclient.message_type = XInternAtom(d, "_NET_ACTIVE_WINDOW",0);
  xev.xclient.format = 32;
  xev.xclient.data.l[0] = 2L;
  xev.xclient.data.l[1] = CurrentTime;
  root = XDefaultRootWindow(d);
  XSendEvent( d,root,0, SubstructureNotifyMask | SubstructureRedirectMask, &xev);
}

#endif //X_WINDOW_SYSTEM
