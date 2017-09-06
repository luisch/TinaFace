#include <pebble.h>
#include "watch.h"
#include "location.h"
#include "tablet.h"
#include "weather.h"

static GFont font_pebble_icomoon[3];
GFont getFontPebbleIcomoon36() { return font_pebble_icomoon[0]; }
GFont getFontPebbleIcomoon24() { return font_pebble_icomoon[1]; }
GFont getFontPebbleIcomoon14() { return font_pebble_icomoon[2]; }

static void bluetooth_check(bool connected){
  static const uint32_t const pulse3[] = {185,95,185,95,185};
  static VibePattern off = {
    .durations = pulse3,
    .num_segments = ARRAY_LENGTH(pulse3),
  };
  if(connected){
      vibes_double_pulse();
  }
  else {
    vibes_enqueue_custom_pattern(off);
  }
}

static void update(struct tm *tick_time, TimeUnits units_changed){
  watch_update(tick_time);
  tablet_update(tick_time);
  location_update(tick_time);
  weather_update(tick_time);
}

static void loadMainWindow(Window* w){
  APP_LOG(APP_LOG_LEVEL_INFO, "Loading main window");
  font_pebble_icomoon[0] = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_PEBBLE_ICOMOON_36));
  font_pebble_icomoon[1] = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_PEBBLE_ICOMOON_24));
  font_pebble_icomoon[2] = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_PEBBLE_ICOMOON_14));
  
  watch_main(w);
  tablet_main(w);
  weather_main(w);
  location_main(w);
  
  tick_timer_service_subscribe(SECOND_UNIT, update);
}
static void unloadMainWindow(Window* w){
  weather_exit(w);
  tablet_exit(w);
  location_exit(w);
  watch_exit(w);
}

int main(void) {  
  APP_LOG(APP_LOG_LEVEL_INFO, "start tina-watch");
  
	// メインウインドウとレイヤーの作成
  static Window* window;
	window = window_create();
	window_set_window_handlers(
		window,
		(WindowHandlers){.load=loadMainWindow, .unload=unloadMainWindow}
	);  
	window_stack_push(window, true);
  
  // bluetooth監視
  connection_service_subscribe((ConnectionHandlers) {
    .pebble_app_connection_handler = bluetooth_check
  });

  // アプリケーションのメインループ
  app_event_loop();
  
  // 終了処理
	window_destroy(window);
}
