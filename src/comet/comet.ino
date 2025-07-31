#include "utils.h"


void setup() {
  setup_display();
  setup_buttons();
  setup_qtr();
  motor_pin_setup();
}

void loop() {
  menu_handle();
}
