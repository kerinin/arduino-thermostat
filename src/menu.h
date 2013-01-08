#ifndef MENU_H
#define MENU_H

#include "MenuBackend.h"
#include "EEPROMex.h"
#include "buttons.h"
#include "avr/eeprom.h"

#include "actions.h"
#include "lcd.h"

#define escape_pin 11
#define confirm_pin 8
#define up_pin 9
#define down_pin 10

extern MenuBackend menu;
extern MenuItem mi_target;
extern MenuItem mi_hardware;
extern MenuItem mi_sample_time;
extern MenuItem mi_noise;
extern MenuItem mi_lookback;
extern MenuItem mi_tune;
extern MenuItem mi_reset;
extern MenuItem mi_reset_confirm;

void menu_setup(void);
void menu_loop(void);

void use_handler(MenuUseEvent e);
void change_handler(MenuChangeEvent e);
void load(void);
void save(void);

#endif
