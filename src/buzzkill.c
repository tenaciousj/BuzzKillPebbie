#include <pebble.h>

#define SCREEN_WIDTH 144
#define SCREEN_HEIGHT 168  
#define BUFF 64
#define TRUE 1
#define FALSE 0

static Window* window;
static TextLayer* timer_text_layer;
static TextLayer* site_text_layer;
static int time1 = 0;

static AppSync sync;
static uint8_t sync_buffer[BUFF];
static int is_running = FALSE;



enum MsgKeys {
  OUR_TIME = 0x0,
  OUR_SITE = 0x0
};

static void sync_error(DictionaryResult dict_error, 
                       AppMessageResult app_message_error, void *context) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "App Message Sync Error: %d", app_message_error);
}

static void sync_success(const uint32_t key, const Tuple* new_tuple, 
                         const Tuple* old_tuple, void* context) {

  
  APP_LOG(APP_LOG_LEVEL_DEBUG, "hello");
  if(strcmp(new_tuple->value->cstring,"good to go") == 0) { // Returns 0 when strings are equal
    is_running = FALSE;
    time1 = 0;
    APP_LOG(APP_LOG_LEVEL_DEBUG, "false");
    text_layer_set_text(timer_text_layer, "0h\n0m\n0s");
    text_layer_set_text(site_text_layer, "You are on kosher sites...");
    
  }
  else {
    is_running = TRUE;
    
    // Update the  text for URL
    vibes_short_pulse();
    static char site_buffer1[32];
    snprintf(site_buffer1, sizeof(site_buffer1), new_tuple->value->cstring);
    text_layer_set_text(site_text_layer, site_buffer1);
  
    
    APP_LOG(APP_LOG_LEVEL_DEBUG, "true");
    
  }
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed){
  
  if(is_running == TRUE){
    
    
    // Use a long-lived buffer
    static char time_buffer1[32];
    
    // Get time since launch
    int seconds = time1 % 60;
    int minutes = (time1 % 3600) / 60;
    int hours = time1 / 3600;
    
    if(seconds%15==0){
      vibes_cancel();
    }
    // Update the TextLayer
    snprintf(time_buffer1, sizeof(time_buffer1), "%dh\n%dm\n%ds", hours, minutes, seconds);
    text_layer_set_text(timer_text_layer, time_buffer1);
  
    // Increment time
    time1++;
  }
  
}

static void window_load(Window* window) {
  Layer* window_layer = window_get_root_layer(window);
  
  GRect bounds = layer_get_bounds(window_layer);
  timer_text_layer = text_layer_create((GRect) {
    .origin = {0, 10},
    .size   = {bounds.size.w, 95}
  });
  site_text_layer = text_layer_create((GRect){
    .origin = {0,105},
    .size = {bounds.size.w,35}
  });
  
  
  text_layer_set_text(timer_text_layer, "0h\n0m\n0s");
  text_layer_set_text_alignment(timer_text_layer, GTextAlignmentCenter);
  text_layer_set_text_color(timer_text_layer, GColorWhite);
  text_layer_set_background_color(timer_text_layer, GColorBlack);
  text_layer_set_font(timer_text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28));
  
  
  layer_add_child(window_layer, text_layer_get_layer(timer_text_layer));
  
  text_layer_set_text(site_text_layer, " ");
  text_layer_set_text_alignment(site_text_layer, GTextAlignmentCenter);
  text_layer_set_text_color(site_text_layer, GColorWhite);
  text_layer_set_background_color(site_text_layer, GColorBlack);
  text_layer_set_font(site_text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14));
  
  layer_add_child(window_layer, text_layer_get_layer(site_text_layer));
  
  Tuplet initial_value[] = {
    TupletCString(OUR_TIME, "0h\n0m\n0s"),
    TupletCString(OUR_SITE, " ")
    
  };
  
  app_sync_init(&sync, sync_buffer, sizeof(sync_buffer), initial_value, 
                ARRAY_LENGTH(initial_value), sync_success, sync_error, NULL);
}


static void window_unload(Window* window) {
  text_layer_destroy(timer_text_layer);
  tick_timer_service_unsubscribe();
}


static void init(void) {
  window = window_create();
  window_set_window_handlers(window, (WindowHandlers){
    .load   = window_load,
    .unload = window_unload
  });
  int animated = 1;
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