#define lcd_rs_pin 13
#define lcd_enable_pin 12
#define lcd_d4_pin 5
#define lcd_d5_pin 4
#define lcd_d6_pin 3
#define lcd_d7_pin 2

#define escape_pin 11
#define confirm_pin 8
#define up_pin 9
#define down_pin 10

// UI
LiquidCrystal lcd(lcd_rs_pin, lcd_enable_pin, lcd_d4_pin, lcd_d5_pin, lcd_d6_pin, lcd_d7_pin);

/*MenuSystem ms;*/
/*Menu mm("Main Menu");*/
MenuBackend menu = MenuBackend(menuUseEvent, menuChangeEvent);
/*MenuItem mi_target("Target Temp.");*/
MenuItem mi_target = MenuItem(menu, "Target Temp.", 1);
/*MenuItem mi_hardware("Hardware Prof.");*/
MenuItem mi_hardware = MenuItem(menu, "Hardware Prof.", 1);
/*MenuItem mi_noise("Tuning Range");*/
MenuItem mi_noise = MenuItem(menu, "Tuning Range", 1);
/*MenuItem mi_lookback("Tuning Lookback");*/
MenuItem mi_lookback = MenuItem(menu, "Tuning Lookback", 1);
/*MenuItem mi_tune("Autotune");*/
MenuItem mi_tune = MenuItem(menu, "Autotune", 1);
/*MenuItem mi_reset("Settings");*/
MenuItem mi_reset = MenuItem(menu, "Settings", 1);

void menuUseEvent(MenuUseEvent used) {}

void menuChangeEvent(MenuChangeEvent changed) {
  char new_lcd1[16];
  char new_lcd2[16];
  strcpy(new_lcd1, changed.to.getName());

  /*
  if(changed.from == mi_target) {
    sprintf(new_lcd2, "%i\337F", (int)config.target_temp);
  } else if(changed.from == mi_hardware) {
    sprintf(new_lcd2, profiles[config.driving].name);
  } else if(changed.from == mi_noise) {
    sprintf(new_lcd2, "%i +/- %i\337F", (int)config.target_temp, (int)config.noise_band);
  } else if(changed.from == mi_lookback) {
    sprintf(new_lcd2, "%i minutes", (int)config.lookback_min);
  } else if(changed.from == mi_tune) {
    sprintf(new_lcd2, "> click to start");
  } else if(changed.tf == mi_reset) {
    sprintf(new_lcd2, "> click to reset");
  } else {
    strcpy(new_lcd2, "");
  }
  */
  flush_lcd(new_lcd1, new_lcd2);
}

char lcd_buf1[16];
char lcd_buf2[16];

Button dummy(MemoryTimer); // The first button instance behaves strangely for some reason
Button up(MemoryTimer);
Button down(MemoryTimer);
Button escape(MemoryTimer);
Button confirm(MemoryTimer);
double last_button_press = -4000;
int screensaver_delay = 4000;
boolean save_later = false;

/*
template <class T>
void increment_within_bounds(T *ref, int increment, int min_val, int max_val) {
  //*ref += (Comparable)increment;
  //if ( ref > max_val ) { ref = max_val; }
  //else if ( ref < min_val ) { ref = min_val; }
}
*/

void increment_target(MenuMoveEvent menu) {
  Serial.println(F("increment target"));
  //increment_within_bounds( config.driving, -1, 32, 260 );
  save_later = true;
}
void decrement_target(MenuMoveEvent menu) {
  Serial.println(F("increment target"));
  //increment_within_bounds( config.driving, 1, 32, 260 );
  save_later = true;
}


void increment_hardware(MenuMoveEvent menu) {
  Serial.println(F("Set hardware"));
  //increment_within_bounds( config.driving, 1, 0, (sizeof profiles / sizeof profiles[0]) );
  save_later = true;
}
void decrement_hardware(MenuMoveEvent menu) {
  Serial.println(F("Set hardware"));
  //increment_within_bounds( config.driving, -1, 0, (sizeof profiles / sizeof profiles[0]) );
  save_later = true;
}


void increment_noise(MenuMoveEvent menu) {
  Serial.println(F("Set noise"));
  //increment_within_bounds( config.noise_band, 1, 0, 50 );
}
void decrement_noise(MenuMoveEvent menu) {
  Serial.println(F("Set noise"));
  //increment_within_bounds( config.noise_band, -1, 0, 50 );
}


void increment_lookback(MenuMoveEvent menu) {
  Serial.println(F("Set lookback"));
  //increment_within_bounds( config.lookback_min, 1, 0, 30 );
}
void decrement_lookback(MenuMoveEvent menu) {
  Serial.println(F("Set lookback"));
  //increment_within_bounds( config.lookback_min, -1, 0, 30 );
}

void ui_setup(){
  lcd.begin( 16, 2);
  
  /*mm.add_item(&mi_target, &on_set_target);*/
  /*mm.add_item(&mi_hardware, &on_set_hardware);*/
  /*mm.add_item(&mi_noise, &on_set_noise);*/
  /*mm.add_item(&mi_lookback, &on_set_lookback);*/
  /*mm.add_item(&mi_tune, &on_set_tune);*/
  /*mm.add_item(&mi_reset, &on_set_reset);*/
  /*ms.set_root_menu(&mm);*/
  menu.getRoot()
    .add(mi_target)
    .addAfter(mi_hardware)
    .addAfter(mi_noise)
    .addAfter(mi_lookback)
    .addAfter(mi_tune)
    .addAfter(mi_reset)
    .addAfter(mi_target);
  
  mi_target.onLeft(decrement_target).onRight(increment_target);
  mi_hardware.onLeft(decrement_hardware).onRight(increment_hardware);
  mi_noise.onLeft(decrement_noise).onRight(increment_noise);
  mi_lookback.onLeft(decrement_lookback).onRight(increment_lookback);
  
  pinMode(up_pin, INPUT);
  up.assign(up_pin);
  pinMode(down_pin, INPUT);
  down.assign(down_pin);
  pinMode(escape_pin, INPUT);
  escape.assign(escape_pin);
  pinMode(confirm_pin, INPUT);
  confirm.assign(confirm_pin);
  confirm.setRefresh(20);
  
  //update_lcd();  
  //menu.moveDown();
}

void ui_loop(){
  int up_check = up.check();
  int down_check = down.check();
  int escape_check = escape.check();
  int confirm_check = confirm.check();

  if(up_check == Released || up_check == Hold) { 
    Serial.println(F("up"));
    menu.moveUp();
    last_button_press = millis();
  }
  if(down_check == Released || down_check == Hold) {
    Serial.println(F("down"));
    menu.moveDown();
    last_button_press = millis();
  }
  if(escape_check == Released || escape_check == Hold) {
    Serial.println(F("left"));
    menu.moveLeft();
    last_button_press = millis();
  }
  if(confirm_check == Released || confirm_check == Hold) {
    Serial.println(F("right"));
    menu.moveRight();
    last_button_press = millis();
  }
  
  /*
  if(millis() - last_button_press > screensaver_delay) { default_display(); }
  if(save_later && millis() - last_button_press > 4000) { 
    save_later = false;
    save();
  }
  */
}

void default_display() {
  char temp_string[5];
  char new_lcd1[16];
  char new_lcd2[16];
  dtostrf(temperature,4,1,temp_string);
  if(tuning) {
    sprintf(new_lcd1, "%s\337 tune:%3i\337", temp_string, (int)config.target_temp);
  } else {
    sprintf(new_lcd1, "%s\337 >>>> %i\337", temp_string, (int)config.target_temp);
  }    
  sprintf(new_lcd2, "power: %i", (int)power);
  
  strcpy(new_lcd2, "[");
  for(int i=0;i<14;i++){
    if( (int)(14 * ((float)power / 255.0)) > i ) {
      strcat(new_lcd2, "*");
    } else {
      strcat(new_lcd2, " ");
    }
  }
  strcat(new_lcd2, "]");

  flush_lcd(new_lcd1, new_lcd2);
}

void on_set_tune(MenuItem* mi){
  Serial.println(F("Set tune"));
  start_autotune();
}

void on_set_reset(MenuItem* mi){
  Serial.println(F("Set reset"));
  config.paused = false;
  config.driving = 0;
  config.target_temp = 140;
  config.noise_band = 2;
  config.lookback_min = 1;
  
  strcpy(profiles[0].name, "Default");
  profiles[0].kp = 4;
  profiles[0].ki = 0.01;
  profiles[0].kd = 0.0;
  profiles[0].sample_time = 1000;
  
  strcpy(profiles[1].name, "Fermenter");
  profiles[1].kp = 4;
  profiles[1].ki = 0.01;
  profiles[1].kd = 0.0;
  profiles[1].sample_time = 1000;
  
  strcpy(profiles[2].name, "Sous Vide");
  profiles[2].kp = 4;
  profiles[2].ki = 0.01;
  profiles[2].kd = 0.0;
  profiles[2].sample_time = 1000;
  
  strcpy(profiles[3].name, "Smoker");
  profiles[3].kp = 4;
  profiles[3].ki = 0.01;
  profiles[3].kd = 0;
  profiles[3].sample_time = 1000;
  
  save();
}

void flush_lcd(char *lcd1, char *lcd2){
  if(strcmp(lcd1, lcd_buf1) != 0 || strcmp(lcd2, lcd_buf2) != 0) {
    lcd.clear();
    
    strcpy(lcd_buf1, lcd1);
    lcd.setCursor(0,0);
    lcd.print(lcd1);
  
    strcpy(lcd_buf2, lcd2);
    lcd.setCursor(0,1);
    lcd.print(lcd2);
  }
}
