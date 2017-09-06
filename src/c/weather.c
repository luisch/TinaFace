#include <pebble.h>
#include "default.h"
#include "weather.h"
#include "location.h"
#include "tablet.h"

enum weather_layers {
  TEMPERATURE,
  CONDITION,
  FORECAST,
  CITY,
  WIND,
  LAYER_NUMS
};

static TextLayer* wl[LAYER_NUMS];

void weather_update(struct tm* t){
  text_layer_set_text(wl[TEMPERATURE], tabletGetCurrentTemperature() );
  text_layer_set_text(wl[CONDITION], tabletGetCurrentCondition() );
  text_layer_set_text(wl[FORECAST], tabletGetForecast() );
  text_layer_set_text(wl[WIND], tabletGetCurrentWind() );
}

void weather_main(Window* window){
  // 天候の表示
  wl[CONDITION] = text_layer_create(GRect(144-80,168-52,36,36));
  text_layer_set_font(wl[CONDITION], getFontPebbleIcomoon36());
	text_layer_set_background_color(wl[CONDITION],TINA_BACKGROUND_COLOR);
	text_layer_set_text_color(wl[CONDITION],TINA_FORGROUND_COLOR);
	text_layer_set_text_alignment(wl[CONDITION], GTextAlignmentCenter); 
	text_layer_set_text(wl[CONDITION],"\ue010");
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(wl[CONDITION]));

  // 予報の表示
  wl[FORECAST] = text_layer_create(GRect(144-80+36,168-31,77-36,15));
  text_layer_set_font(wl[FORECAST], getFontPebbleIcomoon14());
	text_layer_set_background_color(wl[FORECAST],TINA_BACKGROUND_COLOR);
	text_layer_set_text_color(wl[FORECAST],TINA_FORGROUND_COLOR);
	text_layer_set_text_alignment(wl[FORECAST], GTextAlignmentRight); 
	text_layer_set_text(wl[FORECAST],"");
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(wl[FORECAST]));

  //温度の表示
  wl[TEMPERATURE] = text_layer_create(GRect(144-80+35,168-49,77-35,18));
	text_layer_set_background_color(wl[TEMPERATURE],TINA_BACKGROUND_COLOR);
	text_layer_set_text_color(wl[TEMPERATURE],TINA_FORGROUND_COLOR);
	text_layer_set_font(wl[TEMPERATURE], fonts_get_system_font(FONT_KEY_GOTHIC_18));
	text_layer_set_text_alignment(wl[TEMPERATURE], GTextAlignmentRight);
	text_layer_set_text(wl[TEMPERATURE],"...");
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(wl[TEMPERATURE]));

  // 風速の表示
  wl[WIND] = text_layer_create(GRect(144-80+35,168-63,77-35,18));
	text_layer_set_background_color(wl[WIND],TINA_BACKGROUND_COLOR);
	text_layer_set_text_color(wl[WIND],TINA_FORGROUND_COLOR);
	text_layer_set_text_alignment(wl[WIND], GTextAlignmentRight); 
	text_layer_set_font(wl[WIND], fonts_get_system_font(FONT_KEY_GOTHIC_18));
	text_layer_set_text(wl[WIND],"...");
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(wl[WIND]));  
}
void weather_exit(Window* window){
  for(int n=0; n<LAYER_NUMS; ++n){
    if(wl[n]){
      text_layer_destroy(wl[n]);
    }
    wl[n] = NULL;
  }
}
