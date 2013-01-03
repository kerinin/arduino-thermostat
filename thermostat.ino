#include <Arduino.h>

#include <EEPROMex.h>
#include <buttons.h>
#include <LCD.h>
#include <LiquidCrystal.h>
#include <MenuSystem.h>

#include <PID_v1.h>
#include <PID_AutoTune_v0.h>

#include <Wire.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#define drivingFermenter 0
#define drivingSousVide 1
#define drivingSmoker 2

double temperature = 0.0;
double power = 0.0;
struct configuration {
  boolean paused;     // PID doesn't update control variable if TRUE
  unsigned short int driving;        // The hardware being controlled
  double targetTemp;  // The temperature we want to hit (user-specified)
  boolean tuning;     // Are we tuning?
  float noiseBand;    // Autotune noise band
  float lookbackMin;  // Autotune lookback
} config;

// Control
struct profile {
  char name[16];
  double kp, ki, kd;
  int sampleTime;
};
profile profiles[4];

void(* resetFunc) (void) = 0;

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
  
  load();
  
  input_setup();
  Serial.println(F("-> Input"));
  Serial.flush();
  
  control_setup();
  Serial.println(F("-> Control"));
  Serial.flush();
  
  ui_setup();
  Serial.println(F("-> UI"));
  Serial.flush();
}

void loop() {
  input_loop();
  control_loop();
  ui_loop();
}



