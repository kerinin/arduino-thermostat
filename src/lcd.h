#ifndef LCD_H
#define LCD_H

#include <Arduino.h>
#include "LiquidCrystal.h"

#include "structs.h"
#include "menu.h"

#define lcd_rs_pin 13
#define lcd_enable_pin 12
#define lcd_d4_pin 7
#define lcd_d5_pin 6
#define lcd_d6_pin 5
#define lcd_d7_pin 4

extern double temperature;
extern double power;
extern configuration config;
extern profile profiles[4];
extern bool tuning;
extern int screensaver_delay;
extern double last_button_press;

void lcd_setup(void);
void lcd_loop(void);

void default_display(void);
void menu_display(MenuItem current);
void flush_lcd(char *lcd1, char *lcd2);

#endif
