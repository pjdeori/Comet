#include <QTRSensors.h>
#include "utils.h"
#include "pin_config.h"

// Menu setup
const String menu_options[] = { "Start", "Calibrate", "Read", "Speed", "Line", "Stop On", "Stop After", "Test Motor" };
int menu_option_len = sizeof(menu_options) / sizeof(menu_options[0]);
int menu_counter = 0;

void menu_handle() {
  // print current selection

  v_print(menu_options[menu_counter], 2);

  // if option selected
  if (digitalRead(PLUS) == LOW) {
    option_handle();
    delay(200);
  } else if (digitalRead(SELECT) == LOW) {
    if (menu_counter > 0) menu_counter -= 1;
    delay(200);
  } else if (digitalRead(MINUS) == LOW) {
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
  v_print("Place on Black\nPress MINUS to calibrate", 1);
  bool white_done = false;
  bool black_done = false;

  while (!white_done) {
    if (digitalRead(MINUS) == LOW) {
      v_print("Calibrating", 1);
      for (int i = 0; i < 25; ++i) {
        qtr.calibrate();
      }
      white_done = true;
    }
  }

  v_print("Place on White\nPress MINUS to calibrate", 1);
  while (!black_done) {
    if (digitalRead(MINUS) == LOW) {
      v_print("Calibrating", 1);
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
  // v_print(message, 1);
  // delay(1500);
  v_print("Min Thres:\n" + String(min_threshold), 1);
  delay(1500);

  // message = "Maximum\n";
  // for (uint8_t i = 0; i < 8; ++i) {
  //   message = message + " " + String(qtr.calibrationOn.maximum[i]);
  // }
  // v_print(message, 1);
  // delay(1500);
  v_print("Max Thres:\n" + String(max_threshold), 1);
  delay(1500);
}

void toggle_line() {
  message = "Line:\n";

  // if button pressed
  while (true) {
    if (digitalRead(SELECT) == LOW || digitalRead(MINUS) == LOW) {
      // toggle type
      if (line_type == "Black") {
        line_type = "White";
      } else {
        line_type = "Black";
      }
      delay(100);
    }

    // show type
    v_print(message + line_type);

    // check if button pressed to go back
    unsigned long startTime = millis();
    while (digitalRead(PLUS) == LOW) {
      if (millis() - startTime >= 500) {
        v_print("release to go back", 1);
        if (digitalRead(PLUS) == HIGH) {
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
    if (digitalRead(SELECT) == LOW || digitalRead(MINUS) == LOW) {
      // toggle condition
      if (stop_condition == "Black") {
        stop_condition = "White";
      } else {
        stop_condition = "Black";
      }
      delay(200);
    }

    // show type
    v_print(message + stop_condition);

    // check if button pressed to go back
    unsigned long startTime = millis();
    while (digitalRead(PLUS) == LOW) {
      if (millis() - startTime >= 500) {
        v_print("release to go back", 1);
        if (digitalRead(PLUS) == HIGH) {
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
    if (digitalRead(MINUS) == LOW) {
      if (stop_time < 1500) {
        stop_time += 100;
      }
      delay(100);
    }
    v_print(message + stop_time + " ms");
    // check if button pressed to go back
    unsigned long startTime = millis();
    while (digitalRead(PLUS) == LOW) {
      if (millis() - startTime >= 500) {
        v_print("release to go back", 1);
        if (digitalRead(PLUS) == HIGH) {
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

    v_print(message, 1);

    // check if button pressed to go back
    unsigned long startTime = millis();
    while (digitalRead(PLUS) == LOW) {
      if (millis() - startTime >= 500) {
        v_print("release to go back", 1);
        if (digitalRead(PLUS) == HIGH) {
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
    v_print(String(i));
    delay(500);
  }
  v_print("");

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
    if (digitalRead(PLUS) == LOW) {
      stop_flag = true;
    }
  }
  set_direction(0);
}

// motor
void motor_pin_setup() {
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
          v_print(message + max_speed);
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
  v_print("Testing Motor", 1);

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
void setup_buttons() {
  pinMode(SELECT, INPUT_PULLUP);
  pinMode(MINUS, INPUT_PULLUP);
  pinMode(PLUS, INPUT_PULLUP);
}

void test_buttons() {
  while (1) {
    if (digitalRead(SELECT) == LOW) v_print("Select");
    if (digitalRead(MINUS) == LOW) v_print("Minus");
    if (digitalRead(PLUS) == LOW) {
      v_print("Plus");
      unsigned long startTime = millis();
      while (digitalRead(PLUS) == LOW) {
        if (millis() - startTime >= 500) {
          v_print("Release to go back", 1);
          if (digitalRead(PLUS) == HIGH) {
            return;
          }
        }
      }
    }
  }
}

// display
Adafruit_SSD1306 display(screen_width, screen_height, &Wire, oled_reset);

void setup_display() {
  if (!display.begin(SSD1306_SWITCHCAPVCC, screen_address)) {
    Serial.println(F("SSD1306 allocation failed"));
    while (true)
      ;  // Infinite loop to halt
  }

  display.setTextColor(SSD1306_WHITE);
  display.clearDisplay();
  display.display();
}

void v_print(String message, int font_size) {
  display.clearDisplay();
  display.setTextSize(font_size);
  display.setCursor(0, 0);
  display.println(message);
  display.display();
}

void testdrawrect() {
  display.clearDisplay();
  for (int16_t i = 0; i < display.height() / 2; i += 2) {
    display.drawRect(i, i, display.width() - 2 * i, display.height() - 2 * i, SSD1306_WHITE);
    display.display();  // Update screen with each newly-drawn rectangle
    delay(1);
  }
}

void testfillrect() {
  display.clearDisplay();
  for (int16_t i = 0; i < display.height() / 2; i += 3) {
    // The INVERSE color is used so rectangles alternate white/black
    display.fillRect(i, i, display.width() - i * 2, display.height() - i * 2, SSD1306_INVERSE);
    display.display();  // Update screen with each newly-drawn rectangle
    delay(1);
  }
}

void testdrawcircle() {
  display.clearDisplay();
  for (int16_t i = 0; i < max(display.width(), display.height()) / 2; i += 2) {
    display.drawCircle(display.width() / 2, display.height() / 2, i, SSD1306_WHITE);
    display.display();
    delay(1);
  }
}

void testfillcircle() {
  display.clearDisplay();
  for (int16_t i = max(display.width(), display.height()) / 2; i > 0; i -= 3) {
    // The INVERSE color is used so circles alternate white/black
    display.fillCircle(display.width() / 2, display.height() / 2, i, SSD1306_INVERSE);
    display.display();  // Update screen with each newly-drawn circle
    delay(1);
  }
}

void testdrawroundrect() {
  display.clearDisplay();
  for (int16_t i = 0; i < display.height() / 2 - 2; i += 2) {
    display.drawRoundRect(i, i, display.width() - 2 * i, display.height() - 2 * i, display.height() / 4, SSD1306_WHITE);
    display.display();
    delay(1);
  }
}

void testfillroundrect() {
  display.clearDisplay();
  for (int16_t i = 0; i < display.height() / 2 - 2; i += 2) {
    // The INVERSE color is used so round-rects alternate white/black
    display.fillRoundRect(i, i, display.width() - 2 * i, display.height() - 2 * i, display.height() / 4, SSD1306_INVERSE);
    display.display();
    delay(1);
  }
}

void testdrawchar() {
  display.clearDisplay();
  display.setTextSize(1);               // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);  // Draw white text
  display.setCursor(0, 0);              // Start at top-left corner
  display.cp437(true);                  // Use full 256 char 'Code Page 437' font

  // Not all the characters will fit on the display. This is normal.
  // Library will draw what it can and the rest will be clipped.
  for (int16_t i = 0; i < 256; i++) {
    if (i == '\n') display.write(' ');
    else display.write(i);
  }
  display.display();
}

void testdrawstyles() {
  display.clearDisplay();
  display.setTextSize(1);               // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);  // Draw white text
  display.setCursor(0, 0);              // Start at top-left corner
  display.println(F("Hello, world!"));
  display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);  // Draw 'inverse' text
  display.println(3.141592);
  display.setTextSize(2);  // Draw 2X-scale text
  display.setTextColor(SSD1306_WHITE);
  display.print(F("0x"));
  display.println(0xDEADBEEF, HEX);
  display.display();
}

void test_display() {
  testdrawrect();       // Draw rectangles (outlines)
  testfillrect();       // Draw rectangles (filled)
  testdrawcircle();     // Draw circles (outlines)
  testfillcircle();     // Draw circles (filled)
  testdrawroundrect();  // Draw rounded rectangles (outlines)
  testfillroundrect();  // Draw rounded rectangles (filled)
  testdrawchar();       // Draw characters of the default font
  testdrawstyles();     // Draw 'stylized' characters
}