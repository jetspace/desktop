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
vWindow *list_windows(Display *d, unsigned long *len)
{
    g_warning("WINDOW LISTS NOT IMPLEMENTED FOR WAYLAND!\nPLEASE NOTE: The Wayland version is HIGHLY experimental!");
    return;
}
#endif

#ifdef MIR
#error "Please note, MIR is not a target, but maybe (unlikely) we will add it somedays"
#endif

#ifdef X_WINDOW_SYSTEM
Window *list_windows(Display *d, unsigned long *len)
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
    Atom atom = XInternAtom(d, "WM_NAME", False), type;
    int form;
    unsigned long remain, len;
    unsigned char *list;

    if(Success != XGetWindowProperty(d, w, atom, 0, 1024, False, XA_STRING, &type,&form,&len,&remain,&list))
        {
            g_warning("Failed accessing WindowList (2)");
            return NULL;
        }

    return (char*)list;
}
#endif //X_WINDOW_SYSTEM
