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

boolean paused = false;     // PID doesn't update control variable if TRUE
int driving = 0;        // The hardware being controlled
float targetTemp = 32.0;   // The temperature we want to hit (user-specified)
double temperature = 0.0;
double power = 0.0;
boolean tuning = false;     // Are we tuning?
float noiseBand;    // Autotune noise band
float lookbackMin;  // Autotune lookback
// Control
struct profile {
  double kp, ki, kd;
  int sampleTime;
};
profile fermenterProfile = {2, 0.5, 2, 1000};
profile sousVideProfile = {2, 0.4, 2, 1000};
profile smokerProfile = {2, 0.5, 2, 1000};
profile otherProfile = {2, 0.5, 2, 1000};
profile* profiles[] = {&fermenterProfile, &sousVideProfile, &smokerProfile, &otherProfile};

void(* resetFunc) (void) = 0;

void setup() {
  Serial.begin(9600);
  Serial.println("Starting");
  Serial.flush();
  
  input_setup();
  Serial.println("Input Setup");
  Serial.flush();
  
  //control_setup();
  Serial.println("Control Setup");
  //Serial.flush();
  
  ui_setup();
  Serial.println("UI Setup");
  Serial.flush();
}

void loop() {
  input_loop();
  //control_loop();
  ui_loop();
}



