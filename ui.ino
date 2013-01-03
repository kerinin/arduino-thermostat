#define lcdRsPin 12
#define lcdEnablePin 13
#define lcdD4Pin 4
#define lcdD5Pin 5
#define lcdD6Pin 6
#define lcdD7Pin 7

#define escapePin A4
#define confirmPin A2
#define upPin A0
#define downPin A3

// UI
LiquidCrystal lcd(lcdRsPin, lcdEnablePin, lcdD4Pin, lcdD5Pin, lcdD6Pin, lcdD7Pin);
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
int lastButtonPress = -4000;
int screensaverDelay = 4000;
boolean saveLater = false;

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
  
  pinMode(upPin, INPUT);
  up.assign(upPin);
  pinMode(downPin, INPUT);
  down.assign(downPin);
  pinMode(escapePin, INPUT);
  escape.assign(escapePin);
  pinMode(confirmPin, INPUT);
  confirm.assign(confirmPin);
  confirm.setRefresh(20);
  
  update_lcd();
}

void ui_loop(){
  if(up.check() == Released || up.check() == Hold) { on_up_pin(); }
  if(down.check() == Released || down.check() == Hold) { on_down_pin(); }
  if(escape.check() == Released || escape.check() == Hold) { on_escape_pin(); }
  if(confirm.check() == Released || confirm.check() == Hold) { on_confirm_pin(); }
  
  if(millis() - lastButtonPress > screensaverDelay) { defaultDisplay(); }
  if(saveLater && millis() - lastButtonPress > 4000) { 
    saveLater = false;
    save();
  }
}

void defaultDisplay() {
  char temp_string[5];
  char new_lcd1[16];
  char new_lcd2[16];
  dtostrf(temperature,4,1,temp_string);
  sprintf(new_lcd1, "%s\337 >>--> %i\337", temp_string, (int)config.targetTemp);
  sprintf(new_lcd2, "power %i%%", (int)(100.0 * ((float)power / 255.0)));
  flush_lcd(new_lcd1, new_lcd2);
}

void on_up_pin(){
  Serial.println(F("up"));
  ms.prev();
  lastButtonPress = millis();
  update_lcd();
}

void on_down_pin(){
  Serial.println(F("down"));
  ms.next();
  lastButtonPress = millis();
  update_lcd();
}

void on_escape_pin(){
  Serial.println(F("escape"));
  ms.back();
  lastButtonPress = millis();
  update_lcd();
}

void on_confirm_pin(){
  Serial.println(F("confirm"));
  ms.select();
  lastButtonPress = millis();
  update_lcd();
}

void on_set_target(MenuItem* mi){
  Serial.println(F("set target"));
  config.targetTemp += 1.0;
  if(config.targetTemp > 240.0) { config.targetTemp = 32.0; }
  saveLater = true;
}

void on_set_hardware(MenuItem* mi){
  Serial.println(F("Set hardware"));
  config.driving += 1;
  if(config.driving >= (sizeof profiles / sizeof profiles[0])) { config.driving = 0; }
  saveLater = true;
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
  config.targetTemp = 140;
  config.tuning = false;
  config.noiseBand = 20;
  config.lookbackMin = 5;
  
  strcpy(profiles[0].name, "Default");
  profiles[0].kp = 2;
  profiles[0].ki = 0.5;
  profiles[0].kd = 2;
  profiles[0].sampleTime = 1000;
  
  strcpy(profiles[1].name, "Fermenter");
  profiles[1].kp = 2;
  profiles[1].ki = 0.5;
  profiles[1].kd = 2;
  profiles[1].sampleTime = 1000;
  
  strcpy(profiles[2].name, "Sous Vide");
  profiles[2].kp = 2;
  profiles[2].ki = 0.5;
  profiles[2].kd = 2;
  profiles[2].sampleTime = 1000;
  
  strcpy(profiles[3].name, "Smoker");
  profiles[3].kp = 2;
  profiles[3].ki = 0.5;
  profiles[3].kd = 2;
  profiles[3].sampleTime = 1000;
  
  save();
}

void update_lcd(){
  char new_lcd1[16];
  char new_lcd2[16];
  strcpy(new_lcd1, ms.get_current_menu()->get_selected()->get_name());
  
  MenuComponent const* selected = ms.get_current_menu()->get_selected();
  if(selected == &mi_target) {
    sprintf(new_lcd2, "%i\337", (int)config.targetTemp);
  } else if(selected == &mi_hardware) {
    sprintf(new_lcd2, profiles[config.driving].name);
  } else if(selected == &m_tune) {
    sprintf(new_lcd2, "->");
  } else if(selected == &mi_noise) {
    sprintf(new_lcd2, "%i\337", (int)config.noiseBand);
  } else if(selected == &mi_lookback) {
    sprintf(new_lcd2, "%i minutes", config.lookbackMin);
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
