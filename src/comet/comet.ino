#include <SPI.h>
#include "pin_config.h"
#include "display_config.h"
#include "utils.h"

void setup() {
  Serial.begin(115200);
  setup_display();
  setup_pins();
  setup_qtr();
  motor_pin_setup();
}

void loop() {
  menu_handle();
}
