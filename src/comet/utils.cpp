#include <QTRSensors.h>
#include "display_config.h"
#include "pin_config.h"
#include "utils.h"

String line_type = "Black";
String message;

// Menu setup
const String menu_options[] = { "Start", "Calibrate", "Read", "Speed", "Line", "Stop On", "Stop After", "Test Motor" };
int menu_option_len = sizeof(menu_options) / sizeof(menu_options[0]);
int menu_counter = 0;

void menu_handle() {
  // print current selection

  v_print(menu_options[menu_counter], 2);

  // if option selected
  if (digitalRead(b2) == LOW) {
    option_handle();
    delay(200);
  } else if (digitalRead(b0) == LOW) {
    if (menu_counter > 0) menu_counter -= 1;
    delay(200);
  } else if (digitalRead(b1) == LOW) {
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

void motor_pin_setup() {
  // === Left TB6612FNG ===
  // Left motor
  pinMode(pwma_l, OUTPUT);
  pinMode(ain1_l, OUTPUT);
  pinMode(ain2_l, OUTPUT);

  // Right motor
  pinMode(pwmb_l, OUTPUT);
  pinMode(bin1_l, OUTPUT);
  pinMode(bin2_l, OUTPUT);

  // === Right TB6612FNG ===
  // Left motor
  pinMode(pwma_r, OUTPUT);
  pinMode(ain1_r, OUTPUT);
  pinMode(ain2_r, OUTPUT);

  // Right motor
  pinMode(pwmb_r, OUTPUT);
  pinMode(bin1_r, OUTPUT);
  pinMode(bin2_r, OUTPUT);

  // Set all pins LOW
  digitalWrite(pwma_l, LOW);
  digitalWrite(ain1_l, LOW);
  digitalWrite(ain2_l, LOW);
  digitalWrite(pwmb_l, LOW);
  digitalWrite(bin1_l, LOW);
  digitalWrite(bin2_l, LOW);

  digitalWrite(pwma_r, LOW);
  digitalWrite(ain1_r, LOW);
  digitalWrite(ain2_r, LOW);
  digitalWrite(pwmb_r, LOW);
  digitalWrite(bin1_r, LOW);
  digitalWrite(bin2_r, LOW);
}

int max_speed = 255;

void set_max_speed() {
  message = "Speed:\n";

  // decrement
  while (true) {
    unsigned long long_press_start_time = millis();
    if (digitalRead(b0) == LOW) {
      if (max_speed > 0) {
        max_speed -= 1;
      }
      while (digitalRead(b0) != HIGH) {  //do until release
        if (millis() - long_press_start_time > 250) {
          if (max_speed > 0) {
            max_speed -= 1;
          }
          v_print(message + max_speed);
          delay(5);
        }
      }
    }

    long_press_start_time = millis();
    if (digitalRead(b1) == LOW) {
      if (max_speed < 255) {
        max_speed += 1;
      }
      while (digitalRead(b1) != HIGH) {  //do until release
        if (millis() - long_press_start_time > 250) {
          if (max_speed < 255) {
            max_speed += 1;
          }
          v_print(message + max_speed);
          delay(5);
        }
      }
    }

    // show type
    v_print(message + max_speed);

    // check if button pressed to go back
    unsigned long startTime = millis();
    while (digitalRead(b2) == LOW) {
      if (millis() - startTime >= 500) {
        v_print("release to go back", 1);
        if (digitalRead(b2) == HIGH) {
          return;
        }
      }
    }
  }
}

void set_direction(int direction){
  if (direction==1){
    // Forward
    digitalWrite(ain1_l, HIGH);
    digitalWrite(ain2_l, LOW);
    digitalWrite(bin1_l, HIGH);
    digitalWrite(bin2_l, LOW);
  }
  else if (direction==-1){
    // Reverse
  digitalWrite(ain1_l, LOW);
  digitalWrite(ain2_l, HIGH);
  digitalWrite(bin1_l, LOW);
  digitalWrite(bin2_l, HIGH);
  }
  else {
    // Neutral
    digitalWrite(ain1_l, LOW);
    digitalWrite(ain2_l, LOW);
    digitalWrite(bin1_l, LOW);
    digitalWrite(bin2_l, LOW);
  }
}

void drive(int left_pwm, int right_pwm){
  analogWrite(pwma_l, left_pwm);
  analogWrite(pwmb_l, right_pwm);

}
void test_motor() {
  v_print("Testing Motor", 1);

  // Forward
  set_direction(1);
  for (int speed = 0; speed <= max_speed; speed += 1) {
    drive(speed, speed);
    delay(20);
  }

  set_direction(-1);

  for (int speed = 0; speed <= max_speed; speed += 1) {
    drive(speed, speed);
    delay(20);
  }

  set_direction(0);
}

// Sensor setup
QTRSensors qtr;
const uint8_t sensor_pins[] = { A15, A13, A11, A9, A7, A5, A3, A1 };
unsigned int sensor_values[8];
uint16_t position;
int min_threshold;
int max_threshold;

void setup_qtr() {
  qtr.setTypeAnalog();
  qtr.setSensorPins(sensor_pins, 8);
  qtr.calibrate();
}

void calibrate() {
  v_print("Place on Black\nPress b1 to calibrate", 1);
  bool white_done = false;
  bool black_done = false;

  while (!white_done) {
    if (digitalRead(b1) == LOW) {
      v_print("Calibrating", 1);
      for (int i = 0; i < 25; ++i) {
        qtr.calibrate();
      }
      white_done = true;
    }
  }

  v_print("Place on White\nPress b1 to calibrate", 1);
  while (!black_done) {
    if (digitalRead(b1) == LOW) {
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
    if (digitalRead(b0) == LOW || digitalRead(b1) == LOW) {
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
    while (digitalRead(b2) == LOW) {
      if (millis() - startTime >= 500) {
        v_print("release to go back", 1);
        if (digitalRead(b2) == HIGH) {
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
    if (digitalRead(b0) == LOW || digitalRead(b1) == LOW) {
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
    while (digitalRead(b2) == LOW) {
      if (millis() - startTime >= 500) {
        v_print("release to go back", 1);
        if (digitalRead(b2) == HIGH) {
          return;
        }
      }
    }
  }
}

int stop_time = 0;
void set_stop_time() {
  while (true) {
    message = "Stop After\n";
    // decrease
    if (digitalRead(b0) == LOW) {
      if (stop_time > 0) {
        stop_time -= 100;
      }
      delay(100);
    }
    // increase
    if (digitalRead(b1) == LOW) {
      stop_time += 100;
      delay(100);
    }
    v_print(message + stop_time + " ms");
    // check if button pressed to go back
    unsigned long startTime = millis();
    while (digitalRead(b2) == LOW) {
      if (millis() - startTime >= 500) {
        v_print("release to go back", 1);
        if (digitalRead(b2) == HIGH) {
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
bool check_stop_condition() {
  bool stop_condition_met = true;
  if (stop_condition == "Black") {
    for (int i = 0; i < 8; ++i) {
      if (sensor_values[i] > min_threshold) {
        stop_condition_met = false;
        break;
      }
    }
  }

  else {
    for (int i = 0; i < 8; ++i) {
      if (sensor_values[i] < max_threshold) {
        stop_condition_met = false;
        break;
      }
    }
  }
  return stop_condition_met;
}



void read_sensor() {
  if (line_type[0] == "B") {
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
    while (digitalRead(b2) == LOW) {
      if (millis() - startTime >= 500) {
        v_print("release to go back", 1);
        if (digitalRead(b2) == HIGH) {
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
int min_speed = 50;

int left_pwm;
int right_pwm;

void pid(){
  // left is 0 right is 7000
  error = 3500 - position;   
  integral += error;
  int derivative = error - last_error;
  last_error = error;  
  int correction = Kp * error + Ki * integral + Kd * derivative;
  left_pwm  = constrain(min_speed + correction, 0, max_speed);
  right_pwm = constrain(min_speed - correction, 0, max_speed);
}

void race() {
  for (int i = 3; i > 0; --i) {
    v_print(String(i));
    delay(1000);
  }
  v_print("");
  set_direction(1);
  while (!stop_flag) {
    read_sensor();
    pid();
    drive(left_pwm,right_pwm);
    if (digitalRead(b1)==LOW){
      stop_flag= true;
    }
  }
  set_direction(0);
}

// Button setup
void setup_pins() {
  pinMode(b0, INPUT_PULLUP);
  pinMode(b1, INPUT_PULLUP);
  pinMode(b2, INPUT_PULLUP);
  pinMode(b3, INPUT_PULLUP);
  pinMode(b4, INPUT_PULLUP);
}

void test_buttons() {
  while (1) {
    if (digitalRead(b0) == LOW) v_print("b0 is pressed");
    if (digitalRead(b1) == LOW) v_print("b1 is pressed");
    if (digitalRead(b2) == LOW) {
      v_print("b2 is pressed");
      unsigned long startTime = millis();
      while (digitalRead(b2) == LOW) {
        if (millis() - startTime >= 500) {
          if (digitalRead(b2) == HIGH) {
            return;
          }
        }
      }
    }
    if (digitalRead(b3) == LOW) v_print("b3 is pressed");
    if (digitalRead(b4) == LOW) v_print("b4 is pressed");
  }
}

// Display setup
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

void testdrawrect(void) {
  display.clearDisplay();
  for (int16_t i = 0; i < display.height() / 2; i += 2) {
    display.drawRect(i, i, display.width() - 2 * i, display.height() - 2 * i, SSD1306_WHITE);
    display.display();  // Update screen with each newly-drawn rectangle
    delay(1);
  }
}

void testfillrect(void) {
  display.clearDisplay();
  for (int16_t i = 0; i < display.height() / 2; i += 3) {
    // The INVERSE color is used so rectangles alternate white/black
    display.fillRect(i, i, display.width() - i * 2, display.height() - i * 2, SSD1306_INVERSE);
    display.display();  // Update screen with each newly-drawn rectangle
    delay(1);
  }
}

void testdrawcircle(void) {
  display.clearDisplay();
  for (int16_t i = 0; i < max(display.width(), display.height()) / 2; i += 2) {
    display.drawCircle(display.width() / 2, display.height() / 2, i, SSD1306_WHITE);
    display.display();
    delay(1);
  }
}

void testfillcircle(void) {
  display.clearDisplay();
  for (int16_t i = max(display.width(), display.height()) / 2; i > 0; i -= 3) {
    // The INVERSE color is used so circles alternate white/black
    display.fillCircle(display.width() / 2, display.height() / 2, i, SSD1306_INVERSE);
    display.display();  // Update screen with each newly-drawn circle
    delay(1);
  }
}

void testdrawroundrect(void) {
  display.clearDisplay();
  for (int16_t i = 0; i < display.height() / 2 - 2; i += 2) {
    display.drawRoundRect(i, i, display.width() - 2 * i, display.height() - 2 * i, display.height() / 4, SSD1306_WHITE);
    display.display();
    delay(1);
  }
}

void testfillroundrect(void) {
  display.clearDisplay();
  for (int16_t i = 0; i < display.height() / 2 - 2; i += 2) {
    // The INVERSE color is used so round-rects alternate white/black
    display.fillRoundRect(i, i, display.width() - 2 * i, display.height() - 2 * i, display.height() / 4, SSD1306_INVERSE);
    display.display();
    delay(1);
  }
}

void testdrawchar(void) {
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

void testdrawstyles(void) {
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
