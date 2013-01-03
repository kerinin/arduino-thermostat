#define oneWirePin 11

int lastReading;

// Input
OneWire oneWire(oneWirePin);
DallasTemperature tempSensor(&oneWire);

void input_setup(){
  // Initialize sensors
  pinMode( oneWirePin, INPUT );
  tempSensor.begin();
  tempSensor.setResolution(12);
  tempSensor.setWaitForConversion(false);
  tempSensor.setCheckForConversion(true);
  tempSensor.requestTemperatures();
  lastReading = millis();
}


void input_loop(){
  /*
  NOTE: this value is based on the bit resolution of the sensor:
  9bit:  94ms
  10bit: 188ms
  11bit: 375ms
  12bit: 750ms
  */
  if(millis() - lastReading > 750) {
    float tempC = tempSensor.getTempCByIndex(0);
    
    if(tempC == DEVICE_DISCONNECTED) {
      // Sensor disconnected
      temperature = -88.8;
    } else if(tempC == 85.0) {
      // Something's wrong with the sensor!
      // -> Reboot
      temperature = -99.9;
      resetFunc();
    } else {
      temperature = tempSensor.toFahrenheit(tempC);
      tempSensor.requestTemperatures();
      lastReading = millis();
    }
  }
}
