#include <X11/X.h>
#include <X11/Xatom.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <fcntl.h>
#include <json-c/json.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

// Function to create a font with a specific size
XFontStruct *createFont(Display *display, const char *fontName, int fontSize) {
  XFontStruct *fontInfo = XLoadQueryFont(display, fontName);
  if (!fontInfo) {
    fprintf(stderr, "Failed to load font: %s\n", fontName);
    return NULL;
  }

  // Adjust the font size
  fontInfo->ascent =
      (int)(fontSize * fontInfo->ascent / fontInfo->max_bounds.ascent);
  fontInfo->descent =
      (int)(fontSize * fontInfo->descent / fontInfo->max_bounds.descent);
  fontInfo->max_bounds.ascent = fontInfo->ascent;
  fontInfo->max_bounds.descent = fontInfo->descent;
  fontInfo->min_bounds.ascent = fontInfo->ascent;
  fontInfo->min_bounds.descent = fontInfo->descent;

  return fontInfo;
}

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

    // Get the text's font size
    struct json_object *font_size;
    json_object_object_get_ex(component, "font_size", &font_size);

    // Create a font with a specific size
    // FIXME: change fixed to font coming from component
    XFontStruct *font = createFont(display, "fixed", json_object_get_int(font_size));
    if (!font) {
      XCloseDisplay(display);
      exit(1);
    }
    // Set the font
    XSetFont(display, DefaultGC(display, DefaultScreen(display)), font->fid);

    // Split the text into lines
    char *text_copy = strdup(json_object_get_string(text));
    char *line = strtok(text_copy, "\n");
    int line_height = 0;
    while (line != NULL) {
      XDrawString(display, window, DefaultGC(display, DefaultScreen(display)),
                  json_object_get_int(x) + 5,
                  json_object_get_int(y) + 5 + line_height, line, strlen(line));
      line = strtok(NULL, "\n");
      line_height += 20;
    }

    // Clean up
    XFreeFont(display, font);
    free(text_copy);
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
  Atom dialogType = XInternAtom(display, "_NET_WM_WINDOW_TYPE_DIALOG", False);
  Atom wmWindowType = XInternAtom(display, "_NET_WM_WINDOW_TYPE", False);
  XChangeProperty(display, window, wmWindowType, XA_ATOM, 32, PropModeReplace,
                  (unsigned char *)&dialogType, 1);
  // Select the events to listen to
  XSelectInput(display, window,
               ExposureMask | StructureNotifyMask | KeyPressMask);

  // Show the window
  XMapWindow(display, window);

  // Flush the display to show the changes
  XFlush(display);

  // Create the named pipe for writing
  const char *write_pipe_path = "/tmp/c_to_python";
  mkfifo(write_pipe_path, 0777);

  // Create the named pipe for reading
  const char *read_pipe_path = "/tmp/python_to_c";

  // Open the named pipe for reading
  int read_pipe_fd = open(read_pipe_path, O_RDONLY);

  // Check if the named pipe was opened successfully
  if (read_pipe_fd == -1) {
    fprintf(stderr, "Failed to open named pipe for reading\n");
    return 1;
  }
  printf("C: Named pipe opened successfully\n");

  // Open the named pipe for writing
  int write_pipe_fd = open(write_pipe_path, O_WRONLY);

  // Check if the named pipe was opened successfully
  if (write_pipe_fd == -1) {
    fprintf(stderr, "Failed to open named pipe for writing\n");
    return 1;
  }
  printf("C: Named pipe opened successfully\n");

  // Event object
  XEvent event;

  // Buffer for reading messages from the read pipe
  char buffer[1024 * 1000];

  // Event loop
  while (1) {
    // Wait for an event
    XNextEvent(display, &event);

    if (event.type == Expose) {
      json_object *event_json = json_object_new_object();

      // Send the expose event to the Python application
      json_object_object_add(event_json, "expose", json_object_new_boolean(1));

      const char *message = json_object_to_json_string(event_json);

      write(write_pipe_fd, message, strlen(message));

      // Clean up the JSON object
      json_object_put(event_json);

      // Read the message from the Python application
      int bytes_read = read(read_pipe_fd, buffer, sizeof(buffer));
      if (bytes_read > 0) {
        // Null-terminate the message
        buffer[bytes_read] = '\0';
        // Draw the window content based on the received message
        drawWindowContent(display, window, buffer);
      }
    }

    if (event.type == KeyPress) {
      json_object *pressed_json = json_object_new_object();

      // Send the key pressed to the Python application
      KeySym key;
      char key_buffer[10];
      XLookupString(&event.xkey, key_buffer, sizeof(key_buffer), &key, NULL);

      unsigned int state = event.xkey.state;

      // Check for modifier keys
      if (state & ControlMask) {
        json_object_object_add(pressed_json, "ctrl",
                               json_object_new_boolean(1));
      }
      if (state & ShiftMask) {
        json_object_object_add(pressed_json, "shift",
                               json_object_new_boolean(1));
      }
      if (state & Mod1Mask) {
        json_object_object_add(pressed_json, "alt", json_object_new_boolean(1));
      }

      // Check for escape
      if (key == XK_Escape) {
        json_object_object_add(pressed_json, "key",
                               json_object_new_string("escape"));
      }

      // Check for tab
      else if (key == XK_Tab) {
        json_object_object_add(pressed_json, "key",
                               json_object_new_string("tab"));
      }

      // Check for space
      else if (key == XK_space) {
        json_object_object_add(pressed_json, "key",
                               json_object_new_string("space"));
      }

      // Check for numbers
      else if (key >= XK_0 && key <= XK_9) {
        json_object_object_add(pressed_json, "key",
                               json_object_new_int(key - XK_0));
      }

      // Check for letters
      else if ((key >= XK_A && key <= XK_Z) || (key >= XK_a && key <= XK_z)) {
        json_object_object_add(pressed_json, "key",
                               json_object_new_string(key_buffer));
      }

      // Check for unknown keys
      else {
        json_object_object_add(pressed_json, "key",
                               json_object_new_string("unknown"));
      }

      // Send the message to the Python application
      const char *message = json_object_to_json_string(pressed_json);
      write(write_pipe_fd, message, strlen(message));

      // Cleanup
      json_object_put(pressed_json);

      // Read the message from the Python application
      int bytes_read = read(read_pipe_fd, buffer, sizeof(buffer));
      if (bytes_read > 0) {
        // Null-terminate the message
        buffer[bytes_read] = '\0';
        // Draw the window content based on the received message
        drawWindowContent(display, window, buffer);
      }
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

  // Close the named pipe
  close(read_pipe_fd);

  // Cleanup and exit
  XDestroyWindow(display, window);
  XCloseDisplay(display);

  return 0;
}
