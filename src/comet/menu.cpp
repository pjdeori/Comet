#include "utils.h"
#include "pin_config.h"

// Menu setup
const String menu_options[] = {
  "Start",
  "Calibrate",
  "Kp",
  "Kd",
  "Ki",
  "Line",
  "Read",
  "Stop On",
  "Stop After",
  "Speed",
  "Test Motor"
};
int total_options = sizeof(menu_options) / sizeof(menu_options[0]);
int menu_counter = 0;
int debounce_delay = 150;
String message = "";

void menu_handle() {
  // show menu option
  print_display(menu_options[menu_counter], 2);

  // while button held down
  // trigger on release of button
  if (digitalRead(SELECT) == LOW) {
    while (digitalRead(SELECT) == LOW) {
    }
    if (menu_options[menu_counter] == "Start") race();
    if (menu_options[menu_counter] == "Calibrate") calibrate();
    if (menu_options[menu_counter] == "Kp") set_kp();
    if (menu_options[menu_counter] == "Kd") set_kd();
    if (menu_options[menu_counter] == "Ki") set_ki();
    if (menu_options[menu_counter] == "Line") toggle_line();
    if (menu_options[menu_counter] == "Read") show_sensor_value();
    if (menu_options[menu_counter] == "Stop On") set_stop_condition();
    if (menu_options[menu_counter] == "Stop After") set_stop_time();
    if (menu_options[menu_counter] == "Speed") set_max_speed();
    if (menu_options[menu_counter] == "Test Motor") test_motor();
  }

  // if next pressed
  if (digitalRead(NEXT) == LOW) {
    if (menu_counter == total_options - 1) {
      menu_counter = 0;
    } else {
      menu_counter += 1;
    }
    delay(debounce_delay);
  }

  // if previous pressed
  if (digitalRead(PREV) == LOW) {
    if (menu_counter == 0) {
      menu_counter = total_options - 1;
    } else {
      menu_counter -= 1;
    }
    delay(debounce_delay);
  }
}
