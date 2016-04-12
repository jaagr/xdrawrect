#include <stdio.h>
#include <stdlib.h>

#include <string.h>
#include <errno.h>

#include "xdrawrect.h"

int main(int argc, char *argv[])
{
  if (argc < 9 || strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0) {
    printf("usage: %s monitor [top|bottom] width[%%] height[%%] x[%%] y[%%] color name [place_above_window_name]\n", argv[0]);
    return EXIT_FAILURE;
  }

  display = XOpenDisplay(NULL);
  screen = XDefaultScreen(display);
  root = XRootWindow(display, screen);
  monitor = get_monitor(display, root, argv[1]);

  if (strcmp(monitor.name, argv[1]) != 0) {
    log_error2("Monitor not found:", argv[1]);
    return EXIT_FAILURE;
  }

  if (strcmp("top", argv[2]) == 0) {
    gravity = GRAVITY_TOP;
  } else if (strcmp("bottom", argv[2]) == 0) {
    gravity = GRAVITY_BOTTOM;
  } else {
    log_error2("Invalid gravity:", argv[2]);
    log_info("Valid values: top, bottom\n");
    return EXIT_FAILURE;
  }

  if (strstr(argv[3], "%") != NULL)
    width = monitor.width * (atoi(argv[3]) / 100.0) + 0.5;
  else width = atoi(argv[3]);

  if (strstr(argv[4], "%") != NULL)
    height = monitor.height * (atoi(argv[4]) / 100.0) + 0.5;
  else height = atoi(argv[4]);

  if (strstr(argv[5], "%") != NULL) {
    pos_x = monitor.width * (atoi(argv[5]) / 100.0) + 0.5;
    pos_x -= width * (atoi(argv[5]) / 100.0) + 0.5;
  } else pos_x = atoi(argv[5]);

  if (strstr(argv[6], "%") != NULL) {
    pos_y = monitor.height * (atoi(argv[6]) / 100.0) + 0.5;
    pos_y -= height * (atoi(argv[6]) / 100.0) + 0.5;
  } else pos_y = atoi(argv[6]);

  if (gravity == GRAVITY_BOTTOM) {
    pos_y = monitor.height - pos_y - height;
  }

  // Move pos to the monitor viewport
  pos_x += monitor.x;
  pos_y += monitor.y;

  if (width > monitor.width) {
    log_error("The window cannot be wider than the viewport");
    return EXIT_FAILURE;
  } else if (height > monitor.height) {
    log_error("The window cannot be taller than the viewport");
    return EXIT_FAILURE;
  } else if (width <= 0) {
    log_error("The window cannot have a negative width");
    return EXIT_FAILURE;
  } else if (height <= 0) {
    log_error("The window cannot have a negative height");
    return EXIT_FAILURE;
  }

  rgba_t def;
  memset(&def, 0, sizeof(def));
  color = parse_color(argv[7], def);

  if (memcmp(&def, &color, sizeof(color)) == 0) {
    return 0;
  }

  memcpy(&name, &argv[8], sizeof(argv[8]));

  if (argc > 9) {
    target = get_window_by_name(display, &root, argv[9]);

    if (target == 0) {
      log_error2("Target window not found:", argv[9]);
      return EXIT_FAILURE;
    }
  }

  XMatchVisualInfo(display, screen, 32, TrueColor, &visual_info);
  colormap = XCreateColormap(display, root, visual_info.visual, screen);

  attr.colormap = colormap;
  attr.background_pixel = color.v;
  attr.border_pixel = 0;

  window = XCreateWindow(
      display, root, pos_x, pos_y, width, height, 0,
      visual_info.depth, InputOutput,
      visual_info.visual, CWBackPixel | CWColormap | CWBorderPixel | CWBackPixel, &attr);

  XFreeColormap(display, colormap);

  // Hints
  set_window_size_hints(display, &window, width, height);
  set_window_wm_hints(display, &window, StateHint | InputHint, NormalState, False);
  set_window_name(display, &window, name, "xdrawrect", "xdrawrect");

  // Properties
  Atom wm_desktop = XInternAtom(display, "_NET_WM_DESKTOP", 0);
  Atom wm_window_type = XInternAtom(display, "_NET_WM_WINDOW_TYPE", 0);
  Atom wm_window_type_dock = XInternAtom(display, "_NET_WM_WINDOW_TYPE_DOCK", 0);
  Atom wm_state = XInternAtom(display, "_NET_WM_STATE", 0);
  Atom wm_state_sticky = XInternAtom(display, "_NET_WM_STATE_STICKY", 0);
  Atom wm_state_above = XInternAtom(display, "_NET_WM_STATE_ABOVE", 0);
  Atom wm_strut = XInternAtom(display, "_NET_WM_STRUT", 0);
  Atom wm_strut_partial  = XInternAtom(display, "_NET_WM_STRUT_PARTIAL", 0);

  strut_t strut = get_window_strut(width, height, pos_x, pos_y);
  strut_partial_t strut_partial = get_window_strut_partial(width, height, pos_x, pos_y);

  XChangeProperty(display, window, wm_desktop, XA_CARDINAL, 32, PropModeReplace, (unsigned char *) (const int[]){ -1 },  1);
  XChangeProperty(display, window, wm_window_type, XA_ATOM, 32, PropModeReplace, (unsigned char *) &wm_window_type_dock,  1);
  XChangeProperty(display, window, wm_state, XA_ATOM, 32, PropModeReplace, (unsigned char *) &wm_state_sticky,  1);
  XChangeProperty(display, window, wm_state, XA_ATOM, 32, PropModeAppend, (unsigned char *) &wm_state_above,  1);
  XChangeProperty(display, window, wm_strut, XA_CARDINAL, 16, PropModeReplace, (unsigned char *) &strut, 4);
  XChangeProperty(display, window, wm_strut_partial, XA_CARDINAL, 16, PropModeReplace, (unsigned char *) &strut_partial, 12);

  if (target != 0) {
    change_stack_mode(display, &window, &target, Above);
  }

  // Subscribe to events
  //XSelectInput(display, window, ExposureMask | StructureNotifyMask | ButtonPressMask);

  // Show the window
  XMapWindow(display, window);

  while (1)  {
    XNextEvent(display, &event);
    // switch (event.type) {
    //   case XExposeEvent: event.xexpose; break;
    //   case XConfigureEvent: event.xconfigure break;
    //   case But
    // }
  }

  XDestroyWindow(display, window);
  XCloseDisplay(display);

  return EXIT_SUCCESS;
}
