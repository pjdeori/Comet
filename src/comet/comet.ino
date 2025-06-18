#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <EEPROM.h>
#include <QTRSensors.h>

// display
#define screen_width 128
#define screen_height 32
#define oled_reset -1
#define screen_address 0x3C
Adafruit_SSD1306 display(screen_width, screen_height, &Wire, oled_reset);
int lineX1 = screen_width / 2;   // Start at the center for the first line
int lineX2 = screen_width / 2;   // Start at the center for the second line
int pulseSpeed = 3;              // Speed at which the lines move apart
int maxDist = screen_width / 2;  // Max distance the lines can move from the center

// button pins
#define b0 23
#define b1 25
#define b2 29
#define b3 31
#define b4 30

// left tb6612fng
#define pwma 13
#define ain1 11
#define ain2 9

#define pwmb 7
#define bin1 5
#define bin2 3

// right tb6612fng
#define pwmc 10
#define ain3 22
#define ain4 8

#define pwmd 6
#define bin3 4
#define bin4 2

// sonar
#define echo 24
#define trigger 26

// servo
#define servo 12