#include <pebble.h>
#include "tablet.h"

static update_status update_state;

// Store incoming information
static char temperature_buffer[8];
static char conditions_buffer[32] = "\ue010";
static char city_name[64];
static char wind_speed[15];
static char weather_forecast[8];

const char* tabletGetCurrentTemperature() {return temperature_buffer;};
const char* tabletGetCurrentCondition()      {return conditions_buffer;}
const char* tabletGetCurrentCityName()     {return city_name;}
const char* tabletGetCurrentWind()            {return wind_speed;}
const char* tabletGetForecast()      {return weather_forecast;}

enum {
  KEY_TEMPERATURE = 0,
  KEY_CONDITIONS,
  KEY_CITY,
  KEY_WINDSPEED,
  KEY_FORECAST,
};

bool tabletPostMessage(const char* key){
  static DictionaryIterator *iter;
  app_message_outbox_begin(&iter);
  dict_write_cstring(iter, 0, key);
  return APP_MSG_OK == app_message_outbox_send();
}

update_status getUpdateStatus(){
  return update_state;
}

void tablet_update(struct tm* t){
}

static void inbox_received_callback(DictionaryIterator *iterator, void *context) {
  // Read tuples for data
  Tuple* t = NULL;
  if( NULL != (t = dict_find(iterator, KEY_TEMPERATURE)) ){
    APP_LOG(APP_LOG_LEVEL_INFO, "received a temperature");
    snprintf(temperature_buffer, sizeof(temperature_buffer), "%s°", t ? t->value->cstring : "99.9");
    persist_write_string(KEY_TEMPERATURE,temperature_buffer);
  }
  if( NULL !=(t = dict_find(iterator, KEY_CONDITIONS))){
    APP_LOG(APP_LOG_LEVEL_INFO, "received a conditions");
    snprintf(conditions_buffer, sizeof(conditions_buffer), "%s", t ? t->value->cstring : "\ue010");
    persist_write_string(KEY_CONDITIONS,conditions_buffer);
    update_state = state_updated;
  }
  if( NULL !=(t = dict_find(iterator, KEY_CITY))){
    APP_LOG(APP_LOG_LEVEL_INFO, "received a city name");
    snprintf(city_name, sizeof(city_name), "@%s", t ? t->value->cstring : "@?");
    persist_write_string(KEY_CITY,city_name);
  }
  if( NULL !=(t = dict_find(iterator, KEY_WINDSPEED))){
    APP_LOG(APP_LOG_LEVEL_INFO, "received a wind speed");
    snprintf(wind_speed, sizeof(wind_speed), "%sm", t ? t->value->cstring : "?");
    persist_write_string(KEY_WINDSPEED,wind_speed);
  }
  if( NULL !=(t = dict_find(iterator, KEY_FORECAST))){
    APP_LOG(APP_LOG_LEVEL_INFO, "received weather forecast");
    snprintf(weather_forecast, sizeof(weather_forecast), "%s", t ? t->value->cstring : "");
    persist_write_string(KEY_FORECAST,weather_forecast);
  }
}

static void inbox_dropped_callback(AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped!");
  update_state = state_init;
}
static void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox send failed!");
  update_state = state_init;
}
static void outbox_sent_callback(DictionaryIterator *iterator, void *context) {
  APP_LOG(APP_LOG_LEVEL_INFO, "Outbox send success!");
}

void tablet_main(Window* window){
  app_message_register_inbox_received(inbox_received_callback);
  app_message_register_inbox_dropped(inbox_dropped_callback);
  app_message_register_outbox_failed(outbox_failed_callback);
  app_message_register_outbox_sent(outbox_sent_callback);
  app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
  
  persist_read_string(KEY_TEMPERATURE,temperature_buffer,sizeof(temperature_buffer));
  persist_read_string(KEY_CONDITIONS,conditions_buffer,sizeof(conditions_buffer));
  persist_read_string(KEY_CITY,city_name,sizeof(city_name));
  persist_read_string(KEY_WINDSPEED,wind_speed,sizeof(wind_speed));
  persist_read_string(KEY_FORECAST,weather_forecast,sizeof(weather_forecast));
}
void tablet_exit(Window* window){
}
