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
    XSetForeground(display, DefaultGC(display, DefaultScreen(display)),
                   json_object_get_int(bg_red) << 16 |
                       json_object_get_int(bg_green) << 8 |
                       json_object_get_int(bg_blue));
    XFillRectangle(display, window, DefaultGC(display, DefaultScreen(display)),
                   json_object_get_int(x), json_object_get_int(y),
                   json_object_get_int(width), json_object_get_int(height));

  // Free the JSON object
  json_object_put(json);

  // Flush the display to show the changes
  XFlush(display);
}
