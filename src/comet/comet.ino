#include "utils.h"

void setup() {
  setup_display();
  setup_buttons();
  setup_qtr();
  setup_motor();
}

void loop() {
  // menu_handle();
  test_display();
}
