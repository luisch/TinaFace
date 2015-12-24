#include <pebble.h>
#include "default.h"
#include "location.h"
#include "tablet.h"

enum location_layers {
  CITY,
  LAYER_NUMS
};

static TextLayer* wl[LAYER_NUMS];

void location_update(struct tm* t){
  text_layer_set_text(wl[CITY], tabletGetCurrentCityName() );
}

void location_main(Window* window){
  // 都市名
  wl[CITY] = text_layer_create(GRect(0,168-14,144,14));
	text_layer_set_background_color(wl[CITY],GColorClear);
	text_layer_set_text_color(wl[CITY],TINA_FORGROUND_COLOR);
	text_layer_set_text_alignment(wl[CITY], GTextAlignmentLeft); 
	text_layer_set_font(wl[CITY], fonts_get_system_font(FONT_KEY_GOTHIC_14));
	text_layer_set_text(wl[CITY],"...");
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(wl[CITY]));
}

void location_exit(Window* window){
  for(int n=0; n<LAYER_NUMS; ++n){
    if(wl[n]){
      text_layer_destroy(wl[n]);
    }
    wl[n] = NULL;
  }
}
