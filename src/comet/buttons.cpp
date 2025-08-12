#include "utils.h"
#include "pin_config.h"

void setup_buttons() {
  pinMode(SELECT, INPUT_PULLUP);
  pinMode(PREV, INPUT_PULLUP);
  pinMode(NEXT, INPUT_PULLUP);
}

void test_buttons() {
  while (1) {
    if (digitalRead(PREV) == LOW){
      print_display("Previous");
    } 

    if (digitalRead(NEXT) == LOW){
      print_display("Next");
    } 

    if (digitalRead(SELECT) == LOW) {
      print_display("Select");
      unsigned long startTime = millis();
      while (digitalRead(SELECT) == LOW) {
        if (millis() - startTime >= 500) {
          print_display("Release to go back", 1);
          if (digitalRead(SELECT) == HIGH) {
            return;
          }
        }
      }
    }
  }
}