#ifndef CONTROL_H
#define CONTROL_H

#include <Arduino.h>

#include "PID_v1.h"
#include "PID_AutoTune_v0.h"

#include "structs.h"
#include "menu.h"

#define control_pin 9

extern PID pid;
extern PID_ATune auto_tune;
extern double temperature;
extern double power;
extern configuration config;
extern profile profiles[4];
extern bool tuning;

void control_setup(void);
void control_loop(void);

void finish_autotune(void);
void start_autotune(void);

#endif
