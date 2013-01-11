#ifndef INPUT_H
#define INPUT_H

#include "Wire.h"
#include "OneWire.h"
#include "DallasTemperature.h"

#include "structs.h"

#define one_wire_pin 8

extern double temperature;
extern double power;
extern configuration config;
extern profile profiles[4];
extern bool tuning;

void input_setup(void);
void input_loop(void);

void initialize_sensors(void);

#endif
