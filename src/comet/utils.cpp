#include <QTRSensors.h>
#include "display_config.h"
#include "pin_config.h"
#include "utils.h"

String line_type = "Black";
String message;

// Menu setup
const String menu_options[] = { "Calibrate", "Read", "Line", "Test Display", "Test Button", "Test Motor" };
int menu_option_len = sizeof(menu_options) / sizeof(menu_options[0]);
int menu_counter = 0;

void menu_handle() {
  // print current selection

  if (menu_options[menu_counter] == "Line") {
    v_print(menu_options[menu_counter] + ":\n" + line_type, 2);
  } else {
    v_print(menu_options[menu_counter], 2);
  }

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
    read();
  } else if (menu_options[menu_counter] == "Line") {
    toggle_line();
  }else if (menu_options[menu_counter] == "Test Motor") {
    test_motor();
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


void test_motor() {
  // === LEFT TB6612FNG TEST ===
  v_print("Testing LEFT TB6612FNG...", 1);

  // Forward
  digitalWrite(ain1_l, HIGH);
  digitalWrite(ain2_l, LOW);
  digitalWrite(bin1_l, HIGH);
  digitalWrite(bin2_l, LOW);

  for (int speed = 0; speed <= 255; speed += 5) {
    analogWrite(pwma_l, speed);
    analogWrite(pwmb_l, speed);
    delay(20);
  }

  delay(500);
  analogWrite(pwma_l, 0);
  analogWrite(pwmb_l, 0);
  delay(300);

  // Reverse
  digitalWrite(ain1_l, LOW);
  digitalWrite(ain2_l, HIGH);
  digitalWrite(bin1_l, LOW);
  digitalWrite(bin2_l, HIGH);

  for (int speed = 0; speed <= 255; speed += 5) {
    analogWrite(pwma_l, speed);
    analogWrite(pwmb_l, speed);
    delay(20);
  }

  delay(500);
  analogWrite(pwma_l, 0);
  analogWrite(pwmb_l, 0);

  // === RIGHT TB6612FNG TEST ===
  v_print("Testing RIGHT TB6612FNG...", 1);

  // Forward
  digitalWrite(ain1_r, HIGH);
  digitalWrite(ain2_r, LOW);
  digitalWrite(bin1_r, HIGH);
  digitalWrite(bin2_r, LOW);

  for (int speed = 0; speed <= 255; speed += 5) {
    analogWrite(pwma_r, speed);
    analogWrite(pwmb_r, speed);
    delay(20);
  }

  delay(500);
  analogWrite(pwma_r, 0);
  analogWrite(pwmb_r, 0);
  delay(300);

  // Reverse
  digitalWrite(ain1_r, LOW);
  digitalWrite(ain2_r, HIGH);
  digitalWrite(bin1_r, LOW);
  digitalWrite(bin2_r, HIGH);

  for (int speed = 0; speed <= 255; speed += 5) {
    analogWrite(pwma_r, speed);
    analogWrite(pwmb_r, speed);
    delay(20);
  }

  delay(500);
  analogWrite(pwma_r, 0);
  analogWrite(pwmb_r, 0);

  v_print("Motor test complete.", 1);
}

// Sensor setup
QTRSensors qtr;
const uint8_t sensor_pins[] = { A15, A13, A11, A9, A7, A5, A3, A1 };
unsigned int sensorValues[8];

void setup_qtr() {
  qtr.setTypeAnalog();
  qtr.setSensorPins(sensor_pins, 8);
}

void calibrate() {
  v_print("Calibrating", 1);
  for (uint16_t i = 0; i < 400; i++) {
    qtr.calibrate();
  }

  message = "Minimum\n";
  for (uint8_t i = 0; i < 8; ++i) {
    message = message + " " + String(qtr.calibrationOn.minimum[i]);
  }
  v_print(message, 1);
  delay(1500);

  message = "Maximum\n";
  for (uint8_t i = 0; i < 8; ++i) {
    message = message + " " + String(qtr.calibrationOn.maximum[i]);
  }
  v_print(message, 1);
  delay(1500);
}

void read() {
  while (1) {
    uint16_t position;
    if (line_type[0] == "B") {
      // read sensor values and obtain a measure of the line position
      position = qtr.readLineBlack(sensorValues);  // from 0 to 5000
    }

    else {
      // read sensor values and obtain a measure of the line position
      position = qtr.readLineWhite(sensorValues);  // from 0 to 5000
    }

    // print the sensor values as numbers from 0 to 1000
    message = "";
    for (uint8_t i = 0; i < 8; i++) {
      // where 0 means maximum reflectance and 1000 means minimum reflectance,
      message = message + " " + String(sensorValues[i]);
    }
    // followed by the line position
    message = message + "\n" + position;

    v_print(message, 1);

    // back traverse check
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

void toggle_line() {
  if (line_type == "Black") {
    line_type = "White";
  } else {
    line_type = "Black";
  }
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
    display.fillRoundRect(i, i, display.width() - 2 * i, display.height() - 2 * i,
                          display.height() / 4, SSD1306_INVERSE);
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
