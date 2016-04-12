#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <X11/Xos.h>
#include <X11/Xatom.h>
#include <X11/extensions/Xrandr.h>

#include "utils.h"

void log_error(const char *msg) {
  if (isatty(STDERR_FILENO))
    fprintf(stderr, "\033[1;31m err\033[1;37m ** \033[0m%s\n", msg);
  else
    fprintf(stderr, "%s\n", msg);
}

void log_error2(const char *msg, const char *msg2) {
  if (isatty(STDERR_FILENO))
    fprintf(stderr, "\033[1;31m err\033[1;37m ** \033[0m%s %s\n", msg, msg2);
  else
    fprintf(stderr, "%s\n", msg);
}

void log_warning(const char *msg) {
  if (isatty(STDERR_FILENO))
    fprintf(stderr, "\033[1;33mwarn\033[1;37m ** \033[0m%s\n", msg);
  else
    fprintf(stderr, "%s\n", msg);
}

void log_info(const char *msg) {
  if (isatty(STDOUT_FILENO))
    fprintf(stdout, "\033[1;36minfo\033[1;37m ** \033[0m%s\n", msg);
  else
    fprintf(stdout, "%s\n", msg);
}

rgba_t parse_color(const char *str, const rgba_t def)
{
  char *ptr = (char *) &str;

  if (!str || str[0] != '#' || strlen(str) != 9) {
    log_error("Invalid color format. Expects #AARRGGBB");
    return def;
  }

  rgba_t rgba;
  rgba.v = (unsigned int) strtoul(&str[1], &ptr, 16);
  rgba.r = (rgba.r * rgba.a) / 255;
  rgba.g = (rgba.g * rgba.a) / 255;
  rgba.b = (rgba.b * rgba.a) / 255;

  return rgba;
}

monitor_t get_monitor(Display *display, Window root, char *monitor_name)
{
  monitor_t monitor;
  memset(&monitor, 0, sizeof(monitor));

  char *name;
  int monitors;

  XRRMonitorInfo *info = XRRGetMonitors(display, root, 1, &monitors);

  for (int i = 0; i < monitors; i++) {
    name = (char *) XGetAtomName(display, info[i].name);

    if (strcmp(name, monitor_name) != 0) {
      continue;
    }

    strcpy(monitor.name, name);
    monitor.width = info[i].width;
    monitor.height = info[i].height;
    monitor.x = info[i].x;
    monitor.y = info[i].y;
    break;
  }

  free(name);
  free(info);

  return monitor;
}

strut_t get_window_strut(int width, int height, int x, int y)
{
  strut_t strut;
  memset(&strut, 0, sizeof(strut));

  strut.left = x;
  strut.right = x + width;
  strut.top = y;
  strut.bottom = y + height;

  return strut;
}

strut_partial_t get_window_strut_partial(int width, int height, int x, int y)
{
  strut_partial_t strut;
  memset(&strut, 0, sizeof(strut));

  strut.left = x;
  strut.right = x + width;
  strut.top = y;
  strut.bottom = y + height;
  strut.left_start_y = y;
  strut.left_end_y = y + height;
  strut.right_start_y = y;
  strut.right_end_y = y + height;
  strut.top_start_x = x;
  strut.top_end_x = x + width;
  strut.bottom_start_x = x;
  strut.bottom_end_x = x + width;

  return strut;
}

Window get_window_by_name(Display *display, Window *window, char *needle)
{
  Window match = 0, root, parent, *children;
  unsigned children_count;
  char *window_name;

  if (XFetchName(display, *window, &window_name) > 0) {
    int r = strcmp(needle, window_name);

    XFree(window_name);

    if (r == 0) return *window;
  }

  // Iterate subwindows recursively
  if (0 != XQueryTree(display, *window, &root, &parent, &children, &children_count)) {
    for (int i = 0; i < children_count; ++i) {
      Window win = get_window_by_name(display, &children[i], needle);

      if (win != 0) {
        match = win;
        break;
      }
    }

    XFree(children);
  }

  return match;
}

Window get_window_by_class(Display *display, Window *window, char *needle)
{
  Window match = 0, root, parent, *children;
  unsigned children_count;
  char *class = get_window_class(display, *window);

  if (strcmp(class, needle) == 0) {
    return *window;
  }

  // Iterate subwindows recursively
  if (0 != XQueryTree(display, *window, &root, &parent, &children, &children_count)) {
    for (int i = 0; i < children_count; ++i) {
      Window win = get_window_by_class(display, &children[i], needle);

      if (win != 0) {
        match = win;
        break;
      }
    }

    XFree(children);
  }

  return match;
}

char *get_window_class(Display *display, Window window_id)
{
  Atom property = XInternAtom(display, "WM_CLASS", 0), type;
  unsigned long remain, len;
  unsigned char *name;
  int form;

  if (XGetWindowProperty(display, window_id, property, 0, 1024, 0, XA_STRING, &type, &form, &len, &remain, &name) != 0) {
    return 0;
  }

  return (char *) name;
}

char *get_window_name(Display *display, Window window_id)
{
  Atom property = XInternAtom(display, "WM_NAME", 0), type;
  unsigned long remain, len;
  unsigned char *name;
  int form;

  if (XGetWindowProperty(display, window_id, property, 0, 1024, 0, XA_STRING, &type, &form, &len, &remain, &name) != 0) {
    return 0;
  }

  return (char *) name;
}

void set_window_size_hints(Display *display, Window *window, unsigned int width, unsigned int height)
{
  XSizeHints *size_hints = XAllocSizeHints();
  size_hints->flags = PMinSize;
  size_hints->min_width = width;
  size_hints->min_height = height;
  XSetWMSizeHints(display, *window, size_hints, XInternAtom(display, "WM_SIZE_HINTS", False));
  XFree(size_hints);
}

void set_window_wm_hints(Display *display, Window *window, int flags, int initial_state, Bool handle_input)
{
  // Window manager hints
  XWMHints *wm_hints;
  wm_hints = XAllocWMHints();
  wm_hints->flags = flags;
  wm_hints->initial_state = initial_state;
  wm_hints->input = handle_input;
  XSetWMHints(display, *window, wm_hints);
  XFree(wm_hints);
}

void set_window_name(Display *display, Window *window, char *wm_name, char *instance_name, char *class_name)
{
  XClassHint *class_hints = XAllocClassHint();
  class_hints->res_name  = instance_name;
  class_hints->res_class = class_name;
  XSetClassHint(display, *window, class_hints);
  XFree(class_hints);

  XStoreName(display, *window, wm_name);
}

void change_stack_mode(Display *display, Window *window, Window *sibling, int stack_mode)
{
  XWindowChanges changes;
  changes.sibling = (Window) *sibling;
  changes.stack_mode = stack_mode;
  XConfigureWindow(display, *window, CWSibling | CWStackMode, &changes);
}
