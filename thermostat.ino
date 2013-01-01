#include <Wire.h>
 
#include <buttons.h>
#include <LCD.h>
#include <LiquidCrystal.h>
#include <EEPROM.h>
#include <MENWIZ.h>
#include <EEPROMex.h>

#include <spline.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <PID_v1.h>
#include <PID_AutoTune_v0.h>


#define lcdRsPin 12
#define lcdEnablePin 13
#define lcdD4Pin 4
#define lcdD5Pin 5
#define lcdD6Pin 6
#define lcdD7Pin 7

#define oneWirePin 11

#define escapePin A4
#define confirmPin A2
#define upPin A1
#define downPin A3

#define drivingFermenter 0
#define drivingSousVide 1
#define drivingSmoker 2

// Input
byte insideThermometer[8];
OneWire oneWire(oneWirePin);
DallasTemperature tempSensor(&oneWire);
boolean waitingForTemp = false;

// UI
menwiz menu;
LiquidCrystal lcd(lcdRsPin, lcdEnablePin, lcdD4Pin, lcdD5Pin, lcdD6Pin, lcdD7Pin);
int operationMode;  // thermostat / autotune

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
boolean paused = false;     // PID doesn't update control variable if TRUE
int driving = 0;        // The hardware being controlled
float targetTemp = 0.0;   // The temperature we want to hit (user-specified)
double temperature = 0.0;
double power = 0.0;
boolean tuning = false;     // Are we tuning?
double setpoint = 0.0;     // The temperature the controller is trying to hit (may change if ramping)
float noiseBand;    // Autotune noise band
float lookbackMin;  // Autotune lookback


PID myPID(&temperature, &power, &setpoint, 0.0, 0.0, 0.0, DIRECT);
PID_ATune aTune(&temperature, &power);

void setup() {
  Serial.begin(9600);
  Serial.println("Starting");
  
  // Initialize sensors
  pinMode( oneWirePin, INPUT );
  tempSensor.begin();
  tempSensor.setResolution(insideThermometer, 12);
  tempSensor.setWaitForConversion(false);
  requestTemp();
  Serial.println("Sensors setup");
  
  // Initialize UI
  _menu *r,*s1,*s2;
  menu.begin(&lcd, 16, 2);
  r=menu.addMenu(MW_ROOT, NULL, F("Main Menu"));
    s1=menu.addMenu(MW_VAR, r, F("Target"));
      //s1->addVar(MW_AUTO_FLOAT, &targetTemp, 32.0, 240.0, 0.5);
      //s1->addVar(MW_BOOLEAN,&tuning);
    s1=menu.addMenu(MW_VAR, r, F("Hardware"));
    /*
      s2->addVar(MW_LIST, &driving);
      s2->addItem(MW_LIST, F("Fermenter"));
      s2->addItem(MW_LIST, F("Sous Vide"));
      s2->addItem(MW_LIST, F("Smoker"));
      */
    s1=menu.addMenu(MW_SUBMENU, r, F("Auto Tune"));
      s1->setBehaviour(MW_MENU_COLLAPSED,true);
      s2=menu.addMenu(MW_VAR, s1, F("Noise Band (F)"));
        //ss1->addVar(MW_AUTO_FLOAT, &noiseBand, 0.0, 50.0, 1.0);
      s2=menu.addMenu(MW_VAR, s1, F("Lookback (s)"));
        //ss2->addVar(MW_AUTO_FLOAT, &lookbackMin, 0.5, 10.0, 0.5);
      s2=menu.addMenu(MW_VAR,s1,F("DOIT NAOW!!!"));
        //ss3->addVar(MW_ACTION,startAutotune);
    s1=menu.addMenu(MW_VAR,r,F("Save Settings"));
      s1->addVar(MW_BOOLEAN, &tuning);  

  //menu.addUsrScreen(defaultDisplay,1000);
  //menu.writeEeprom();
  //menu.readEeprom();
  
  pinMode(upPin, INPUT);
  pinMode(downPin, INPUT);
  pinMode(escapePin, INPUT);
  pinMode(confirmPin, INPUT);
  menu.navButtons(upPin, downPin, escapePin, confirmPin);
  Serial.println("UI Setup");
  
  // Update tuning
  //EEPROMex.readBlock(0, profiles, 4);
  Serial.println("Profiles updated");
}

void loop() {
  /*
  if(tuning && aTune.Runtime()) {
    finishAutotune();
  } else if(!tuning) {
    myPID.SetMode(!paused);
    myPID.SetTunings(currentProfile()->kp, currentProfile()->ki, currentProfile()->kd);
    myPID.SetSampleTime(currentProfile()->sampleTime);  // Update the control value once per second
    myPID.Compute();
  }
  
  checkTemp();
  */
  menu.draw();
  
  /*
  Serial.print(digitalRead(upPin) == HIGH);
  Serial.print(digitalRead(downPin) == HIGH);
  Serial.print(digitalRead(escapePin) == HIGH);
  Serial.println(digitalRead(confirmPin) == HIGH);
  Serial.flush();
  */
}

struct profile* currentProfile() {
  switch (driving) {
  case drivingFermenter:
    return profiles[0];
  case drivingSousVide:
    return profiles[1];
  case drivingSmoker:
    return profiles[2];
  default:
    return profiles[3];
  }
}

void finishAutotune() {
  currentProfile()->kp = aTune.GetKp();
  currentProfile()->ki = aTune.GetKi();
  currentProfile()->kd = aTune.GetKd();
  saveSettings();
  tuning = false;
}

void requestTemp() {
  waitingForTemp = true;
  tempSensor.requestTemperatures();
}

void checkTemp() {
  if(waitingForTemp && tempSensor.isConversionAvailable(0)) {
    temperature = tempSensor.getTempF(insideThermometer);
    tempSensor.requestTemperatures();
  } else if(!waitingForTemp) {
    tempSensor.requestTemperatures();
  }
}

void defaultDisplay() {
  sprintf(menu.sbuf, "sp: %.1f t: %.1f\npower: %i%%", setpoint, temperature, power);

  menu.drawUsrScreen(menu.sbuf);
  delay(500);
}

void startAutotune() {
  tuning = true;
  aTune.SetNoiseBand(noiseBand);
  aTune.SetOutputStep(50);
  aTune.SetLookbackSec((int)(lookbackMin * 60));
}

void saveSettings(){
  //menu.writeEeprom();
  //EEPROMex.updateBlock(0, profiles, 4);
  Serial.println("saveSettings called");
  Serial.flush();
}

