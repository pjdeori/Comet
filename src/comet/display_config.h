#pragma once
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>

// Display settings
#define screen_width 128
#define screen_height 32
#define oled_reset -1
#define screen_address 0x3C

extern Adafruit_SSD1306 display;

void setup_display();
void v_print(String message, int font_size = 2);

void test_display();
void testdrawrect();
void testfillrect();
void testdrawcircle();
void testfillcircle();
void testdrawroundrect();
void testfillroundrect();
void testdrawchar();
void testdrawstyles();