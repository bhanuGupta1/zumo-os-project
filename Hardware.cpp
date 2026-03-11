#include "Hardware.h"
#include "Config.h"

Zumo32U4Motors motors;
Zumo32U4Encoders encoders;
Zumo32U4LineSensors lineSensors;
Zumo32U4ProximitySensors prox;
Zumo32U4OLED display;
Zumo32U4Buzzer buzzer;
Zumo32U4ButtonA buttonA;
Zumo32U4ButtonB buttonB;
Zumo32U4ButtonC buttonC;
Zumo32U4IMU imu;

unsigned int lineValues[5];
int lastError = 0;

void initializeHardware() {
  display.clear();
  display.print("ZumoOS Pro");
  display.gotoXY(0, 1);
  display.print("Advanced LF");
  Wire.begin();
  if (imu.init()) {
    imu.enableDefault();
  }
  delay(1000);
  display.clear();
}

void playStartupSound() {
  buzzer.play("!L16 ceg>c");
}

void playModeChangeSound() {
  buzzer.play("!L16 ce");
}

void playErrorSound() {
  buzzer.play("!L8 c<c");
}

void playSuccessSound() {
  buzzer.play("!L16 ece");
}

void playCalibrationDone() {
  buzzer.play("!L16 ceg>ce");
}

int clampSpeed(int speed) {
  if (speed > MOTOR_MAX) return MOTOR_MAX;
  if (speed < MOTOR_MIN) return MOTOR_MIN;
  return speed;
}

void stopMotors() {
  motors.setSpeeds(0, 0);
}

void setMotorSpeeds(int left, int right) {
  motors.setSpeeds(clampSpeed(left), clampSpeed(right));
}
