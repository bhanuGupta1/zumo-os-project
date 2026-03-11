#pragma once
#include <Arduino.h>

void waitForButtonPress();
void calibrateLineSensors();
void calibrateProximitySensors();
void showSensorReadings();
void performSystemCheck();
int mapRange(int value, int fromLow, int fromHigh, int toLow, int toHigh);
float constrainFloat(float value, float min, float max);
bool isTimeoutReached(unsigned long startTime, unsigned long timeoutMs);
void delayWithButtonCheck(unsigned long delayMs);
void resetEncoders();
void driveDistance(int distance, int speed);
void turnAngle(int angle, int speed);
void driveTicks(int ticks, int speed);
void turnRight90();
void turnLeft90();
void turn180();
