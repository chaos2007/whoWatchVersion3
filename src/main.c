#include <pebble.h>
  
static Window *s_main_window;
static GFont *s_time_font;
static TextLayer *s_time_layer;
static GFont *s_date_font;
static TextLayer *s_date_layer;
static BitmapLayer *s_background_layer;
static GBitmap *s_background_bitmap;
static InverterLayer *inverter_layer;

static void update_time() {
  // Get a tm structure
  time_t temp = time(NULL); 
  struct tm *tick_time = localtime(&temp);

  // Create a long-lived buffer
  static char buffer[] = "00:00";
  static char dateBuffer[] = "Thu Aug 23";

  // Write the current hours and minutes into the buffer
  if(clock_is_24h_style() == true) {
    //Use 2h hour format
    strftime(buffer, sizeof("00:00"), "%H:%M", tick_time);
  } else {
    //Use 12 hour format
    strftime(buffer, sizeof("00:00"), "%I:%M", tick_time);
  }
  strftime(dateBuffer, sizeof("Thu Aug 23"), "%a %b %e", tick_time);  

  // Display this time on the TextLayer
  text_layer_set_text(s_time_layer, buffer);
  text_layer_set_text(s_date_layer, dateBuffer);
    
  
}

static void main_window_load(Window *window) {
  // Create GBitmap, then set to created BitmapLayer
  s_background_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_DOCTOR_WHO_LOGO);
  s_background_layer = bitmap_layer_create(GRect(0, 0, 140, 82));
  bitmap_layer_set_bitmap(s_background_layer, s_background_bitmap);
  bitmap_layer_set_background_color(s_background_layer, GColorBlack);
  bitmap_layer_set_compositing_mode(s_background_layer, GCompOpAssignInverted);
    
  // Create time TextLayer
  s_time_layer = text_layer_create(GRect(0, 54, 144, 56));
  #ifdef PBL_COLOR
  text_layer_set_background_color(s_time_layer, GColorBlue);
  #else
  text_layer_set_background_color(s_time_layer, GColorClear);
  #endif
  text_layer_set_text_color(s_time_layer, GColorWhite);
  text_layer_set_text(s_time_layer, "00:00");
   
  // Create date TextLayer  
  s_date_layer = text_layer_create(GRect(0, 110, 144, 30));
  text_layer_set_background_color(s_date_layer, GColorClear);
  text_layer_set_text_color(s_date_layer, GColorWhite);
  text_layer_set_text(s_date_layer, "Thu Aug 23");  

  // Improve the layout to be more like a watchface
  // Create GFont
  s_time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_DOCTOR_WHO_56));
  s_date_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_DOCTOR_WHO_30));

  // Apply to TextLayer
  text_layer_set_font(s_time_layer, s_time_font);
  text_layer_set_font(s_date_layer, s_date_font);
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);
  text_layer_set_text_alignment(s_date_layer, GTextAlignmentCenter);
    
  // Inverter Layer
  inverter_layer = inverter_layer_create(GRect(0,0,144,168));
  layer_add_child(window_get_root_layer(window), inverter_layer_get_layer(inverter_layer));
  

  // Add it as a child layer to the Window's root layer
  layer_add_child(inverter_layer_get_layer(inverter_layer), bitmap_layer_get_layer(s_background_layer));
  layer_add_child(inverter_layer_get_layer(inverter_layer), text_layer_get_layer(s_time_layer));
  layer_add_child(inverter_layer_get_layer(inverter_layer), text_layer_get_layer(s_date_layer));

  //layer_set_hidden(inverter_layer_get_layer(inverter_layer), false);
  // Make sure the time is displayed from the start
  update_time();
}

static void main_window_unload(Window *window) {
  // Destroy GBitmap
  gbitmap_destroy(s_background_bitmap);

  // Destroy BitmapLayer
  bitmap_layer_destroy(s_background_layer);  
    
  // Unload GFont
  fonts_unload_custom_font(s_time_font);
  fonts_unload_custom_font(s_date_font);
    
  // Destroy TextLayer
  text_layer_destroy(s_time_layer);
  text_layer_destroy(s_date_layer);
    
  // Destroy Inverter Layer.
  inverter_layer_destroy(inverter_layer);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
}
  
static void init() {
  // Create main Window element and assign to pointer
  s_main_window = window_create();

  // Set handlers to manage the elements inside the Window
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });

  // Show the Window on the watch, with animated=true
  window_stack_push(s_main_window, true);
  
  // Register with TickTimerService
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
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
