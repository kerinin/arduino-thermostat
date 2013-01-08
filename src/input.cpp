#include "input.h"

/*
NOTE: this value is based on the bit resolution of the sensor:
9bit:  94ms
10bit: 188ms
11bit: 375ms
12bit: 750ms
*/
int sensor_read_delay = 375;
unsigned long last_reading;
DeviceAddress sensor_address;
OneWire one_wire(one_wire_pin);
DallasTemperature temp_sensor(&one_wire);

void initialize_sensors() {
  temp_sensor.begin();
  temp_sensor.setResolution(11);
  temp_sensor.setWaitForConversion(false);
  temp_sensor.setCheckForConversion(true);
}

void input_setup(){
  pinMode( one_wire_pin, INPUT );
  initialize_sensors();
  temp_sensor.getAddress(sensor_address, 0);
  temp_sensor.requestTemperatures();
  last_reading = millis();
}


void input_loop(){
  if(millis() - last_reading > sensor_read_delay) {
    float temp_c = temp_sensor.getTempC(sensor_address);
    
    if(temp_c == DEVICE_DISCONNECTED) {
      // Serial.println(F("Device disconnected"));
      
      temperature = -88.8;
    } else if(temp_c == 85.0) {
      Serial.println(F("Device read prematurely, re-initializing IC"));
      
      sensor_read_delay += 5;
      initialize_sensors();
      last_reading = millis();
    } else {
      if(temp_sensor.toFahrenheit(temp_c) != temperature) {
        temperature = temp_sensor.toFahrenheit(temp_c);
        Serial.print("Log\ttemp\t");
        Serial.println(temperature);
      }   
      temp_sensor.requestTemperatures();
      last_reading = millis();
    }
  }
}
