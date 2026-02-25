/*
 * Utils.h - Utility Function Declarations
 * 
 * This header file declares helper functions used throughout the project.
 * These functions provide common functionality like sensor calibration,
 * encoder-based movement, and timing utilities.
 * 
 * Categories:
 * - Utility functions: Calibration, system checks
 * - Math helpers: Mapping and constraining values
 * - Timing helpers: Timeouts and delays
 * - Encoder helpers: Precise movement and turning
 */

#pragma once
#include <Arduino.h>

// ============ UTILITY FUNCTIONS ============

/*
 * waitForButtonPress() - Wait for any button to be pressed
 * Blocks execution until user presses and releases a button
 */
void waitForButtonPress();

/*
 * calibrateLineSensors() - Calibrate line sensors
 * Spins the robot to expose sensors to light and dark surfaces
 * Establishes min/max values for accurate line detection
 */
void calibrateLineSensors();

/*
 * calibrateProximitySensors() - Initialize proximity sensors
 * Sets up the IR proximity sensors for obstacle detection
 */
void calibrateProximitySensors();

/*
 * showSensorReadings() - Display live sensor values on OLED
 * Shows line sensor and proximity sensor readings for debugging
 */
void showSensorReadings();

/*
 * performSystemCheck() - Test all robot systems
 * Verifies motors, sensors, and buzzer are working correctly
 */
void performSystemCheck();

// ============ MATH HELPERS ============

/*
 * mapRange() - Map a value from one range to another
 * Similar to Arduino's map() function
 * 
 * @param value: Input value to map
 * @param fromLow: Lower bound of input range
 * @param fromHigh: Upper bound of input range
 * @param toLow: Lower bound of output range
 * @param toHigh: Upper bound of output range
 * @return: Mapped value in output range
 */
int mapRange(int value, int fromLow, int fromHigh, int toLow, int toHigh);

/*
 * constrainFloat() - Limit a float value to a range
 * 
 * @param value: Value to constrain
 * @param min: Minimum allowed value
 * @param max: Maximum allowed value
 * @return: Constrained value
 */
float constrainFloat(float value, float min, float max);

// ============ TIMING HELPERS ============

/*
 * isTimeoutReached() - Check if a timeout has elapsed
 * 
 * @param startTime: Start time in milliseconds (from millis())
 * @param timeoutMs: Timeout duration in milliseconds
 * @return: true if timeout elapsed, false otherwise
 */
bool isTimeoutReached(unsigned long startTime, unsigned long timeoutMs);

/*
 * delayWithButtonCheck() - Delay with ability to interrupt
 * Like delay() but can be interrupted by Button B press
 * 
 * @param delayMs: Delay duration in milliseconds
 */
void delayWithButtonCheck(unsigned long delayMs);

// ============ ENCODER HELPERS ============
// These functions provide precise movement control using wheel encoders
// Encoders count wheel rotations for accurate distance and angle control

/*
 * resetEncoders() - Reset encoder counts to zero
 * Call before starting encoder-based movement
 */
void resetEncoders();

/*
 * driveDistance() - Drive a specific distance
 * 
 * @param distance: Distance to drive (in encoder ticks)
 * @param speed: Motor speed (-400 to 400)
 */
void driveDistance(int distance, int speed);

/*
 * turnAngle() - Turn a specific angle
 * 
 * @param angle: Angle to turn in degrees
 * @param speed: Turn speed
 */
void turnAngle(int angle, int speed);

/*
 * driveTicks() - Drive forward for specified encoder ticks
 * Used by ballistic behaviors for precise straight-line movement
 * 
 * @param ticks: Number of encoder ticks to drive
 * @param speed: Motor speed (both wheels same speed)
 */
void driveTicks(int ticks, int speed);

/*
 * turnRight90() - Turn exactly 90 degrees clockwise
 * Uses TURN_90_TICKS constant from Config.h
 */
void turnRight90();

/*
 * turnLeft90() - Turn exactly 90 degrees counter-clockwise
 * Uses TURN_90_TICKS constant from Config.h
 */
void turnLeft90();

/*
 * turn180() - Turn exactly 180 degrees
 * Uses TURN_180_TICKS constant from Config.h
 */
void turn180();
