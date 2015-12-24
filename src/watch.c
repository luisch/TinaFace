#include <pebble.h>
#include "default.h"
#include "watch.h"

enum layer_names {
  TIME,
  DATE,
  CHARGE,
  LAYER_NUM
};
static TextLayer* wl[LAYER_NUM];
static BitmapLayer* image_layer;

void watch_update(struct tm *tick_time){
  
  //時刻
  const char* const time_format[2] = {
    clock_is_24h_style() ? "%H:%M" : "%I:%M",
    clock_is_24h_style() ? "%H.%M" : "%I.%M",
  };
  static char str_time[32];
  strftime(str_time, sizeof(str_time), time_format[tick_time->tm_sec%2], tick_time);
  text_layer_set_text(wl[TIME], str_time);
  
  //日付
  static char str_date[32];
  strftime(str_date, sizeof(str_date), "%a %d", tick_time);
  text_layer_set_text(wl[DATE],str_date);
  
  //バッテリー残量
  static char str_battery[32];
  BatteryChargeState charge_state = battery_state_service_peek();
  if( !charge_state.is_charging ){
    snprintf(str_battery, sizeof(str_battery),  connection_service_peek_pebble_app_connection() ? "%d%%" : "%d%%[-]", charge_state.charge_percent);
  }
  else {
    snprintf(str_battery, sizeof(str_battery),  "[+]");
  }
  text_layer_set_text(wl[CHARGE],str_battery);
}

void watch_main(Window* window)
{
  APP_LOG(APP_LOG_LEVEL_INFO, "on watch_main");
  static Layer* main_layer;
  main_layer = window_get_root_layer(window);
	GRect bounds = layer_get_bounds(main_layer);
  
	// 背景のビットマップ
	bitmap_layer_set_bitmap(
		image_layer = bitmap_layer_create(bounds),
		gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BACKGROUND)
	);
	layer_add_child(main_layer, bitmap_layer_get_layer(image_layer));
  
  // 時刻表示レイヤー
	wl[TIME] = text_layer_create(GRect(0,0,144,35));
	text_layer_set_background_color(wl[TIME],TINA_BACKGROUND_COLOR);
	text_layer_set_text_color(wl[TIME],TINA_FORGROUND_COLOR);
	text_layer_set_text(wl[TIME],"--:--");
	text_layer_set_font(wl[TIME], fonts_get_system_font(FONT_KEY_BITHAM_34_MEDIUM_NUMBERS));
	text_layer_set_text_alignment(wl[TIME], GTextAlignmentRight); 
	layer_add_child(main_layer, text_layer_get_layer(wl[TIME]));

  // 日付
  wl[DATE] = text_layer_create(GRect(0,0,75,20));
	text_layer_set_background_color(wl[DATE],GColorClear);
	text_layer_set_text_color(wl[DATE],TINA_FORGROUND_COLOR);
	text_layer_set_text(wl[DATE],"...");
	text_layer_set_font(wl[DATE], fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
	text_layer_set_text_alignment(wl[DATE], GTextAlignmentLeft); 
	layer_add_child(main_layer, text_layer_get_layer(wl[DATE]));
  
  // バッテリー残量
  wl[CHARGE] = text_layer_create(GRect(0,20,55,15));
	text_layer_set_background_color(wl[CHARGE],GColorClear);
	text_layer_set_text_color(wl[CHARGE],TINA_FORGROUND_COLOR);
	text_layer_set_text(wl[CHARGE],"...");
	text_layer_set_font(wl[CHARGE], fonts_get_system_font(FONT_KEY_GOTHIC_14));
	text_layer_set_text_alignment(wl[CHARGE], GTextAlignmentLeft); 
	layer_add_child(main_layer, text_layer_get_layer(wl[CHARGE]));
}

void watch_exit(Window* window){
  for(int n=0; n<LAYER_NUM; ++n){
    if(wl[n]){
      text_layer_destroy(wl[n]);
    }
    wl[n] = NULL;
  }
  bitmap_layer_destroy(image_layer);
}
