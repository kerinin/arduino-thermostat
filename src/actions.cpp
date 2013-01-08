#include "actions.h"

template <class T>
void increment_within_bounds(T *ref, int increment, int min_val, int max_val) {
  *ref += (T)increment;
  if ( *ref > max_val ) { *ref = min_val; }
  if ( *ref < min_val ) { *ref = max_val; }
}

void increment_target(MenuMoveEvent menu) {
  Serial.print(F("increment target "));
  increment_within_bounds( &profiles[config.driving].target_temp, 1, 32, 260 );
  save_later = true;
  menu_display(menu.item);
  Serial.println(profiles[config.driving].target_temp);
}
void decrement_target(MenuMoveEvent menu) {
  Serial.print(F("decrement target "));
  increment_within_bounds( &profiles[config.driving].target_temp, -1, 32, 260 );
  save_later = true;
  menu_display(menu.item);
  Serial.println(profiles[config.driving].target_temp);
}


void increment_hardware(MenuMoveEvent menu) {
  Serial.print(F("increment hardware "));
  increment_within_bounds( &config.driving, 1, 0, (sizeof profiles / sizeof profiles[0]) - 1 );
  save_later = true;
  menu_display(menu.item);
  Serial.println(config.driving);
}
void decrement_hardware(MenuMoveEvent menu) {
  Serial.print(F("decrement hardware "));
  increment_within_bounds( &config.driving, -1, 0, (sizeof profiles / sizeof profiles[0]) - 1 );
  save_later = true;
  menu_display(menu.item);
  Serial.println(config.driving);
}


void increment_sample_time(MenuMoveEvent menu) {
  Serial.print(F("increment sample time "));
  increment_within_bounds( &profiles[config.driving].sample_time, 1, 1, 600);
  save_later = true;
  menu_display(menu.item);
  Serial.println(profiles[config.driving].sample_time);
}
void decrement_sample_time(MenuMoveEvent menu) {
  Serial.print(F("decrement sample time "));
  increment_within_bounds( &profiles[config.driving].sample_time, -1, 1, 600);
  save_later = true;
  menu_display(menu.item);
  Serial.println(profiles[config.driving].sample_time);
}


void increment_noise(MenuMoveEvent menu) {
  Serial.print(F("increment noise "));
  increment_within_bounds( &config.noise_band, 1, 0, 50 );
  save_later = true;
  menu_display(menu.item);
  Serial.println(config.noise_band);
}
void decrement_noise(MenuMoveEvent menu) {
  Serial.print(F("decrement noise "));
  increment_within_bounds( &config.noise_band, -1, 0, 50 );
  save_later = true;
  menu_display(menu.item);
  Serial.println(config.noise_band);
}


void increment_lookback(MenuMoveEvent menu) {
  Serial.print(F("increment lookback "));
  increment_within_bounds( &config.lookback_min, 1, 0, 30 );
  save_later = true;
  menu_display(menu.item);
  Serial.println(config.lookback_min);
}
void decrement_lookback(MenuMoveEvent menu) {
  Serial.print(F("decrement lookback "));
  increment_within_bounds( &config.lookback_min, -1, 0, 30 );
  save_later = true;
  menu_display(menu.item);
  Serial.println(config.lookback_min);
}

void on_set_tune(MenuMoveEvent menu){
  Serial.println(F("Set tune"));
  start_autotune();
  menu_display(menu.item);
}

void on_set_reset(MenuMoveEvent menu){
  Serial.println(F("Set reset"));
  reset_settings();
  save();
  menu_display(menu.item);
}

void reset_settings(void) {
  config.paused = false;
  config.driving = 0;
  config.noise_band = 2;
  config.lookback_min = 1;
  
  strcpy(profiles[0].name, "Default");
  profiles[0].kp = 4;
  profiles[0].ki = 0.01;
  profiles[0].kd = 0.0;
  profiles[0].sample_time = 1;
  profiles[0].target_temp = 100;
  
  strcpy(profiles[1].name, "Fermenter");
  profiles[1].kp = 4;
  profiles[1].ki = 0.01;
  profiles[1].kd = 0.0;
  profiles[1].sample_time = 1;
  profiles[1].target_temp = 74;
  
  strcpy(profiles[2].name, "Sous Vide");
  profiles[2].kp = 4;
  profiles[2].ki = 0.01;
  profiles[2].kd = 0.0;
  profiles[2].sample_time = 1;
  profiles[2].target_temp = 130;
  
  strcpy(profiles[3].name, "Smoker");
  profiles[3].kp = 4;
  profiles[3].ki = 0.01;
  profiles[3].kd = 0;
  profiles[3].sample_time = 1;
  profiles[3].target_temp = 220;
}


