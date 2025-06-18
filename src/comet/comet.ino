#include <SPI.h>
#include "pin_config.h"
#include "display_config.h"

void setup() {
  Serial.begin(115200);
  init_display();
}

void loop() {
  String message = "testing display";
  v_print(message);
}
