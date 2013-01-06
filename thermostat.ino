#include <Arduino.h>

#include <EEPROMex.h>
#include <buttons.h>
//#include <LCD.h>
#include <LiquidCrystal.h>
//#include <MenuSystem.h>
#include <MenuBackend.h>

#include <PID_v1.h>
#include <PID_AutoTune_v0.h>

#include <Wire.h>
#include <OneWire.h>
#include <DallasTemperature.h>

double temperature = 0.0;
double power = 0.0;
boolean tuning = false;     // Are we tuning?
struct configuration {
  boolean paused;     // PID doesn't update control variable if TRUE
  unsigned short int driving;        // The hardware being controlled
  double target_temp;  // The temperature we want to hit (user-specified)
  float noise_band;    // Autotune noise band
  float lookback_min;  // Autotune lookback
} config;

// Control
struct profile {
  char name[16];
  double kp, ki, kd;
  int sample_time;
};
profile profiles[4];

void(* reset_func) (void) = 0;

void load(){
  Serial.println(F("Loading"));
  EEPROM.readBlock(256, config);
  EEPROM.readBlock(0, profiles, 4);
}

void save(){
  Serial.println(F("Saving"));
  Serial.println(EEPROM.updateBlock(256, config));
  Serial.println(EEPROM.updateBlock(0, profiles, 4));
}

void setup() {
  Serial.begin(9600);
  Serial.println("*******************");
  Serial.flush();
  /*
  load();
  
  input_setup();
  Serial.println(F("-> Input"));
  Serial.flush();
  
  control_setup();
  Serial.println(F("-> Control"));
  Serial.flush();
  */
  ui_setup();
  Serial.println(F("-> UI"));
  Serial.flush();
  
  /*
  char buf[20];
  Serial.println(profiles[config.driving].name);
  Serial.print("Kp: ");
  dtostrf(profiles[config.driving].kp,18,17,buf);
  Serial.println(buf);
  Serial.print("Ki: ");
  dtostrf(profiles[config.driving].ki,18,17,buf);
  Serial.println(buf);
  Serial.print("Kd: ");
  dtostrf(profiles[config.driving].kd,18,17,buf);
  Serial.println(buf);
  */
}

void loop() {
  //input_loop();
  //control_loop();
  ui_loop();
}



