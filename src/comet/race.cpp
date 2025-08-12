#include "utils.h"
#include "pin_config.h"

bool stop_flag = false;
float Kp = 0.05;
float Kd = 0.5;
float Ki = 0;
int error = 0;
int last_error = 0;
int integral = 0;
int max_speed = 255;
int min_speed = 50;
int left_pwm;
int right_pwm;
unsigned long stop_trigger_time = 0;

void race() {
  // display count down
  for (int i = 3; i > 0; --i) {
    print_display(String(i));
    delay(500);
  }
  print_display("");

  set_motor_direction(1); // set motor direction to forward

  stop_trigger_time = 0;
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
  set_motor_direction(0);
}

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

void set_kp() {
  String message = "Kp:\n";

  // Starting Kp value (example, adjust to your scale)
  float step = 0.01; // amount to increment/decrement per press

  while (true) {
    unsigned long long_press_start_time = millis();

    if (digitalRead(PREV) == LOW) {
      if (Kp > 0.0) {
        Kp -= step;
        if (Kp < 0.0) Kp = 0.0;  // prevent negative Kp
      }
      while (digitalRead(PREV) != HIGH) {  // wait until release
        if (millis() - long_press_start_time > 250) {
          if (Kp > 0.0) {
            Kp -= step;
            if (Kp < 0.0) Kp = 0.0;
          }
          print_display(message + String(Kp, 2));
          delay(5);
        }
      }
    }

    else if (digitalRead(NEXT) == LOW) {
      if (Kp < 10.0) {  // arbitrary upper limit for Kp
        Kp += step;
      }
      while (digitalRead(NEXT) != HIGH) {  // wait until release
        if (millis() - long_press_start_time > 250) {
          if (Kp < 10.0) {
            Kp += step;
          }
          print_display(message + String(Kp, 2));
          delay(5);
        }
      }
    }

    print_display(message + String(Kp, 2));
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

void set_kd() {
  String message = "Kd:\n";

  // Starting Kp value (example, adjust to your scale)
  float step = 0.01; // amount to increment/decrement per press

  while (true) {
    unsigned long long_press_start_time = millis();

    if (digitalRead(PREV) == LOW) {
      if (Kd > 0.0) {
        Kd -= step;
        if (Kd < 0.0) Kd = 0.0;  // prevent negative Kd
      }
      while (digitalRead(PREV) != HIGH) {  // wait until release
        if (millis() - long_press_start_time > 250) {
          if (Kd > 0.0) {
            Kd -= step;
            if (Kd < 0.0) Kd = 0.0;
          }
          print_display(message + String(Kd, 2));
          delay(5);
        }
      }
    }

    else if (digitalRead(NEXT) == LOW) {
      if (Kd < 10.0) {  // arbitrary upper limit for Kd
        Kd += step;
      }
      while (digitalRead(NEXT) != HIGH) {  // wait until release
        if (millis() - long_press_start_time > 250) {
          if (Kd < 10.0) {
            Kd += step;
          }
          print_display(message + String(Kd, 2));
          delay(5);
        }
      }
    }

    print_display(message + String(Kd, 2));
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

void set_ki() {
  String message = "Ki:\n";

  // Starting Kp value (example, adjust to your scale)
  float step = 0.01; // amount to increment/decrement per press

  while (true) {
    unsigned long long_press_start_time = millis();

    if (digitalRead(PREV) == LOW) {
      if (Ki > 0.0) {
        Ki -= step;
        if (Ki < 0.0) Ki = 0.0;  // prevent negative Ki
      }
      while (digitalRead(PREV) != HIGH) {  // wait until release
        if (millis() - long_press_start_time > 250) {
          if (Ki > 0.0) {
            Ki -= step;
            if (Ki < 0.0) Ki = 0.0;
          }
          print_display(message + String(Ki, 2));
          delay(5);
        }
      }
    }

    else if (digitalRead(NEXT) == LOW) {
      if (Ki < 10.0) {  // arbitrary upper limit for Ki
        Ki += step;
      }
      while (digitalRead(NEXT) != HIGH) {  // wait until release
        if (millis() - long_press_start_time > 250) {
          if (Ki < 10.0) {
            Ki += step;
          }
          print_display(message + String(Ki, 2));
          delay(5);
        }
      }
    }

    print_display(message + String(Ki, 2));
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


