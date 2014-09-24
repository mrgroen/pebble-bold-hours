/*

  Bold Hours
  
  Pebble watch face for the brave and bold among us.
  Multiple configurations included!
  
  Author: Marcus Groen <mrgroen@gmail.com>
  Credits: Jon Eisen <jon@joneisen.me>

*/

// includes
#include <pebble.h>

// assets
static Window *s_main_window;
static BitmapLayer *s_hours_layer;
static GBitmap *s_hours_bitmap;
static unsigned int s_loaded_hour;
static TextLayer *s_minutes_layer;
static GRect s_minutes_frame;
static TextLayer *s_seconds_layer;
static GRect s_seconds_frame;
const int IMAGE_RESOURCE_IDS[12] = {
  RESOURCE_ID_IMAGE_NUM_1, RESOURCE_ID_IMAGE_NUM_2, RESOURCE_ID_IMAGE_NUM_3,
  RESOURCE_ID_IMAGE_NUM_4, RESOURCE_ID_IMAGE_NUM_5, RESOURCE_ID_IMAGE_NUM_6,
  RESOURCE_ID_IMAGE_NUM_7, RESOURCE_ID_IMAGE_NUM_8, RESOURCE_ID_IMAGE_NUM_9,
  RESOURCE_ID_IMAGE_NUM_10, RESOURCE_ID_IMAGE_NUM_11, RESOURCE_ID_IMAGE_NUM_12
};

// functions
static void load_hour_image(unsigned int hour) {
  if (hour < 1 || hour > 12) {
    return;
  }
  if (!s_hours_bitmap) {
    s_hours_bitmap = gbitmap_create_with_resource(IMAGE_RESOURCE_IDS[hour-1]);
    bitmap_layer_set_bitmap(s_hours_layer, s_hours_bitmap);
    s_loaded_hour = hour;
  }
}

static void unload_hour_image() {
  if (s_hours_bitmap) {
    gbitmap_destroy(s_hours_bitmap);
    s_loaded_hour = 0;
  }
}

/*
  Function to change the location of the minutes layer.
*/
void set_minute_layer_location(unsigned short horiz) {
  if (s_minutes_frame.origin.x != horiz) {
    s_minutes_frame.origin.x = horiz;
    layer_set_frame(text_layer_get_layer(s_minutes_layer), s_minutes_frame);
    layer_mark_dirty(text_layer_get_layer(s_minutes_layer));
  }
}

/*
  Function to change the location of the seconds layer.
*/
void set_seconds_layer_location(unsigned short horiz) {
  if (s_seconds_frame.origin.x != horiz) {
    s_seconds_frame.origin.x = horiz;
    layer_set_frame(text_layer_get_layer(s_seconds_layer), s_seconds_frame);
    layer_mark_dirty(text_layer_get_layer(s_seconds_layer));
  }
}

/*
  Function to update the time layers.
*/
static void update_time() {
  // get a tm structure
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);
  
  // 12 hour clock
  unsigned int hour = tick_time->tm_hour % 12;

  // converts "0" to "12"
  hour = hour ? hour : 12;

  // only do memory unload/load if necessary
  if (s_loaded_hour != hour) {
    unload_hour_image();
    load_hour_image(hour);
  }
  
  // long-lived buffer for the minutes
  static char s_minutes[] = "00";
  
  // set minutes
  strftime(s_minutes, sizeof("00"), "%M", tick_time);
  
  // change minutes layer location
  unsigned short n1s = (s_minutes[0]=='1') + (s_minutes[1]=='1');
  if (hour == 10 || hour == 12) {
    set_minute_layer_location(70 + 3*n1s);
  } else {
    set_minute_layer_location(53 + 3*n1s);
  }
  
  // show minutes
  text_layer_set_text(s_minutes_layer, s_minutes);
}

/*
  Function to update the seconds layer.
*/
static void update_seconds() {
  // get a tm structure
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);
  
  // 12 hour clock
  unsigned int hour = tick_time->tm_hour % 12;

  // converts "0" to "12"
  hour = hour ? hour : 12;
  
  // long-lived buffer for the seconds
  static char s_seconds[] = "00";
  
  // set seconds
  strftime(s_seconds, sizeof("00"), "%S", tick_time);
  
  // change seconds layer location
  unsigned short n1s = (s_seconds[0]=='1') + (s_seconds[1]=='1');
  if (hour == 10 || hour == 12) {
    set_seconds_layer_location(70 + 3*n1s);
  } else {
    set_seconds_layer_location(53 + 3*n1s);
  }
  
  // show seconds
  text_layer_set_text(s_seconds_layer, s_seconds);
}

/*
  Handler function to update the time, which gets triggered by the TickTimerService.
*/
static void handle_tick(struct tm *tick_time, TimeUnits units_changed) {
  if (units_changed & SECOND_UNIT) {
    update_seconds();
  }
  if (units_changed & MINUTE_UNIT) {
    update_time();
  }
}

/*
  Handler function to create the layers within the main window.
*/
static void main_window_load(Window *window) {
  // create layers
  s_seconds_frame = GRect(53, 83, 40, 40);
  s_seconds_layer = text_layer_create(s_seconds_frame);
  s_minutes_frame = GRect(53, 23, 40, 40);
  s_minutes_layer = text_layer_create(s_minutes_frame);
  s_hours_layer = bitmap_layer_create(GRect(0, 0, 144, 168));
  
  // setup seconds layer
  text_layer_set_text_color(s_seconds_layer, GColorWhite);
  text_layer_set_background_color(s_seconds_layer, GColorClear);
  text_layer_set_font(s_seconds_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ASAKIM_BOLD_38)));
  
  // setup minutes layer
  text_layer_set_text_color(s_minutes_layer, GColorWhite);
  text_layer_set_background_color(s_minutes_layer, GColorClear);
  text_layer_set_font(s_minutes_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ASAKIM_BOLD_38)));
  
  // setup layers
  layer_add_child(bitmap_layer_get_layer(s_hours_layer), text_layer_get_layer(s_seconds_layer));
  layer_add_child(bitmap_layer_get_layer(s_hours_layer), text_layer_get_layer(s_minutes_layer));
  layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_hours_layer));
}

/*
  Handler function to destroy all layers within the main window.
*/
static void main_window_unload(Window *window) {
  if (s_seconds_layer) {
    text_layer_destroy(s_seconds_layer);
  }
  if (s_minutes_layer) {
    text_layer_destroy(s_minutes_layer);
  }
  unload_hour_image();
  if (s_hours_layer) {
    bitmap_layer_destroy(s_hours_layer);
  }
}

static void init() {
  // create main window element and assign to pointer
  s_main_window = window_create();

  // set handlers to manage the elements inside the window
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });

  // subscribe to TickTimerService
  tick_timer_service_subscribe(SECOND_UNIT, handle_tick);
  
  // configure window
  window_set_background_color(s_main_window, GColorClear);
  window_set_fullscreen(s_main_window, true);
  window_stack_push(s_main_window, true);
  
  // make sure the time is displayed from the start
  update_time();
  update_seconds();
}

static void deinit() {
  // unsubscribe from TickTimerService
  tick_timer_service_unsubscribe();
  // destroy window
  window_destroy(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}