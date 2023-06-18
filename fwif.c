#include <X11/X.h>
#include <X11/Xatom.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <fcntl.h>
#include <json-c/json.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

int APPLICATION_LAYER_MESSAGE_BUFFER_SIZE = 1000000;

void fwif_load_environment();

const char *FWIF_WRITE_PIPE;
const char *FWIF_READ_PIPE;
int write_pipe;
int read_pipe;
int fwif_setup_write_pipe();
int fwif_setup_read_pipe();
void fwif_close_pipes();

Display *display;
Display *fwif_create_display();

int screen;
int fwif_create_screen();

int WINDOW_WIDTH;
int WINDOW_HEIGHT;
Window window;
Window fwif_create_window(const char *title);

void fwif_draw(struct json_object *);

void fwif_event_loop();
int fwif_handle_event(XEvent); // returns 0 if the event loop should stop
struct json_object *fwif_receive_updates();
struct json_object *fwif_create_event(const char *type);
void fwif_send_event(struct json_object *event);

void fwif_cleanup();
void fwif_destroy_window();
void fwif_destroy_display();

int main() {
  fwif_load_environment();

  printf("FWIF_WRITE_PIPE: %s\n", FWIF_WRITE_PIPE);
  printf("FWIF_READ_PIPE: %s\n", FWIF_READ_PIPE);

  read_pipe = fwif_setup_read_pipe();
  write_pipe = fwif_setup_write_pipe();

  display = fwif_create_display();
  screen = fwif_create_screen();

  WINDOW_WIDTH = 500;
  WINDOW_HEIGHT = 500;
  window = fwif_create_window("fwif");

  XMapWindow(display, window);

  fwif_event_loop();

  fwif_cleanup();
  return 0;
}

void fwif_event_loop() {
  XEvent event;
  int running = 1;
  while (running) {
    XNextEvent(display, &event);
    running = fwif_handle_event(event);
  }
}

void fwif_load_environment() {
  FWIF_WRITE_PIPE = getenv("FWIF_WRITE_PIPE");
  FWIF_READ_PIPE = getenv("FWIF_READ_PIPE");
}

int fwif_setup_write_pipe() {
  mkfifo(FWIF_WRITE_PIPE, 0666);
  int write_pipe = open(FWIF_WRITE_PIPE, O_WRONLY);
  if (write_pipe == -1) {
    fprintf(stderr, "Cannot open WRITE named pipe\n");
    exit(1);
  }
  return write_pipe;
}

int fwif_setup_read_pipe() {
  int read_pipe = open(FWIF_READ_PIPE, O_RDONLY);
  if (read_pipe == -1) {
    fprintf(stderr, "Cannot open READ named pipe\n");
    exit(1);
  }
  return read_pipe;
}

Display *fwif_create_display() {
  Display *display = XOpenDisplay(NULL);
  if (display == NULL) {
    fprintf(stderr, "Cannot open display\n");
    exit(1);
  }
  return display;
}

void fwif_destroy_display() { XCloseDisplay(display); }

int fwif_create_screen() {
  int screen = DefaultScreen(display);
  return screen;
}

Window fwif_create_window(const char *title) {
  int width = WINDOW_WIDTH;
  int height = WINDOW_HEIGHT;

  Window window =
      XCreateWindow(display, RootWindow(display, screen), 0, 0, width, height,
                    0, CopyFromParent, CopyFromParent, CopyFromParent, 0, NULL);
  XStoreName(display, window, title);

  Atom dialog_type = XInternAtom(display, "_NET_WM_WINDOW_TYPE_DIALOG", False);
  Atom wm_window_type = XInternAtom(display, "_NET_WM_WINDOW_TYPE", False);
  XChangeProperty(display, window, wm_window_type, XA_ATOM, 32, PropModeReplace,
                  (unsigned char *)&dialog_type, 1);

  XSelectInput(display, window,
               ExposureMask | StructureNotifyMask | KeyPressMask);
  return window;
}

void fwif_destroy_window() { XDestroyWindow(display, window); }

void fwif_draw(struct json_object *components) {
  // Create Pixmaps for double buffering
  Pixmap pixmap = XCreatePixmap(display, window, WINDOW_WIDTH, WINDOW_HEIGHT,
                                DefaultDepth(display, DefaultScreen(display)));

  // Clear the window
  XSetForeground(display, DefaultGC(display, DefaultScreen(display)),
                 WhitePixel(display, DefaultScreen(display)));
  XFillRectangle(display, pixmap, DefaultGC(display, DefaultScreen(display)), 0,
                 0, WINDOW_WIDTH, WINDOW_HEIGHT);

  int components_count =
      json_object_array_length(components); // Get the number of components
  struct json_object *component;            // Component iterator

  // Iterate over the components
  for (int i = 0; i < components_count; i++) {
    component = json_object_array_get_idx(components, i); // Get the component

    // Get the component's x and y position
    struct json_object *x;
    json_object_object_get_ex(component, "x", &x);
    struct json_object *y;
    json_object_object_get_ex(component, "y", &y);

    // Get the component's width and height
    struct json_object *width;
    json_object_object_get_ex(component, "width", &width);
    struct json_object *height;
    json_object_object_get_ex(component, "height", &height);

    // Get the component's background color
    struct json_object *background_color;
    json_object_object_get_ex(component, "background_color", &background_color);
    struct json_object *bg_red;
    json_object_object_get_ex(background_color, "r", &bg_red);
    struct json_object *bg_green;
    json_object_object_get_ex(background_color, "g", &bg_green);
    struct json_object *bg_blue;
    json_object_object_get_ex(background_color, "b", &bg_blue);

    // Get the component's border color
    struct json_object *border_color;
    json_object_object_get_ex(component, "border_color", &border_color);
    struct json_object *border_red;
    json_object_object_get_ex(border_color, "r", &border_red);
    struct json_object *border_green;
    json_object_object_get_ex(border_color, "g", &border_green);
    struct json_object *border_blue;
    json_object_object_get_ex(border_color, "b", &border_blue);

    // Get the component's border width
    struct json_object *border_width;
    json_object_object_get_ex(component, "border_width", &border_width);

    XSetForeground(display, DefaultGC(display, DefaultScreen(display)),
                   json_object_get_int(bg_red) << 16 |
                       json_object_get_int(bg_green) << 8 |
                       json_object_get_int(bg_blue));
    XFillRectangle(display, pixmap, DefaultGC(display, DefaultScreen(display)),
                   json_object_get_int(x), json_object_get_int(y),
                   json_object_get_int(width), json_object_get_int(height));

    // Change border attributes
    XSetLineAttributes(display, DefaultGC(display, DefaultScreen(display)),
                       json_object_get_int(border_width), LineSolid, CapButt,
                       JoinBevel);

    if (json_object_get_int(border_width) > 0) {
      XSetForeground(display, DefaultGC(display, DefaultScreen(display)),
                     json_object_get_int(border_red) << 16 |
                         json_object_get_int(border_green) << 8 |
                         json_object_get_int(border_blue));
      XDrawRectangle(display, pixmap,
                     DefaultGC(display, DefaultScreen(display)),
                     json_object_get_int(x), json_object_get_int(y),
                     json_object_get_int(width), json_object_get_int(height));
    }

    // Get the component's text
    struct json_object *text;
    json_object_object_get_ex(component, "text", &text);

    // Get the component's text color
    struct json_object *text_color;
    json_object_object_get_ex(component, "foreground_color", &text_color);
    struct json_object *text_red;
    json_object_object_get_ex(text_color, "r", &text_red);
    struct json_object *text_green;
    json_object_object_get_ex(text_color, "g", &text_green);
    struct json_object *text_blue;
    json_object_object_get_ex(text_color, "b", &text_blue);

    // Draw the text
    XSetForeground(display, DefaultGC(display, DefaultScreen(display)),
                   json_object_get_int(text_red) << 16 |
                       json_object_get_int(text_green) << 8 |
                       json_object_get_int(text_blue));

    // Split the text into lines
    char *text_copy = strdup(json_object_get_string(text));
    char *line = strtok(text_copy, "\n");
    int line_height = 0;
    while (line != NULL) {
      XDrawString(display, pixmap, DefaultGC(display, DefaultScreen(display)),
                  json_object_get_int(x) + 5,
                  json_object_get_int(y) + 5 + line_height, line, strlen(line));
      line = strtok(NULL, "\n");
      line_height += 20;
    }
    XCopyArea(display, pixmap, window,
              DefaultGC(display, DefaultScreen(display)), 0, 0, WINDOW_WIDTH,
              WINDOW_HEIGHT, 0, 0);
    free(text_copy);
  }
  XFlush(display);
}

void fwif_sync_components() {
  struct json_object *updates = fwif_receive_updates();

  struct json_object *components;
  json_object_object_get_ex(updates, "components", &components);

  fwif_draw(components);

  // json_object_put(updates);
  // json_object_put(components);
}

int fwif_handle_event(XEvent xevent) {
  struct json_object *event;

  switch (xevent.type) {
  case Expose:

    event = fwif_create_event("expose");

    fwif_send_event(event);

    fwif_sync_components();

    break;

  case KeyPress:

    event = fwif_create_event("keypress");

    json_object *pressed_json = json_object_new_object();

    KeySym key;
    char key_buffer[10];
    XLookupString(&xevent.xkey, key_buffer, sizeof(key_buffer), &key, NULL);

    unsigned int state = xevent.xkey.state;

    if (state & ControlMask)
      json_object_object_add(pressed_json, "ctrl", json_object_new_boolean(1));
    if (state & ShiftMask)
      json_object_object_add(pressed_json, "shift", json_object_new_boolean(1));
    if (state & Mod1Mask)
      json_object_object_add(pressed_json, "alt", json_object_new_boolean(1));

    if (key == XK_Escape)
      json_object_object_add(pressed_json, "key",
                             json_object_new_string("escape"));
    else if (key == XK_Tab)
      json_object_object_add(pressed_json, "key",
                             json_object_new_string("tab"));
    else if (key == XK_space)
      json_object_object_add(pressed_json, "key",
                             json_object_new_string("space"));
    else if (key >= XK_0 && key <= XK_9)
      json_object_object_add(pressed_json, "key",
                             json_object_new_int(key - XK_0));
    else if ((key >= XK_A && key <= XK_Z) || (key >= XK_a && key <= XK_z))
      json_object_object_add(pressed_json, "key",
                             json_object_new_string(key_buffer));
    else
      json_object_object_add(pressed_json, "key",
                             json_object_new_string("unknown"));

    json_object_object_add(event, "pressed", pressed_json);

    fwif_send_event(event);

    fwif_sync_components();

    // json_object_put(pressed_json);

    break;

  case ClientMessage:
    if (xevent.xclient.data.l[0] == 0)
      return 0;

  case DestroyNotify:
    return 0;
  }

  return 1;
}

struct json_object *fwif_receive_updates() {
  char buffer[APPLICATION_LAYER_MESSAGE_BUFFER_SIZE];
  int bytes_read = read(read_pipe, buffer, sizeof(buffer));

  if (bytes_read == -1) {
    fprintf(stderr, "Cannot read from pipe\n");
    exit(1);
  }

  if (bytes_read > 0)
    buffer[bytes_read] = '\0'; // Null-terminate the message

  printf("Received JSON: %s\n", buffer);

  struct json_object *json = json_tokener_parse(buffer);
  return json;
}

void fwif_close_pipes() {
  close(write_pipe);
  close(read_pipe);

  unlink(FWIF_WRITE_PIPE);
  unlink(FWIF_READ_PIPE);
}

void fwif_cleanup() {
  fwif_close_pipes();
  fwif_destroy_display();
  fwif_destroy_window();
}

struct json_object *fwif_create_event(const char *type) {
  struct json_object *event = json_object_new_object();
  json_object_object_add(event, "type", json_object_new_string(type));
  return event;
}

void fwif_send_event(struct json_object *event) {
  const char *event_string = json_object_to_json_string(event);
  write(write_pipe, event_string, strlen(event_string));
}
