#include <pebble.h>
#include "default.h"
#include "weather.h"
#include "location.h"
#include "tablet.h"

enum weather_layers {
  TEMPERATURE,
  CONDITION,
  CITY,
  LAYER_NUMS
};

static TextLayer* wl[LAYER_NUMS];

void weather_update(struct tm* t){
  text_layer_set_text(wl[TEMPERATURE], tabletGetCurrentTemperature() );
  text_layer_set_text(wl[CONDITION], tabletGetCurrentCondition() );
}

void weather_main(Window* window){
  //温度の表示
  wl[TEMPERATURE] = text_layer_create(GRect(0,168-28,144,28));
	text_layer_set_background_color(wl[TEMPERATURE],TINA_BACKGROUND_COLOR);
	text_layer_set_text_color(wl[TEMPERATURE],TINA_FORGROUND_COLOR);
	text_layer_set_text_alignment(wl[TEMPERATURE], GTextAlignmentRight); 
	text_layer_set_font(wl[TEMPERATURE], fonts_get_system_font(FONT_KEY_ROBOTO_CONDENSED_21));
	text_layer_set_text(wl[TEMPERATURE],"...");
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(wl[TEMPERATURE]));

  // 天候の表示
  wl[CONDITION] = text_layer_create(GRect(0,168-28,100,14));
	text_layer_set_background_color(wl[CONDITION],GColorClear);
	text_layer_set_text_color(wl[CONDITION],TINA_FORGROUND_COLOR);
	text_layer_set_text_alignment(wl[CONDITION], GTextAlignmentLeft); 
	text_layer_set_font(wl[CONDITION], fonts_get_system_font(FONT_KEY_GOTHIC_14));
	text_layer_set_text(wl[CONDITION],"...");
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(wl[CONDITION]));
}
void weather_exit(Window* window){
  for(int n=0; n<LAYER_NUMS; ++n){
    if(wl[n]){
      text_layer_destroy(wl[n]);
    }
    wl[n] = NULL;
  }
}
