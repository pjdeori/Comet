#include <QTRSensors.h>
#include "utils.h"
#include "pin_config.h"

// Menu setup
const String menu_options[] = { "Start", "Calibrate", "Read", "Speed", "Line", "Stop On", "Stop After", "Test Motor" };
int menu_option_len = sizeof(menu_options) / sizeof(menu_options[0]);
int menu_counter = 0;

void menu_handle() {
  print_display(menu_options[menu_counter], 2);

  // if option selected
  if (digitalRead(NEXT) == LOW) {
    option_handle();
    delay(200);
  } else if (digitalRead(SELECT) == LOW) {
    if (menu_counter > 0) menu_counter -= 1;
    delay(200);
  } else if (digitalRead(PREV) == LOW) {
    if (menu_counter < menu_option_len - 1) menu_counter += 1;
    delay(200);
  }
}

void option_handle() {
  if (menu_options[menu_counter] == "Test Display") {
    test_display();
  } else if (menu_options[menu_counter] == "Test Button") {
    test_buttons();
  } else if (menu_options[menu_counter] == "Calibrate") {
    calibrate();
  } else if (menu_options[menu_counter] == "Read") {
    show_sensor_value();
  } else if (menu_options[menu_counter] == "Line") {
    toggle_line();
  } else if (menu_options[menu_counter] == "Test Motor") {
    test_motor();
  } else if (menu_options[menu_counter] == "Stop On") {
    set_stop_condition();
  } else if (menu_options[menu_counter] == "Stop After") {
    set_stop_time();
  } else if (menu_options[menu_counter] == "Speed") {
    set_max_speed();
  } else if (menu_options[menu_counter] == "Start") {
    race();
  }
}

// sensor
QTRSensors qtr;
const uint8_t sensor_pins[] = { PA7, PA6, PA5, PA4, PA3, PA2, PA1, PA0 };
uint16_t sensor_values[8];
uint16_t position;
unsigned int min_threshold;
unsigned int max_threshold;

String line_type = "Black";
String message;

void setup_qtr() {
  qtr.setTypeAnalog();
  qtr.setSensorPins(sensor_pins, 8);
  qtr.calibrate();
}

void calibrate() {
  print_display("Place on Black\nPress PREV to calibrate", 1);
  bool white_done = false;
  bool black_done = false;

  while (!white_done) {
    if (digitalRead(PREV) == LOW) {
      print_display("Calibrating", 1);
      for (int i = 0; i < 25; ++i) {
        qtr.calibrate();
      }
      white_done = true;
    }
  }

  print_display("Place on White\nPress PREV to calibrate", 1);
  while (!black_done) {
    if (digitalRead(PREV) == LOW) {
      print_display("Calibrating", 1);
      for (int i = 0; i < 25; ++i) {
        qtr.calibrate();
      }
      black_done = true;
    }
  }

  set_stop_condition_threshold();

  // message = "Minimum\n";
  // for (uint8_t i = 0; i < 8; ++i) {
  //   message = message + " " + String(qtr.calibrationOn.minimum[i]);
  // }
  // print_display(message, 1);
  // delay(1500);
  print_display("Min Thres:\n" + String(min_threshold), 1);
  delay(1500);

  // message = "Maximum\n";
  // for (uint8_t i = 0; i < 8; ++i) {
  //   message = message + " " + String(qtr.calibrationOn.maximum[i]);
  // }
  // print_display(message, 1);
  // delay(1500);
  print_display("Max Thres:\n" + String(max_threshold), 1);
  delay(1500);
}

void toggle_line() {
  message = "Line:\n";

  // if button pressed
  while (true) {
    if (digitalRead(SELECT) == LOW || digitalRead(PREV) == LOW) {
      // toggle type
      if (line_type == "Black") {
        line_type = "White";
      } else {
        line_type = "Black";
      }
      delay(100);
    }

    // show type
    print_display(message + line_type);

    // check if button pressed to go back
    unsigned long startTime = millis();
    while (digitalRead(NEXT) == LOW) {
      if (millis() - startTime >= 500) {
        print_display("release to go back", 1);
        if (digitalRead(NEXT) == HIGH) {
          return;
        }
      }
    }
  }
}

String stop_condition = "Black";
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
    while (digitalRead(NEXT) == LOW) {
      if (millis() - startTime >= 500) {
        print_display("release to go back", 1);
        if (digitalRead(NEXT) == HIGH) {
          return;
        }
      }
    }
  }
}

int stop_time = 500;
void set_stop_time() {
  while (true) {
    message = "Stop After\n";
    // decrease
    if (digitalRead(SELECT) == LOW) {
      if (stop_time > 0) {
        stop_time -= 100;
      }
      delay(100);
    }
    // increase
    if (digitalRead(PREV) == LOW) {
      if (stop_time < 1500) {
        stop_time += 100;
      }
      delay(100);
    }
    print_display(message + stop_time + " ms");
    // check if button pressed to go back
    unsigned long startTime = millis();
    while (digitalRead(NEXT) == LOW) {
      if (millis() - startTime >= 500) {
        print_display("release to go back", 1);
        if (digitalRead(NEXT) == HIGH) {
          return;
        }
      }
    }
  }
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

bool stop_flag = false;
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



void read_sensor() {
  if (line_type[0] == 'B') {
    // read sensor values and obtain a measure of the line position
    position = qtr.readLineBlack(sensor_values);
  }

  else {
    // read sensor values and obtain a measure of the line position
    position = qtr.readLineWhite(sensor_values);
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
    while (digitalRead(NEXT) == LOW) {
      if (millis() - startTime >= 500) {
        print_display("release to go back", 1);
        if (digitalRead(NEXT) == HIGH) {
          return;
        }
      }
    }
  }
}
float Kp = 0.05;
float Ki = 0;
float Kd = 0.5;

int error = 0;
int last_error = 0;
int integral = 0;
int max_speed = 255;
int min_speed = 50;

int left_pwm;
int right_pwm;

void pid() {
  // left is 0 right is 7000
  error = 3500 - position;
  integral += error;
  int derivative = error - last_error;
  last_error = error;
  int correction = Kp * error + Ki * integral + Kd * derivative;
  left_pwm = constrain(min_speed + correction, 0, max_speed);
  right_pwm = constrain(min_speed - correction, 0, max_speed);
}

void race() {
  for (int i = 3; i > 0; --i) {
    print_display(String(i));
    delay(500);
  }
  print_display("");

  set_direction(1);
  unsigned long stop_trigger_time = 0;
  stop_flag = false;
  while (!stop_flag) {
    read_sensor();
    pid();
    drive(left_pwm, right_pwm);

    // stop condition check
    if (check_stop_trigger()) {
      if (stop_trigger_time == 0) {
        stop_trigger_time = millis();
      } else if (millis() - stop_trigger_time > stop_time) {
        stop_flag = true;
      }
    } else {
      stop_trigger_time = 0;
    }
    if (digitalRead(NEXT) == LOW) {
      stop_flag = true;
    }
  }
  set_direction(0);
}

// motor
void setup_motor() {
  // Left motor
  pinMode(PWM_A, OUTPUT);
  pinMode(AIN_1, OUTPUT);
  pinMode(AIN_2, OUTPUT);

  // Right motor
  pinMode(PWM_B, OUTPUT);
  pinMode(BIN_1, OUTPUT);
  pinMode(BIN_2, OUTPUT);

  // STBY pin
  pinMode(STBY, OUTPUT);
  digitalWrite(STBY, HIGH);  // enable motor driver

  // Set all motor pins LOW initially
  digitalWrite(PWM_A, LOW);
  digitalWrite(AIN_1, LOW);
  digitalWrite(AIN_2, LOW);
  digitalWrite(PWM_B, LOW);
  digitalWrite(BIN_1, LOW);
  digitalWrite(BIN_2, LOW);
}

void set_max_speed() {
  message = "Speed:\n";

  // decrement
  while (true) {
    unsigned long long_press_start_time = millis();
    if (digitalRead(SELECT) == LOW) {
      if (max_speed > 0) {
        max_speed -= 1;
      }
      while (digitalRead(SELECT) != HIGH) {  //do until release
        if (millis() - long_press_start_time > 250) {
          if (max_speed > 0) {
            max_speed -= 1;
          }
          print_display(message + max_speed);
          delay(5);
        }
      }
    }
  }
}

// Set motor direction: -1=reverse, 1=forward, 0=stop
void set_direction(int direction) {
  if (direction == -1) {
    // Reverse
    digitalWrite(AIN_1, HIGH);
    digitalWrite(AIN_2, LOW);
    digitalWrite(BIN_1, LOW);
    digitalWrite(BIN_2, HIGH);
  } else if (direction == 1) {
    // Forward
    digitalWrite(AIN_1, LOW);
    digitalWrite(AIN_2, HIGH);
    digitalWrite(BIN_1, HIGH);
    digitalWrite(BIN_2, LOW);
  } else {
    // Neutral (stop)
    digitalWrite(AIN_1, LOW);
    digitalWrite(AIN_2, LOW);
    digitalWrite(BIN_1, LOW);
    digitalWrite(BIN_2, LOW);
    digitalWrite(STBY, LOW);  // standby mode when not driving
    return;
  }

  digitalWrite(STBY, HIGH);  // ensure motor driver is active
}

// Drive motors with PWM values
void drive(int left_pwm, int right_pwm) {
  digitalWrite(STBY, HIGH);  // ensure motors are enabled
  analogWrite(PWM_A, left_pwm);
  analogWrite(PWM_B, right_pwm);
}

// Motor test
void test_motor() {
  print_display("Testing Motor", 1);

  // Forward test
  set_direction(1);
  for (int speed = 0; speed <= max_speed; speed += 1) {
    drive(speed, speed);
    delay(20);
  }

  // Reverse test
  set_direction(-1);
  for (int speed = 0; speed <= max_speed; speed += 1) {
    drive(speed, speed);
    delay(20);
  }

  // Stop
  set_direction(0);
}

// button

