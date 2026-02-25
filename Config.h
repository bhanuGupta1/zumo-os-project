/*
 * Config.h - Configuration Constants
 * 
 * This file contains all tunable parameters for the robot system.
 * Centralizing constants here makes it easy to adjust robot behavior
 * without modifying the main code.
 * 
 * Categories:
 * - Motor speeds for different behaviors
 * - PID control constants for line following
 * - Sensor thresholds for detection
 * - Timing parameters
 * - Encoder calibration values
 * - Maze navigation settings
 */

#pragma once

// ============ MOTOR SPEEDS ============
// Motor speed values range from -400 to 400
// Higher values = faster movement, but less control
// Lower values = slower but more precise

#define LINE_BASE_SPEED 220        // Base speed for line following
#define ENCODER_SPEED 250          // Speed for encoder-based movement (ballistic behaviors)
#define OBSTACLE_SPEED 200         // Speed when avoiding obstacles
#define WALL_FOLLOW_SPEED 180      // Speed for wall following mode
#define SUMO_SPEED 300             // Maximum speed for sumo mode

// ============ PID CONSTANTS ============
// PID (Proportional-Integral-Derivative) control for smooth line following
// Kp: Proportional gain - how strongly to correct errors
// Kd: Derivative gain - dampens oscillations

#define LINE_KP 0.15f              // Proportional constant for line following
#define LINE_KD 2.0f               // Derivative constant for line following

// ============ SENSOR THRESHOLDS ============
// Threshold values determine when sensors detect objects/lines

#define OBSTACLE_THRESHOLD 3       // Proximity sensor threshold for obstacle detection
#define WALL_DISTANCE_TARGET 5     // Target distance from wall (for wall following)
#define LINE_BLACK_THRESHOLD 500   // Line sensor threshold (above = black line detected)

// ============ TIMING ============
// Time-based parameters in milliseconds

#define MENU_TIMEOUT_MS 10000      // Menu auto-timeout (10 seconds)
#define BUTTON_DEBOUNCE_MS 50      // Button debounce delay
#define SENSOR_READ_INTERVAL_MS 20 // How often to read sensors

// ============ ENCODER CONSTANTS ============
// Encoder tick counts for precise turns and movements
// These values are calibrated for the Zumo32U4 robot
// May need adjustment based on battery level and surface

#define TURN_90_TICKS 320          // Encoder ticks for 90-degree turn
#define TURN_180_TICKS 640         // Encoder ticks for 180-degree turn
#define SQUARE_SIDE_TICKS 900      // Encoder ticks for square side length

// ============ MAZE SETTINGS ============
// Parameters for maze navigation modes

#define MAZE_WALL_THRESHOLD 3      // Proximity threshold for maze walls
#define MAZE_TURN_SPEED 250        // Speed when turning in maze
#define MAZE_FORWARD_SPEED 220     // Speed when driving forward in maze

// ============ CALIBRATION ============
// Parameters for sensor calibration routines

#define CALIBRATION_SPINS 80       // Number of calibration readings to take
#define CALIBRATION_DELAY_MS 20    // Delay between calibration readings
