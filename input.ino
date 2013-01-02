/*
#define oneWirePin 11

// Input
byte insideThermometer[8];
OneWire oneWire(oneWirePin);
DallasTemperature tempSensor(&oneWire);
boolean waitingForTemp = false;

void input_setup(){
  // Initialize sensors
  pinMode( oneWirePin, INPUT );
  tempSensor.begin();
  tempSensor.setResolution(insideThermometer, 12);
  tempSensor.setWaitForConversion(false);
  requestTemp();
  Serial.println("Sensors setup");
}

void input_loop(){
  if(waitingForTemp && tempSensor.isConversionAvailable(0)) {
    temperature = tempSensor.getTempF(insideThermometer);
    tempSensor.requestTemperatures();
  } else if(!waitingForTemp) {
    tempSensor.requestTemperatures();
  }
}

void requestTemp() {
  waitingForTemp = true;
  tempSensor.requestTemperatures();
}
*/
