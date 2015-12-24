#pragma once

typedef enum _update_status {
  state_init,
  state_updated,
  state_updating,
} update_status;

void tablet_main(Window* window);
void tablet_exit(Window* window);
void tablet_update(struct tm *tick_time);

const char* tabletGetCurrentTemperature();
const char* tabletGetCurrentCondition();
const char* tabletGetCurrentCityName();

update_status getUpdateStatus();
