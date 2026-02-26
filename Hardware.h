/*
 * Hardware.h - Hardware Interface Declarations
 * 
 * ========== WHAT THIS FILE DOES ==========
 * This header file declares all hardware components and functions for
 * interfacing with the Zumo32U4 robot. It provides access to:
 * - Motors (for movement)
 * - Encoders (for measuring wheel rotation)
 * - Sensors (line sensors, proximity sensors, IMU)
 * - Display (OLED screen)
 * - Buttons (A, B, C for user input)
 * - Buzzer (for sound feedback)
 * 
 * ========== HOW TO USE ==========
 * Include this file in any .cpp file that needs hardware access:
 *   #include "Hardware.h"
 * 
 * Then you can use hardware objects like:
 *   motors.setSpeeds(200, 200);  // Drive forward
 *   display.print("Hello");       // Show text on screen
 *   buttonA.isPressed();          // Check if button A is pressed
 * 
 * ========== KEY CONCEPTS ==========
 * - "extern" keyword: Declares that these objects are defined elsewhere
 *   (in Hardware.cpp) but can be used in any file that includes this header
 * - Hardware abstraction: This file hides the complexity of hardware
 *   initialization and provides simple functions to control the robot
 */

#pragma once

#include <Wire.h>
#include <Zumo32U4.h>

// ============ HARDWARE INSTANCES ============
// These are the physical components of the Zumo32U4 robot
// Each object provides functions to control that component

extern Zumo32U4Motors motors;              // Controls left and right motors
                                           // Use: motors.setSpeeds(left, right)
                                           
extern Zumo32U4Encoders encoders;          // Reads wheel rotation counts
                                           // Use: encoders.getCountsLeft()
                                           
extern Zumo32U4LineSensors lineSensors;    // 5 IR sensors on bottom for line following
                                           // Use: lineSensors.read(lineValues)
                                           
extern Zumo32U4ProximitySensors prox;      // Front IR sensors for obstacle detection
                                           // Use: prox.read()
                                           
extern Zumo32U4OLED display;               // 128x64 OLED screen
                                           // Use: display.print("text")
                                           
extern Zumo32U4Buzzer buzzer;              // Speaker for sound effects
                                           // Use: buzzer.play("c4 e4 g4")
                                           
extern Zumo32U4ButtonA buttonA;            // Button A (left button)
extern Zumo32U4ButtonB buttonB;            // Button B (middle button)
extern Zumo32U4ButtonC buttonC;            // Button C (right button)
                                           // Use: buttonA.isPressed()
                                           
extern Zumo32U4IMU imu;                    // Inertial Measurement Unit (accelerometer + gyro)
                                           // Use: imu.read() for tilt/acceleration data

// ============ SENSOR DATA ============
// Global variables to store sensor readings

extern unsigned int lineValues[5];         // Array of 5 line sensor readings (0-2000)
                                           // Index 0 = leftmost, 4 = rightmost
                                           // Lower values = darker surface
                                           
extern int lastError;                      // Last line following error (-4 to +4)
                                           // Used for PID control in line following

// ============ HARDWARE FUNCTIONS ============
// These functions initialize and control hardware components

/*
 * initializeHardware() - Initialize all hardware components
 * MUST be called in setup() before using any hardware
 * Sets up motors, sensors, display, and buzzer
 */
void initializeHardware();

/*
 * playStartupSound() - Play sound when robot powers on
 * Provides audio feedback that system is ready
 */
void playStartupSound();

/*
 * playModeChangeSound() - Play sound when mode changes
 * Short beep to confirm mode selection
 */
void playModeChangeSound();

/*
 * playErrorSound() - Play sound for errors
 * Low tone to indicate something went wrong
 */
void playErrorSound();

/*
 * playSuccessSound() - Play sound for successful completion
 * Happy tone when task completes successfully
 */
void playSuccessSound();

// ============ MOTOR HELPERS ============
// Helper functions for safe motor control

/*
 * clampSpeed(speed) - Limit motor speed to safe range
 * 
 * @param speed: Desired motor speed (-500 to 500)
 * @return: Clamped speed (currently returns input directly - LUDICROUS MODE!)
 * 
 * NOTE: Normal safe range is -400 to 400
 * Current implementation allows up to 500 for maximum speed
 */
int clampSpeed(int speed);

/*
 * stopMotors() - Immediately stop both motors
 * Sets both motor speeds to 0
 * Use this for emergency stops or end of movement
 */
void stopMotors();

/*
 * setMotorSpeeds(left, right) - Set motor speeds with safety clamping
 * 
 * @param left: Left motor speed (-500 to 500)
 * @param right: Right motor speed (-500 to 500)
 * 
 * Automatically clamps speeds to safe range before applying
 */
void setMotorSpeeds(int left, int right);