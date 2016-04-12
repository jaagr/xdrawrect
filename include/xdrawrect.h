#ifndef _XDRAWRECT_H_
#define _XDRAWRECT_H_

#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xos.h>

#include "utils.h"

enum {
  GRAVITY_TOP = 0,
  GRAVITY_BOTTOM,
};

Display *display;
monitor_t monitor;
int screen;

Window root;
Window window;
Window target;

Colormap colormap;
XVisualInfo visual_info;

XSetWindowAttributes attr;
XEvent event;

char *name;
int width;
int height;
int pos_x;
int pos_y;
int gravity;
rgba_t color;


#endif
