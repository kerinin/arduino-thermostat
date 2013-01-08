#include <Arduino.h>

#include "PID_v1.h"
#include "PID_AutoTune_v0.h"
#include "MenuBackend.h"
#include "Wire.h"
#include "OneWire.h"
#include "DallasTemperature.h"
#include "LiquidCrystal.h"
/*#include "EEPROMex.h"*/
#include "buttons.h"
#include "avr/eeprom.h"


#include "structs.h"

// Shared variables
double temperature = 0.0;
double power = 0.0;
bool tuning = false;     // Are we tuning?
configuration config;
profile profiles[4];

#include "actions.h"
#include "control.h"
#include "input.h"
#include "lcd.h"
#include "menu.h"


void setup() {
  Serial.begin(9600);
  Serial.println("*******************");
  Serial.flush();

  reset_settings();
  load();
  
  input_setup();
  Serial.println(F("-> Input"));
  Serial.flush();
  
  control_setup();
  Serial.println(F("-> Control"));
  Serial.flush();
  
  menu_setup();
  Serial.println(F("-> Menu"));
  Serial.flush();
  
  lcd_setup();
  Serial.println(F("-> LCD"));
  Serial.flush();
 
  /*char buf[20];*/
  /*Serial.println(profiles[config.driving].name);*/
  /*Serial.print("Kp: ");*/
  /*dtostrf(profiles[config.driving].kp,18,17,buf);*/
  /*Serial.println(buf);*/
  /*Serial.print("Ki: ");*/
  /*dtostrf(profiles[config.driving].ki,18,17,buf);*/
  /*Serial.println(buf);*/
  /*Serial.print("Kd: ");*/
  /*dtostrf(profiles[config.driving].kd,18,17,buf);*/
  /*Serial.println(buf);*/
}

void loop() {
  input_loop();
  control_loop();
  menu_loop();
  lcd_loop();
}

void(* reset_func) (void) = 0;
