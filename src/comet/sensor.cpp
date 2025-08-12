#include <QTRSensors.h>
#include "utils.h"
#include "pin_config.h"

QTRSensors qtr;
const uint8_t sensor_pins[] = { PA7, PA6, PA5, PA4, PA3, PA2, PA1, PA0 };
uint16_t sensor_values[8];
uint16_t position;
unsigned int min_threshold;
unsigned int max_threshold;
String line_type = "Black";
String stop_condition = "White";
int stop_time = 500;

void setup_qtr() {
  qtr.setTypeAnalog();
  qtr.setSensorPins(sensor_pins, 8);
  qtr.calibrate();
}

void calibrate() {
  print_display("Place on Black\nPress SELECT to calibrate", 1);
  bool white_done = false;
  bool black_done = false;

  while (!white_done) {
    if (digitalRead(SELECT) == LOW) {
      print_display("Calibrating", 1);
      for (int i = 0; i < 25; ++i) {
        qtr.calibrate();
      }
      white_done = true;
    }
  }

  print_display("Place on White\nPress PREV to calibrate", 1);
  while (!black_done) {
    if (digitalRead(SELECT) == LOW) {
      print_display("Calibrating", 1);
      for (int i = 0; i < 25; ++i) {
        qtr.calibrate();
      }
      black_done = true;
    }
  }

  set_stop_condition_threshold();

  print_display("Min Thres:\n" + String(min_threshold), 1);
  delay(1500);

  print_display("Max Thres:\n" + String(max_threshold), 1);
  delay(1500);
}

void set_stop_condition_threshold() {
  // get highest min threshold
  min_threshold = qtr.calibrationOn.minimum[0];
  for (uint8_t i = 0; i < 8; ++i) {
    if (min_threshold < qtr.calibrationOn.minimum[i]) {
      min_threshold = qtr.calibrationOn.minimum[i];
    }
  }

  // get lowest max threshold
  max_threshold = qtr.calibrationOn.maximum[0];
  for (uint8_t i = 0; i < 8; ++i) {
    if (max_threshold > qtr.calibrationOn.maximum[i]) {
      max_threshold = qtr.calibrationOn.maximum[i];
    }
  }
}

void toggle_line() {
  message = "Line:\n";

  // if button pressed
  while (true) {
    if (digitalRead(NEXT) == LOW || digitalRead(PREV) == LOW) {
      // toggle type
      if (line_type == "Black") {
        line_type = "White";
      } else {
        line_type = "Black";
      }
      delay(200);
    }

    // show type
    print_display(message + line_type);

    // check if button pressed to go back
    unsigned long startTime = millis();
    while (digitalRead(SELECT) == LOW) {
      if (millis() - startTime >= 500) {
        print_display("release to go back", 1);
        if (digitalRead(SELECT) == HIGH) {
          return;
        }
      }
    }
  }
}

void show_sensor_value() {
  while (1) {
    read_sensor();
    message = "";
    for (uint8_t i = 0; i < 8; i++) {
      message = message + " " + String(sensor_values[i]);
    }

    message = message + "\n" + position;

    print_display(message, 1);

    // check if button pressed to go back
    unsigned long startTime = millis();
    while (digitalRead(SELECT) == LOW) {
      if (millis() - startTime >= 500) {
        print_display("release to go back", 1);
        if (digitalRead(SELECT) == HIGH) {
          return;
        }
      }
    }
  }
}

void read_sensor() {
  if (line_type[0] == 'B') {
    position = qtr.readLineBlack(sensor_values);
  }

  else {
    position = qtr.readLineWhite(sensor_values);
  }
}

void set_stop_condition() {
  message = "Condition:\n";

  while (true) {
    if (digitalRead(SELECT) == LOW || digitalRead(PREV) == LOW) {
      // toggle condition
      if (stop_condition == "Black") {
        stop_condition = "White";
      } else {
        stop_condition = "Black";
      }
      delay(200);
    }

    // show type
    print_display(message + stop_condition);

    // check if button pressed to go back
    unsigned long startTime = millis();
    while (digitalRead(SELECT) == LOW) {
      if (millis() - startTime >= 500) {
        print_display("release to go back", 1);
        if (digitalRead(SELECT) == HIGH) {
          return;
        }
      }
    }
  }
}

void set_stop_time() {
  while (true) {
    message = "Stop After\n";
    // decrease
    if (digitalRead(PREV) == LOW) {
      if (stop_time > 0) {
        stop_time -= 100;
      }
      delay(100);
    }
    // increase
    if (digitalRead(NEXT) == LOW) {
      if (stop_time < 1500) {
        stop_time += 100;
      }
      delay(100);
    }
    print_display(message + stop_time + " ms");
    // check if button pressed to go back
    unsigned long startTime = millis();
    while (digitalRead(SELECT) == LOW) {
      if (millis() - startTime >= 500) {
        print_display("release to go back", 1);
        if (digitalRead(SELECT) == HIGH) {
          return;
        }
      }
    }
  }
}

bool check_stop_trigger() {
  if (stop_condition == "White") {
    for (int i = 0; i < 8; ++i) {
      if (sensor_values[i] > min_threshold) {
        return false;
      }
    }
  }

  else {
    for (int i = 0; i < 8; ++i) {
      if (sensor_values[i] < max_threshold) {
        return false;
      }
    }
  }
  return true;
}