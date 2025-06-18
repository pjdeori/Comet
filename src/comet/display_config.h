#ifndef DISPLAY_CONFIG_H
#define DISPLAY_CONFIG_H

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Display settings
#define screen_width 128
#define screen_height 32
#define oled_reset -1
#define screen_address 0x3C

extern Adafruit_SSD1306 display;

void init_display();
void v_print(String message, int font_size = 2);

#endif
