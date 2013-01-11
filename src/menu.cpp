#include "menu.h"

MenuBackend menu = MenuBackend(use_handler, change_handler);
MenuItem mi_target = MenuItem(menu, "Target Temp.");
MenuItem mi_hardware = MenuItem(menu, "Hardware Prof.");
MenuItem mi_sample_time = MenuItem(menu, "Update Interval");
MenuItem mi_noise = MenuItem(menu, "Tuning Range");
MenuItem mi_lookback = MenuItem(menu, "Tuning Lookback");
MenuItem mi_tune = MenuItem(menu, "Autotune");
MenuItem mi_reset = MenuItem(menu, "Settings");
  MenuItem mi_reset_confirm = MenuItem(menu, "Are you SURE?");

Button dummy(MemoryTimer); // The first button instance behaves strangely for some reason
Button up(MemoryTimer);
Button down(MemoryTimer);
Button escape(MemoryTimer);
Button confirm(MemoryTimer);
bool save_later = false;

void menu_setup(){
  menu.getRoot()
    .add(mi_target)
    .addAfter(mi_hardware)
    .addAfter(mi_sample_time)
    .addAfter(mi_noise)
    .addAfter(mi_lookback)
    .addAfter(mi_tune)
    .addAfter(mi_reset)
    .addAfter(mi_target);

  mi_reset.addRight(mi_reset_confirm);
  
  mi_target.onLeft(decrement_target).onRight(increment_target);
  mi_hardware.onLeft(decrement_hardware).onRight(increment_hardware);
  mi_sample_time.onLeft(decrement_sample_time).onRight(increment_sample_time);
  mi_noise.onLeft(decrement_noise).onRight(increment_noise);
  mi_lookback.onLeft(decrement_lookback).onRight(increment_lookback);

  mi_tune.onRight(on_set_tune); // NOTE: This could use stateful display...
  mi_reset_confirm.onRight(on_set_reset);
  
  pinMode(up_pin, INPUT);
  up.assign(up_pin);
  pinMode(down_pin, INPUT);
  down.assign(down_pin);
  pinMode(left_pin, INPUT);
  escape.assign(left_pin);
  pinMode(right_pin, INPUT);
  confirm.assign(right_pin);
  confirm.setRefresh(20);
  
  menu.select(mi_target);
}

void menu_loop(){
  int up_check = up.check();
  int down_check = down.check();
  int escape_check = escape.check();
  int confirm_check = confirm.check();

  if(up_check == Released || up_check == Hold) { 
    Serial.println(F("up"));
    if(millis() - last_button_press <= screensaver_delay) { 
      menu.moveUp();
    } else { 
      menu_display(menu.getCurrent());
    }
    last_button_press = millis();
  }
  if(down_check == Released || down_check == Hold) {
    Serial.println(F("down"));
    if(millis() - last_button_press <= screensaver_delay) { 
      menu.moveDown();
    } else { 
      menu_display(menu.getCurrent());
    }
    last_button_press = millis();
  }
  if(escape_check == Released || escape_check == Hold) {
    Serial.println(F("left"));
    if(millis() - last_button_press <= screensaver_delay) { 
      menu.moveLeft();
    } else { 
      menu_display(menu.getCurrent());
    }
    last_button_press = millis();
  }
  if(confirm_check == Released || confirm_check == Hold) {
    Serial.println(F("right"));
    if(millis() - last_button_press <= screensaver_delay) { 
      menu.moveRight();
    } else { 
      menu_display(menu.getCurrent());
    }
    last_button_press = millis();
  }

  if(save_later && millis() - last_button_press > screensaver_delay) { 
    save_later = false;
    save();
  }
}

void use_handler(MenuUseEvent e) {
  menu_display(e.item);
}

void change_handler(MenuChangeEvent changed) {
  menu_display(changed.to);
}

void load(){
  Serial.println(F("Loading"));
  eeprom_read_block((void*)&config, (void*)0, sizeof(config));
  int i;
  for (i=0; i<(sizeof profiles / sizeof profiles[0]); i++) {
    eeprom_read_block((void*)&profiles[i], (void*)(256 + (i * sizeof(profiles[0]))), sizeof(profiles[0]));
  }
}

void save(){
  Serial.println(F("Saving"));
  eeprom_write_block((const void*)&config, (void*)0, sizeof(config));
  int i;
  for (i=0; i<(sizeof profiles / sizeof profiles[0]); i++) {
   eeprom_write_block((const void*)&profiles[i], (void*)(256 + (i * sizeof(profiles[0]))), sizeof(profiles[0]));
  }
}
