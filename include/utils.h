#ifndef _UTILS_H_
#define _UTILS_H_

#include <stdint.h>
#include <X11/Xutil.h>
#include <X11/Xlib.h>

void log_error(const char *msg);
void log_error2(const char *msg, const char *msg2);
void log_warning(const char *msg);
void log_info(const char *msg);

typedef union rgba_t {
  struct {
    uint8_t b;
    uint8_t g;
    uint8_t r;
    uint8_t a;
  };
  uint32_t v;
} rgba_t;

rgba_t parse_color(const char *str, const rgba_t def);

typedef struct monitor_t {
  char name[32];
  unsigned int width;
  unsigned int height;
  unsigned int x;
  unsigned int y;
} monitor_t;

monitor_t get_monitor(Display *display, Window root, char *monitor_name);

typedef struct strut_t {
  unsigned int left;
  unsigned int right;
  unsigned int top;
  unsigned int bottom;
} strut_t;

typedef struct strut_partial_t {
  unsigned int left;
  unsigned int right;
  unsigned int top;
  unsigned int bottom;
  unsigned int left_start_y;
  unsigned int left_end_y;
  unsigned int right_start_y;
  unsigned int right_end_y;
  unsigned int top_start_x;
  unsigned int top_end_x;
  unsigned int bottom_start_x;
  unsigned int bottom_end_x;
} strut_partial_t;

strut_t get_window_strut(int width, int height, int x, int y);
strut_partial_t get_window_strut_partial(int width, int height, int x, int y);

Window get_window_by_name(Display *display, Window *window, char *needle);
Window get_window_by_class(Display *display, Window *window, char *needle);

char *get_window_class(Display *display, Window window_id);
char *get_window_name(Display *display, Window window_id);

void set_window_wm_hints(Display *display, Window *window, int flags, int initial_state, Bool handle_input);
void set_window_size_hints(Display *display, Window *window, unsigned int width, unsigned int height);
void set_window_name(Display *display, Window *window, char *wm_name, char *instance_name, char *class_name);

void change_stack_mode(Display *display, Window *window, Window *sibling, int stack_mode);

#endif
