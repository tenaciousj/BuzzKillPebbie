#include <pebble.h>

#define SCREEN_WIDTH 144
#define SCREEN_HEIGHT 168  
#define BUFF 64


static Window* window;
static TextLayer* text_layer;
static int time1 = 0;

static AppSync sync;
static uint8_t sync_buffer[BUFF];
static bool is_running = false;

enum MsgKeys {
  OUR_TIME = 0x0
};

static void sync_error(DictionaryResult dict_error, 
                       AppMessageResult app_message_error, void *context) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "App Message Sync Error: %d", app_message_error);
}

static void sync_success(const uint32_t key, const Tuple* new_tuple, 
                         const Tuple* old_tuple, void* context) {
APP_LOG(APP_LOG_LEVEL_DEBUG, "App Sync Success %s", new_tuple->value->cstring);
  text_layer_set_text(text_layer, new_tuple->value->cstring);
  if(strcmp(new_tuple->value->cstring,"good to go!") == 0) { // Returns 0 when strings are equal
    is_running = false;
    time1 = 0;
  }
  else {
    is_running = true;
  }
}


static void tick_handler(struct tm *tick_time, TimeUnits units_changed){
  
  if(is_running == false){
    return;
  }
  // Use a long-lived buffer
  static char time_buffer1[32];
  
  // Get time since launch
  int seconds = time1 % 60;
  int minutes = (time1 % 3600) / 60;
  int hours = time1 / 3600;
  
  // Update the TextLayer
  snprintf(time_buffer1, sizeof(time_buffer1), "%dh %dm %ds", hours, minutes, seconds);
  text_layer_set_text(text_layer, time_buffer1);

  // Increment time
  time1++;
}

static void window_load(Window* window) {
  Layer* window_layer = window_get_root_layer(window);
  
  GRect bounds = layer_get_bounds(window_layer);
  text_layer = text_layer_create((GRect) {
    .origin = {0, 30},
    .size   = {bounds.size.w, 150}
  });
  
//  text_layer_set_text(text_layer, "0h 0m 0s");
  text_layer_set_text_alignment(text_layer, GTextAlignmentCenter);
  text_layer_set_text_color(text_layer, GColorWhite);
  text_layer_set_background_color(text_layer, GColorBlack);
  GSize size = GSize(SCREEN_WIDTH, 50);
  text_layer_set_size(text_layer, size);
  
  
  layer_add_child(window_layer, text_layer_get_layer(text_layer));
  
  Tuplet initial_value[] = {
    TupletCString(OUR_TIME, "0h 0m 0s")
  };
  
  app_sync_init(&sync, sync_buffer, sizeof(sync_buffer), initial_value, 
                ARRAY_LENGTH(initial_value), sync_success, sync_error, NULL);
}


static void window_unload(Window* window) {
  text_layer_destroy(text_layer);
  tick_timer_service_unsubscribe();
}


static void init(void) {
  window = window_create();
  window_set_window_handlers(window, (WindowHandlers){
    .load   = window_load,
    .unload = window_unload
  });
  bool animated = true;
  app_message_open(BUFF, BUFF);
  window_stack_push(window, animated);
  tick_timer_service_subscribe(SECOND_UNIT, tick_handler);
  
}

static void deinit(void) {
  window_destroy(window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}