#include <SPI.h>
#include "pin_config.h"
#include "display_config.h"
#include "utils.h"

void setup() {
  Serial.begin(115200);
  setup_display();
  setup_pins();
}

void loop() {
  menu_handle();
}
