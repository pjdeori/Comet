#pragma once
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// global variables
extern const uint8_t sensor_pins[];
extern uint16_t sensor_values[8];
extern uint16_t position;
extern unsigned int min_threshold;
extern unsigned int max_threshold;
extern String line_type;
extern String stop_condition;
extern int stop_time;
extern bool stop_flag;
extern float Kp;
extern float Ki;
extern float Kd;
extern int error;
extern int last_error;
extern int integral;
extern int max_speed;
extern int min_speed;
extern int left_pwm;
extern int right_pwm;
extern unsigned long stop_trigger_time;
extern String message;

// menu
void menu_handle();
void option_handle();

// race
void set_kp();
void set_kd();
void set_ki();

// sensor
void setup_qtr();
void calibrate();
void set_stop_condition_threshold();
void set_stop_condition();
void set_stop_time();
void read_sensor();
void show_sensor_value();
void toggle_line();
bool check_stop_trigger();

// motor
void setup_motor();
void set_max_speed();
void pid();
void race(); 
void set_motor_direction(int direction);
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