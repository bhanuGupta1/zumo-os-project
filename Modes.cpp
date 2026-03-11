#include "Modes.h"
#include "Hardware.h"
#include "Config.h"
#include "Utils.h"

// ============================================================
//  ZumoOS V6 — Smooth Dead Zone + Spiral Search
//
//  V5 jitter cause: hard dead zone boundary at ±200
//    error=199 → correction=0
//    error=201 → correction=JUMP
//    Robot oscillates across boundary = jitter
//
//  V6 fix: SMOOTH DEAD ZONE
//    error < 150  → correction = 0     (silent)
//    error 150-400 → correction ramps 0% to 100% (smooth)
//    error > 400  → correction = 100%  (full PD)
//    No boundary to oscillate across = no jitter
// ============================================================

void runMode(Mode m) {
  switch (m) {
    case MODE_LINE_FOLLOW: modeLineFollow(); break;
    default: break;
  }
}

// ========== STATE ==========
static int   prevError     = 0;
static float filteredPos   = 2000.0f;
static int   lastDirection = 0;

// ============================================================
//  CALIBRATION
// ============================================================
static void calibrate3Pass() {
  display.clear();
  display.print("Calibrating");
  display.gotoXY(0, 1);
  display.print("[1/3] >>>");

  lineSensors.initFiveSensors();

  motors.setSpeeds(CALIBRATION_SPEED, -CALIBRATION_SPEED);
  for (int i = 0; i < CALIBRATION_SPINS / 3; i++) {
    lineSensors.calibrate();
    delay(CALIBRATION_DELAY_MS);
  }

  display.gotoXY(0, 1);
  display.print("[2/3] <<<");
  motors.setSpeeds(-CALIBRATION_SPEED, CALIBRATION_SPEED);
  for (int i = 0; i < (CALIBRATION_SPINS * 2) / 3; i++) {
    lineSensors.calibrate();
    delay(CALIBRATION_DELAY_MS);
  }

  display.gotoXY(0, 1);
  display.print("[3/3] >>>");
  motors.setSpeeds(CALIBRATION_SPEED, -CALIBRATION_SPEED);
  for (int i = 0; i < CALIBRATION_SPINS / 3; i++) {
    lineSensors.calibrate();
    delay(CALIBRATION_DELAY_MS);
  }

  motors.setSpeeds(0, 0);
  playCalibrationDone();
  display.clear();
  display.print("Cal Done!");
  delay(500);
}

// ============================================================
//  SENSOR HELPERS
// ============================================================
static unsigned int sensorSum() {
  unsigned int s = 0;
  for (int i = 0; i < 5; i++) s += lineValues[i];
  return s;
}

static bool isLineLost() {
  return sensorSum() < LINE_LOST_THRESH;
}

static bool isIntersection() {
  int count = 0;
  for (int i = 0; i < 5; i++) {
    if (lineValues[i] > INTERSECTION_THRESH) count++;
  }
  return count >= INTERSECTION_COUNT;
}

static bool isOnLeftEdge() {
  return lineValues[0] > CORNER_SENSOR_THRESH;
}

static bool isOnRightEdge() {
  return lineValues[4] > CORNER_SENSOR_THRESH;
}

// ============================================================
//  SMOOTH DEAD ZONE SCALING
//
//  Returns a value between 0.0 and 1.0:
//    |error| < DEAD_INNER  → 0.0 (no correction)
//    |error| > DEAD_OUTER  → 1.0 (full correction)
//    between              → linear ramp 0.0 to 1.0
//
//  This is multiplied into the PD output so the correction
//  smoothly fades in instead of snapping on.
// ============================================================
static float deadZoneScale(int error) {
  int absErr = abs(error);
  if (absErr <= DEAD_INNER) return 0.0f;
  if (absErr >= DEAD_OUTER) return 1.0f;
  // Linear ramp between inner and outer
  return (float)(absErr - DEAD_INNER) / (float)(DEAD_OUTER - DEAD_INNER);
}

// ============================================================
//  STEERING — Simple PD × smooth dead zone scale
// ============================================================
static float computeSteering(int error, int errorDelta) {
  float scale = deadZoneScale(error);

  // If scale is 0, return 0 — no correction at all
  if (scale <= 0.0f) return 0.0f;

  // Standard PD
  float pTerm = LINE_KP * (float)error;
  float dTerm = LINE_KD * (float)errorDelta;

  // Scale the whole output by the smooth ramp
  return (pTerm + dTerm) * scale;
}

// ============================================================
//  ADAPTIVE SPEED
// ============================================================
static int computeSpeed(int error) {
  int absErr = abs(error);
  if (absErr < SPEED_ERROR_THRESH) return SPEED_STRAIGHT;
  float reduction = (float)(absErr - SPEED_ERROR_THRESH) * SPEED_REDUCE_RATE;
  int speed = SPEED_STRAIGHT - (int)reduction;
  if (speed < SPEED_CURVE) speed = SPEED_CURVE;
  return speed;
}

// ============================================================
//  CORNER BOOST
// ============================================================
static void applyCornerBoost(int* leftSpeed, int* rightSpeed) {
  bool leftEdge  = isOnLeftEdge();
  bool rightEdge = isOnRightEdge();

  if (leftEdge && !rightEdge) {
    *leftSpeed  -= CORNER_BOOST;
    *rightSpeed += CORNER_BOOST;
  }
  if (rightEdge && !leftEdge) {
    *leftSpeed  += CORNER_BOOST;
    *rightSpeed -= CORNER_BOOST;
  }
}

// ============================================================
//  SPIRAL SEARCH RECOVERY
//
//  1. Back up briefly
//  2. Spiral outward in lastDirection
//     - Outer wheel constant fast, inner wheel starts slow
//     - Inner wheel ramps up → spiral expands
//  3. If fails, back up + spiral opposite direction
// ============================================================
static bool spiralSearch(int direction) {
  int innerSpeed = SPIRAL_INNER_START;
  int outerSpeed = SPIRAL_OUTER_START;

  for (int step = 0; step < SPIRAL_MAX_STEPS; step++) {
    if (direction > 0) {
      motors.setSpeeds(outerSpeed, innerSpeed);
    } else {
      motors.setSpeeds(innerSpeed, outerSpeed);
    }

    delay(SPIRAL_STEP_MS);

    lineSensors.readLine(lineValues);
    if (!isLineLost()) {
      motors.setSpeeds(0, 0);
      return true;
    }

    if (buttonB.isPressed()) {
      motors.setSpeeds(0, 0);
      return false;
    }

    innerSpeed += SPIRAL_RAMP_RATE;
    if (innerSpeed > outerSpeed) innerSpeed = outerSpeed;
  }

  motors.setSpeeds(0, 0);
  return false;
}

static bool recoverLine() {
  motors.setSpeeds(0, 0);

  display.clear();
  display.print("SPIRAL");
  display.gotoXY(0, 1);
  display.print("SEARCH...");

  // Back up
  motors.setSpeeds(-SPIRAL_BACKUP_SPD, -SPIRAL_BACKUP_SPD);
  unsigned long t = millis();
  while ((millis() - t) < SPIRAL_BACKUP_MS) {
    lineSensors.readLine(lineValues);
    if (!isLineLost()) { motors.setSpeeds(0, 0); return true; }
    delay(5);
  }
  motors.setSpeeds(0, 0);
  delay(20);

  // Spiral in last known direction
  int searchDir = (lastDirection >= 0) ? 1 : -1;

  display.gotoXY(0, 1);
  display.print(searchDir > 0 ? "Spiral R  " : "Spiral L  ");

  if (spiralSearch(searchDir)) {
    playSuccessSound();
    return true;
  }

  // Failed — back up and try opposite
  motors.setSpeeds(-SPIRAL_BACKUP_SPD, -SPIRAL_BACKUP_SPD);
  t = millis();
  while ((millis() - t) < SPIRAL_BACKUP_MS) {
    lineSensors.readLine(lineValues);
    if (!isLineLost()) { motors.setSpeeds(0, 0); return true; }
    delay(5);
  }
  motors.setSpeeds(0, 0);
  delay(20);

  display.gotoXY(0, 1);
  display.print(searchDir > 0 ? "Spiral L  " : "Spiral R  ");

  if (spiralSearch(-searchDir)) {
    playSuccessSound();
    return true;
  }

  motors.setSpeeds(0, 0);
  playErrorSound();
  return false;
}

// ============================================================
//  RESET
// ============================================================
static void resetState() {
  prevError   = 0;
  filteredPos = 2000.0f;
}

// ============================================================
//  MAIN LOOP
// ============================================================
void modeLineFollow() {

  calibrate3Pass();

  display.clear();
  display.print("V6 Smooth");
  display.gotoXY(0, 1);
  display.print("Starting...");
  delay(800);

  resetState();
  lastDirection = 0;
  unsigned long lastDisplayTime = 0;

  display.clear();
  display.print("Running...");

  while (true) {

    // Emergency stop
    if (buttonB.isPressed()) {
      motors.setSpeeds(0, 0);
      display.clear();
      display.print("PAUSED");
      display.gotoXY(0, 1);
      display.print("B = Resume");
      while (buttonB.isPressed()) delay(10);
      while (!buttonB.getSingleDebouncedPress()) delay(50);
      resetState();
      display.clear();
      continue;
    }

    // Read sensors
    int rawPos = lineSensors.readLine(lineValues);

    // LINE LOST → spiral search
    if (isLineLost()) {
      if (!recoverLine()) {
        display.clear();
        display.print("LOST LINE");
        display.gotoXY(0, 1);
        display.print("B=Recalib");
        while (!buttonB.getSingleDebouncedPress()) delay(50);
        calibrate3Pass();
        resetState();
        display.clear();
        display.print("Running...");
        continue;
      }
      rawPos = lineSensors.readLine(lineValues);
      resetState();
      filteredPos = (float)rawPos;
    }

    // Intersection → straight through
    if (isIntersection()) {
      motors.setSpeeds(INTERSECTION_BRAKE, INTERSECTION_BRAKE);
      delay(50);
      continue;
    }

    // Corner detection: bypass filter for instant response
    bool onEdge = isOnLeftEdge() || isOnRightEdge();
    if (onEdge) {
      filteredPos = (float)rawPos;
    } else {
      filteredPos = (EMA_ALPHA * (float)rawPos) + ((1.0f - EMA_ALPHA) * filteredPos);
    }
    int position = (int)filteredPos;

    // Error
    int error = position - 2000;

    // Track direction for spiral search
    if (error > 80)       lastDirection =  1;
    else if (error < -80) lastDirection = -1;

    // Steering (PD × smooth dead zone)
    int errorDelta = error - prevError;
    float steer = computeSteering(error, errorDelta);
    prevError = error;

    // Speed
    int baseSpeed = computeSpeed(error);
    if (onEdge) baseSpeed = CORNER_SPEED;

    // Motor speeds
    int leftSpeed  = baseSpeed + (int)steer;
    int rightSpeed = baseSpeed - (int)steer;

    // Corner boost on top
    if (onEdge) applyCornerBoost(&leftSpeed, &rightSpeed);

    // Clamp and drive
    leftSpeed  = clampSpeed(leftSpeed);
    rightSpeed = clampSpeed(rightSpeed);
    motors.setSpeeds(leftSpeed, rightSpeed);

    // Telemetry
    if ((millis() - lastDisplayTime) > DISPLAY_UPDATE_MS) {
      lastDisplayTime = millis();
      display.clear();
      if (onEdge) {
        display.print("CORNER");
      } else {
        display.print("E:");
        display.print(error);
      }
      display.print(" S:");
      display.print(baseSpeed);
      display.gotoXY(0, 1);
      display.print(leftSpeed);
      display.print("  ");
      display.print(rightSpeed);
    }
  }
}
