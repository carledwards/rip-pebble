#include <pebble.h>

#define MAX_IMAGES 4

static Window *s_main_window;
static uint32_t image_resource_ids[MAX_IMAGES] = {RESOURCE_ID_MAC_SICK, RESOURCE_ID_TOMBSTONE, RESOURCE_ID_SAD_WINOS, RESOURCE_ID_STILL_ALLOCATED_0B};
static GBitmap *background_images[MAX_IMAGES];
static BitmapLayer *background_image_layers[MAX_IMAGES];
static int current_image_index = 0;

static void draw_next_image(void) {
  layer_set_hidden(bitmap_layer_get_layer(background_image_layers[current_image_index]), true);
  current_image_index = current_image_index + 1;
  if (current_image_index >= MAX_IMAGES) {
    current_image_index = 0;
  }
  layer_set_hidden(bitmap_layer_get_layer(background_image_layers[current_image_index]), false);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  BatteryChargeState charge = battery_state_service_peek();
  light_enable(charge.is_plugged);

  if (tick_time->tm_sec % 5 != 0) {
    return;
  }

  draw_next_image();
}

static void main_window_load(Window *window) {
  // Get information about the Window
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  for (int i = 0; i < MAX_IMAGES; i++) {
    background_images[i] = gbitmap_create_with_resource(image_resource_ids[i]);
    background_image_layers[i] = bitmap_layer_create(bounds);
    bitmap_layer_set_bitmap(background_image_layers[i], background_images[i]);
    bitmap_layer_set_alignment(background_image_layers[i], GAlignCenter);
    layer_add_child(window_layer, bitmap_layer_get_layer(background_image_layers[i]));
    layer_set_hidden(bitmap_layer_get_layer(background_image_layers[i]), true);
  }
  draw_next_image();
}

static void main_window_unload(Window *window) {
  for (int i = 0; i < MAX_IMAGES; i++) {
    bitmap_layer_destroy(background_image_layers[i]);
    gbitmap_destroy(background_images[i]);
  }
}

static void init() {
  s_main_window = window_create();

  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });

  window_stack_push(s_main_window, true);

  tick_timer_service_subscribe(SECOND_UNIT, tick_handler);
}

static void deinit() {
  // Destroy Window
  window_destroy(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}