#include "utils.h"
#include "pin_config.h"

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
    if (digitalRead(PREV) == LOW) {
      if (max_speed > 0) {
        max_speed -= 1;
      }
      while (digitalRead(PREV) != HIGH) {  //do until release
        if (millis() - long_press_start_time > 250) {
          if (max_speed > 0) {
            max_speed -= 1;
          }
          print_display(message + max_speed);
          delay(5);
        }
      }
    }

    else if (digitalRead(NEXT) == LOW) {
      if (max_speed < 255) {
        max_speed += 1;
      }
      while (digitalRead(NEXT) != HIGH) {  //do until release
        if (millis() - long_press_start_time > 250) {
          if (max_speed < 256) {
            max_speed += 1;
          }
          print_display(message + max_speed);
          delay(5);
        }
      }
    }

    print_display(message + max_speed);
    delay(5);

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

// Set motor direction: -1=reverse, 1=forward, 0=stop
void set_motor_direction(int direction) {
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
  set_motor_direction(1);
  for (int speed = 0; speed <= max_speed; speed += 1) {
    drive(speed, speed);
    delay(20);
  }

  // Reverse test
  set_motor_direction(-1);
  for (int speed = 0; speed <= max_speed; speed += 1) {
    drive(speed, speed);
    delay(20);
  }

  // Stop
  set_motor_direction(0);
}