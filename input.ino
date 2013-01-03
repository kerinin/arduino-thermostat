#define one_wire_pin 11

int last_reading;

// Input
OneWire one_wire(one_wire_pin);
DallasTemperature temp_sensor(&one_wire);

void input_setup(){
  // Initialize sensors
  pinMode( one_wire_pin, INPUT );
  temp_sensor.begin();
  temp_sensor.setResolution(12);
  //temp_sensor.setWaitForConversion(false);
  temp_sensor.setCheckForConversion(true);
  temp_sensor.requestTemperatures();
  last_reading = millis();
}


void input_loop(){
  /*
  NOTE: this value is based on the bit resolution of the sensor:
  9bit:  94ms
  10bit: 188ms
  11bit: 375ms
  12bit: 750ms
  */
  if(millis() - last_reading > 750) {
    float temp_c = temp_sensor.getTempCByIndex(0);
    
    if(temp_c == DEVICE_DISCONNECTED) {
      // Sensor disconnected
      temperature = -88.8;
    } else if(temp_c == 85.0) {
      // Something's wrong with the sensor!
      // -> Reboot
      temperature = -99.9;
      reset_func();
    } else {
      temperature = temp_sensor.toFahrenheit(temp_c);
      temp_sensor.requestTemperatures();
      last_reading = millis();
      
      Serial.print("Log\ttemp\t");
      Serial.println(temperature);
    }
  }
}
