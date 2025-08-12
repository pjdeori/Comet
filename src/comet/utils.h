#pragma once
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// menu
void menu_handle();
void option_handle();

// sensor
void setup_qtr();
void calibrate();
void set_stop_condition_threshold();
void set_stop_condition();
void set_stop_time();
void read();
void show_sensor_value();
void toggle_line();

// motor
void setup_motor();
void set_max_speed();
void pid();
void race(); 
void set_direction(int direction);
void drive(int left_pwm, int right_pwm);
void test_motor();

// button
void setup_buttons();
void test_buttons(); 

// display
#define screen_width 128
#define screen_height 32
#define oled_reset -1
#define screen_address 0x3C

void setup_display();

void print_display(String message, int font_size = 2);

void test_display();
void testdrawrect();
void testfillrect();
void testdrawcircle();
void testfillcircle();
void testdrawroundrect();
void testfillroundrect();
void testdrawchar();
void testdrawstyles();