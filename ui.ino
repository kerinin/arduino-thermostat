#define lcd_rs_pin 12
#define lcd_enable_pin 13
#define lcd_d4_pin 4
#define lcd_d5_pin 5
#define lcd_d6_pin 6
#define lcd_d7_pin 7

#define escape_pin A4
#define confirm_pin A2
#define up_pin A0
#define down_pin A3

// UI
LiquidCrystal lcd(lcd_rs_pin, lcd_enable_pin, lcd_d4_pin, lcd_d5_pin, lcd_d6_pin, lcd_d7_pin);
MenuSystem ms;
Menu mm("Main Menu");
MenuItem mi_target("Target Temp.");
MenuItem mi_hardware("Hardware Prof.");
Menu m_tune("Auto Tune");
  MenuItem mi_noise("Noise Band (F)");
  MenuItem mi_lookback("Loockback (s)");
  MenuItem mi_tune("DOIT NAOW!!!");
MenuItem mi_reset("Reset Settings");
char lcd_buf1[16];
char lcd_buf2[16];
Button up(MemoryTimer);
Button down(MemoryTimer);
Button escape(MemoryTimer);
Button confirm(MemoryTimer);
double last_button_press = -4000;
int screensaver_delay = 4000;
boolean save_later = false;

void ui_setup(){
  lcd.begin( 16, 2);
  ms.set_root_menu(&mm);
  mm.add_item(&mi_target, &on_set_target);
  mm.add_item(&mi_hardware, &on_set_hardware);
  mm.add_menu(&m_tune);
    m_tune.add_item(&mi_noise, &on_set_noise);
    m_tune.add_item(&mi_lookback, &on_set_loockback);
    m_tune.add_item(&mi_tune, &on_set_tune);
  mm.add_item(&mi_reset, &on_set_reset);
  
  pinMode(up_pin, INPUT);
  up.assign(up_pin);
  pinMode(down_pin, INPUT);
  down.assign(down_pin);
  pinMode(escape_pin, INPUT);
  escape.assign(escape_pin);
  pinMode(confirm_pin, INPUT);
  confirm.assign(confirm_pin);
  confirm.setRefresh(20);
  
  update_lcd();
}

void ui_loop(){
  if(up.check() == Released || up.check() == Hold) { on_up_pin(); }
  if(down.check() == Released || down.check() == Hold) { on_down_pin(); }
  if(escape.check() == Released || escape.check() == Hold) { on_escape_pin(); }
  if(confirm.check() == Released || confirm.check() == Hold) { on_confirm_pin(); }
  
  if(millis() - last_button_press > screensaver_delay) { default_display(); }
  if(save_later && millis() - last_button_press > 4000) { 
    save_later = false;
    save();
  }
}

void default_display() {
  char temp_string[5];
  char new_lcd1[16];
  char new_lcd2[16];
  dtostrf(temperature,4,1,temp_string);
  sprintf(new_lcd1, "%s\337 >>--> %i\337", temp_string, (int)config.target_temp);
  sprintf(new_lcd2, "power %i%%", (int)(100.0 * ((float)power / 255.0)));
  flush_lcd(new_lcd1, new_lcd2);
}

void on_up_pin(){
  Serial.println(F("up"));
  ms.prev();
  last_button_press = millis();
  update_lcd();
}

void on_down_pin(){
  Serial.println(F("down"));
  ms.next();
  last_button_press = millis();
  update_lcd();
}

void on_escape_pin(){
  Serial.println(F("escape"));
  ms.back();
  last_button_press = millis();
  update_lcd();
}

void on_confirm_pin(){
  Serial.println(F("confirm"));
  ms.select();
  last_button_press = millis();
  update_lcd();
}

void on_set_target(MenuItem* mi){
  Serial.println(F("set target"));
  config.target_temp += 1.0;
  if(config.target_temp > 240.0) { config.target_temp = 32.0; }
  save_later = true;
}

void on_set_hardware(MenuItem* mi){
  Serial.println(F("Set hardware"));
  config.driving += 1;
  if(config.driving >= (sizeof profiles / sizeof profiles[0])) { config.driving = 0; }
  save_later = true;
}

void on_set_noise(MenuItem* mi){
  Serial.println(F("Set noise"));
}

void on_set_loockback(MenuItem* mi){
  Serial.println(F("Set loockback"));
}

void on_set_tune(MenuItem* mi){
  Serial.println(F("Set tune"));
}

void on_set_reset(MenuItem* mi){
  Serial.println(F("Set reset"));
  config.paused = false;
  config.driving = 0;
  config.target_temp = 140;
  config.tuning = false;
  config.noise_band = 20;
  config.lookback_min = 5;
  
  strcpy(profiles[0].name, "Default");
  profiles[0].kp = 2;
  profiles[0].ki = 0.5;
  profiles[0].kd = 2;
  profiles[0].sample_time = 1000;
  
  strcpy(profiles[1].name, "Fermenter");
  profiles[1].kp = 2;
  profiles[1].ki = 0.5;
  profiles[1].kd = 2;
  profiles[1].sample_time = 1000;
  
  strcpy(profiles[2].name, "Sous Vide");
  profiles[2].kp = 2;
  profiles[2].ki = 0.5;
  profiles[2].kd = 2;
  profiles[2].sample_time = 1000;
  
  strcpy(profiles[3].name, "Smoker");
  profiles[3].kp = 2;
  profiles[3].ki = 0.5;
  profiles[3].kd = 2;
  profiles[3].sample_time = 1000;
  
  save();
}

void update_lcd(){
  char new_lcd1[16];
  char new_lcd2[16];
  strcpy(new_lcd1, ms.get_current_menu()->get_selected()->get_name());
  
  MenuComponent const* selected = ms.get_current_menu()->get_selected();
  if(selected == &mi_target) {
    sprintf(new_lcd2, "%i\337", (int)config.target_temp);
  } else if(selected == &mi_hardware) {
    sprintf(new_lcd2, profiles[config.driving].name);
  } else if(selected == &m_tune) {
    sprintf(new_lcd2, "->");
  } else if(selected == &mi_noise) {
    sprintf(new_lcd2, "%i\337", (int)config.noise_band);
  } else if(selected == &mi_lookback) {
    sprintf(new_lcd2, "%i minutes", config.lookback_min);
  } else {
    strcpy(new_lcd2, "");
  } /*else if(selected == &mi_tune) {
    sprintf(new_lcd2, "Start");
  }*/
  
  
  flush_lcd(new_lcd1, new_lcd2);
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
