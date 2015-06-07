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
    Atom atom = XInternAtom(d, "WM_ICON_NAME", False), type;
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

void unhide(Display *d, Window w)
{
    g_warning("Not Implemeted Function");
}

#endif //X_WINDOW_SYSTEM
