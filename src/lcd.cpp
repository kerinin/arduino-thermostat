#include "lcd.h"

char lcd_buf1[17];
char lcd_buf2[17];
int screensaver_delay = 4000;
double last_button_press = -4000;
LiquidCrystal lcd(lcd_rs_pin, lcd_enable_pin, lcd_d4_pin, lcd_d5_pin, lcd_d6_pin, lcd_d7_pin);

void lcd_setup(){
  lcd.begin( 16, 2);

  default_display();  
}

void lcd_loop(){
  if(millis() - last_button_press > screensaver_delay) { default_display(); }
}

void default_display() {
  char temp_string[5];
  char new_lcd1[17];
  char new_lcd2[17];
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

void menu_display(MenuItem current) {
  char new_lcd1[17];
  char new_lcd2[17];
  strcpy(new_lcd1, current.getName());
  
  if(mi_target.isEqual(current)) {
    sprintf(new_lcd2, "%i\337F", (int)config.target_temp);
  } else if(mi_hardware.isEqual(current)) {
    sprintf(new_lcd2, profiles[config.driving].name);
  } else if(mi_sample_time.isEqual(current)) {
    sprintf(new_lcd2, "%i s", profiles[config.driving].sample_time);
  } else if(mi_noise.isEqual(current)) {
    sprintf(new_lcd2, "%i +/- %i\337F", (int)config.target_temp, (int)config.noise_band);
  } else if(mi_lookback.isEqual(current)) {
    sprintf(new_lcd2, "%i minutes", (int)config.lookback_min);
  } else if(mi_tune.isEqual(current)) {
    sprintf(new_lcd2, "--> Start");
  } else if(mi_reset.isEqual(current)) {
    sprintf(new_lcd2, "--> Reset");
  } else if(mi_reset_confirm.isEqual(current)) {
    sprintf(new_lcd2, "--> DESTROY!!!");
  } else {
    strcpy(new_lcd2, " ");
  }

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
