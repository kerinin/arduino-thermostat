#ifndef ACTIONS_H
#define ACTIONS_H

#include <Arduino.h>
#include "MenuBackend.h"

#include "control.h"

extern bool save_later;
extern double last_button_press;

template <class T>
void increment_within_bounds(T *ref, int increment, int min_val, int max_val);

void increment_target(MenuMoveEvent menu);
void decrement_target(MenuMoveEvent menu);

void increment_hardware(MenuMoveEvent menu);
void decrement_hardware(MenuMoveEvent menu);

void increment_sample_time(MenuMoveEvent menu);
void decrement_sample_time(MenuMoveEvent menu);

void increment_noise(MenuMoveEvent menu); 
void decrement_noise(MenuMoveEvent menu);

void increment_lookback(MenuMoveEvent menu);
void decrement_lookback(MenuMoveEvent menu);

void on_set_tune(MenuMoveEvent menu);
void on_set_reset(MenuMoveEvent menu);

void reset_settings(void);
#endif
