#include "utils.h"
#include "pin_config.h"

bool stop_flag = false;
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


