#pragma once

#include <Wire.h>
#include <Zumo32U4.h>

// --- Hardware objects ---
extern Zumo32U4Motors motors;
extern Zumo32U4Encoders encoders;
extern Zumo32U4LineSensors lineSensors;
extern Zumo32U4ProximitySensors prox;
extern Zumo32U4OLED display;
extern Zumo32U4Buzzer buzzer;
extern Zumo32U4ButtonA buttonA;
extern Zumo32U4ButtonB buttonB;
extern Zumo32U4ButtonC buttonC;
extern Zumo32U4IMU imu;

// --- Shared data ---
extern unsigned int lineValues[5];
extern int lastError;

// --- Hardware functions ---
void initializeHardware();
void playStartupSound();
void playModeChangeSound();
void playErrorSound();
void playSuccessSound();
void playCalibrationDone();
int  clampSpeed(int speed);
void stopMotors();
void setMotorSpeeds(int left, int right);
