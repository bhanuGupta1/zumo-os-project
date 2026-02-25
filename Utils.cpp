#include "Utils.h"
#include "Hardware.h"
#include "Config.h"

// ============ UTILITY FUNCTIONS ============
void waitForButtonPress() {
  display.gotoXY(0, 3);
  display.print("Press any btn");
  
  while (!buttonA.isPressed() && !buttonB.isPressed() && !buttonC.isPressed()) {
    delay(10);
  }
  
  // Wait for release
  while (buttonA.isPressed() || buttonB.isPressed() || buttonC.isPressed()) {
    delay(10);
  }
}

void calibrateLineSensors() {
  display.clear();
  display.print("Calibrating");
  display.gotoXY(0, 1);
  display.print("Line Sensors");
  
  lineSensors.initFiveSensors();
  
  // Spin for calibration
  motors.setSpeeds(150, -150);
  for (int i = 0; i < CALIBRATION_SPINS; i++) {
    lineSensors.calibrate();
    delay(CALIBRATION_DELAY_MS);
  }
  motors.setSpeeds(0, 0);
  
  display.gotoXY(0, 2);
  display.print("Complete!");
  playSuccessSound();
  delay(500);
}

void calibrateProximitySensors() {
  display.clear();
  display.print("Calibrating");
  display.gotoXY(0, 1);
  display.print("Prox Sensors");
  
  prox.initThreeSensors();
  
  display.gotoXY(0, 2);
  display.print("Complete!");
  playSuccessSound();
  delay(500);
}

void showSensorReadings() {
  lineSensors.read(lineValues);
  prox.read();
  
  display.clear();
  display.print("L:");
  display.print(lineValues[0]);
  display.print(" ");
  display.print(lineValues[1]);
  
  display.gotoXY(0, 1);
  display.print("M:");
  display.print(lineValues[2]);
  
  display.gotoXY(0, 2);
  display.print("R:");
  display.print(lineValues[3]);
  display.print(" ");
  display.print(lineValues[4]);
  
  display.gotoXY(0, 3);
  display.print("P:");
  display.print(prox.countsLeftWithLeftLeds());
  display.print(" ");
  display.print(prox.countsFrontWithLeftLeds());
  display.print(" ");
  display.print(prox.countsRightWithRightLeds());
}

void performSystemCheck() {
  display.clear();
  display.print("System Check");
  
  // Test motors
  display.gotoXY(0, 1);
  display.print("Motors...");
  motors.setSpeeds(100, 100);
  delay(500);
  motors.setSpeeds(-100, -100);
  delay(500);
  motors.setSpeeds(0, 0);
  
  // Test buzzer
  display.gotoXY(0, 2);
  display.print("Buzzer...");
  buzzer.play("c32");
  delay(500);
  
  display.gotoXY(0, 3);
  display.print("Complete!");
  playSuccessSound();
  delay(1000);
}

// ============ MATH HELPERS ============
int mapRange(int value, int fromLow, int fromHigh, int toLow, int toHigh) {
  return (value - fromLow) * (toHigh - toLow) / (fromHigh - fromLow) + toLow;
}

float constrainFloat(float value, float min, float max) {
  if (value < min) return min;
  if (value > max) return max;
  return value;
}

// ============ TIMING HELPERS ============
bool isTimeoutReached(unsigned long startTime, unsigned long timeoutMs) {
  return (millis() - startTime) > timeoutMs;
}

void delayWithButtonCheck(unsigned long delayMs) {
  unsigned long startTime = millis();
  while (!isTimeoutReached(startTime, delayMs)) {
    if (buttonB.isPressed()) break;
    delay(10);
  }
}

// ============ ENCODER HELPERS ============
void resetEncoders() {
  encoders.getCountsAndResetLeft();
  encoders.getCountsAndResetRight();
}

void driveTicks(int ticks, int speed) {
  resetEncoders();
  while (encoders.getCountsLeft() < ticks && encoders.getCountsRight() < ticks) {
    motors.setSpeeds(speed, speed);
    if (buttonB.isPressed()) break;
  }
  motors.setSpeeds(0, 0);
  delay(200);
}

void turnRight90() {
  resetEncoders();
  while (abs(encoders.getCountsLeft()) < TURN_90_TICKS) {
    motors.setSpeeds(MAZE_TURN_SPEED, -MAZE_TURN_SPEED);
    if (buttonB.isPressed()) break;
  }
  motors.setSpeeds(0, 0);
  delay(200);
}

void turnLeft90() {
  resetEncoders();
  while (abs(encoders.getCountsLeft()) < TURN_90_TICKS) {
    motors.setSpeeds(-MAZE_TURN_SPEED, MAZE_TURN_SPEED);
    if (buttonB.isPressed()) break;
  }
  motors.setSpeeds(0, 0);
  delay(200);
}

void turn180() {
  resetEncoders();
  while (abs(encoders.getCountsLeft()) < TURN_180_TICKS) {
    motors.setSpeeds(MAZE_TURN_SPEED, -MAZE_TURN_SPEED);
    if (buttonB.isPressed()) break;
  }
  motors.setSpeeds(0, 0);
  delay(200);
}