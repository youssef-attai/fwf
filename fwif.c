#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <fcntl.h>
#include <json-c/json.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

// Function to draw the window content based on the received message
void drawWindowContent(Display *display, Window window, const char *message) {
  // Clear the window
  XClearWindow(display, window);

  // Read JSON components from the message and draw them on the window
  struct json_object *json = json_tokener_parse(message);
  struct json_object *components;
  json_object_object_get_ex(json, "components", &components);
  int components_count = json_object_array_length(components);

  struct json_object *component; // Component iterator

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
    XFillRectangle(display, window, DefaultGC(display, DefaultScreen(display)),
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
      XDrawRectangle(display, window,
                     DefaultGC(display, DefaultScreen(display)),
                     json_object_get_int(x), json_object_get_int(y),
                     json_object_get_int(width), json_object_get_int(height));
    }
  }

  // Free the JSON object
  json_object_put(json);

  // Flush the display to show the changes
  XFlush(display);
}
int main() {
  // Initialize X11 display and create the window
  Display *display = XOpenDisplay(NULL);

  // Check if the display was opened successfully
  if (display == NULL) {
    fprintf(stderr, "Failed to open X11 display\n");
    return 1;
  }

  // Get the default screen
  int screen = DefaultScreen(display);

  // Create the window
  Window window = XCreateSimpleWindow(
      display, RootWindow(display, screen), 0, 0, 400, 300, 0,
      BlackPixel(display, screen), WhitePixel(display, screen));
  // Select the events to listen to
  XSelectInput(display, window,
               ExposureMask | StructureNotifyMask | KeyPressMask);

  // Show the window
  XMapWindow(display, window);

  // Flush the display to show the changes
  XFlush(display);

  // Event object
  XEvent event;

  // Event loop
  while (1) {
    // Wait for an event
    XNextEvent(display, &event);

    if (event.type == Expose) {
    }

    if (event.type == KeyPress) {
    }

    if (event.type == ClientMessage && event.xclient.data.l[0] == 0) {
      // Exit the event loop when receiving the termination message from the
      // Python application
      break;
    }

    if (event.type == DestroyNotify) {
      // Exit the event loop when the window is destroyed (closed)
      break;
    }
  }
  // Cleanup and exit
  XDestroyWindow(display, window);
  XCloseDisplay(display);

  return 0;
}
