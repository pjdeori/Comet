#pragma once

void setup_display();
void setup_pins();
void setup_qtr();
void calibrate();
void read();
void toggle_line();
void menu_handle();
void option_handle();
void go_back_handle();
void v_print(String message, int font_size = 2);
void test_display();
void test_buttons();
