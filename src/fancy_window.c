#include "fancy_window.h"
#include <pebble.h>

// BEGIN AUTO-GENERATED UI CODE; DO NOT MODIFY
static Window *s_window;
static InverterLayer *s_seconds_layer;

static void initialise_ui(void) {
  s_window = window_create();
  window_set_background_color(s_window, GColorBlack);
  window_set_fullscreen(s_window, true);
  
  // s_seconds_layer
  s_seconds_layer = inverter_layer_create(GRect(0, 0, 4, 4));
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_seconds_layer);
}

static void destroy_ui(void) {
  window_destroy(s_window);
  inverter_layer_destroy(s_seconds_layer);
}
// END AUTO-GENERATED UI CODE

static void handle_window_unload(Window* window) {
  destroy_ui();
}

void show_fancy_window(void) {
  initialise_ui();
  window_set_window_handlers(s_window, (WindowHandlers) {
    .unload = handle_window_unload,
  });
  window_stack_push(s_window, true);
}

void hide_fancy_window(void) {
  window_stack_remove(s_window, true);
}
