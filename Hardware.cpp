#include "Hardware.h"
#include "Config.h"

// ============ HARDWARE INSTANCES ============
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

// ============ SENSOR DATA ============
unsigned int lineValues[5];
int lastError = 0;

// ============ HARDWARE FUNCTIONS ============
void initializeHardware() {
  // Initialize display
  display.clear();
  display.print("ZumoOS Pro");
  display.gotoXY(0, 1);
  display.print("Initializing");
  
  // Initialize IMU
  Wire.begin();
  if (imu.init()) {
    imu.enableDefault();
  }
  
  // Initialize sensors (will be done per mode as needed)
  // lineSensors.initFiveSensors();
  // prox.initThreeSensors();
  
  delay(1000);
  display.clear();
}

void playStartupSound() {
  buzzer.play("c32 e32 g32 c64");
}

void playModeChangeSound() {
  buzzer.play("g32 c32");
}

void playErrorSound() {
  buzzer.play("c16 c16 c16");
}

void playSuccessSound() {
  buzzer.play("g32 a32 b32 c64");
}

// ============ MOTOR HELPERS ============
int clampSpeed(int speed) {
  // LUDICROUS SPEED MODE - No limits!
  // WARNING: Speeds above 400 may damage motors or cause loss of control
  return speed;  // No clamping!
}

void stopMotors() {
  motors.setSpeeds(0, 0);
}

void setMotorSpeeds(int left, int right) {
  motors.setSpeeds(left, right);  // Direct control, no clamping!
}