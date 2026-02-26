/*
 * Config.h - Configuration Constants
 * 
 * ========== WHAT THIS FILE DOES ==========
 * This file contains all tunable parameters and constants for the robot.
 * Centralizing configuration here makes it easy to adjust robot behavior
 * without modifying the main code in multiple places.
 * 
 * ========== WHY USE #define ==========
 * #define creates compile-time constants that:
 * - Use no RAM (replaced during compilation)
 * - Cannot be changed during runtime
 * - Make code more readable (names instead of magic numbers)
 * - Easy to adjust in one place
 * 
 * ========== HOW TO ADJUST ROBOT BEHAVIOR ==========
 * Change values in this file to tune robot performance:
 * - Increase speeds for faster movement (max 400, ludicrous 500)
 * - Adjust encoder ticks if turns are not accurate
 * - Modify PID constants if line following is wobbly
 * - Change thresholds if sensors are too/not sensitive enough
 * 
 * ========== CATEGORIES ==========
 * - Motor speeds: How fast robot moves in different modes
 * - PID constants: Line following smoothness and accuracy
 * - Sensor thresholds: When sensors detect objects/lines
 * - Timing: Delays and timeouts
 * - Encoder calibration: Distance and angle measurements
 * - Maze settings: Parameters for maze navigation
 */

#pragma once

// ============ MOTOR SPEEDS ============
// Motor speed values range from -400 to 400 (safe limit)
// Current implementation allows up to 500 (LUDICROUS MODE!)
// 
// HOW TO ADJUST:
// - Higher values = faster movement, but less control and accuracy
// - Lower values = slower but more precise and stable
// - Negative values = reverse direction
// 
// IMPORTANT: Battery level affects actual speed
// - Fresh batteries: Robot moves faster at same speed value
// - Low batteries: Robot moves slower, may need higher values

#define LINE_BASE_SPEED 220        // Base speed for line following
                                   // Used as target speed, PID adjusts from this
                                   
#define ENCODER_SPEED 250          // Speed for encoder-based movement
                                   // Used in ballistic behaviors for straight lines
                                   
#define OBSTACLE_SPEED 200         // Speed when avoiding obstacles
                                   // Slower for better reaction time
                                   
#define WALL_FOLLOW_SPEED 180      // Speed for wall following mode
                                   // Slow for precise distance control
                                   
#define SUMO_SPEED 300             // Maximum speed for sumo mode
                                   // Fast for aggressive pushing

// ============ PID CONSTANTS ============
// PID (Proportional-Integral-Derivative) control for smooth line following
// 
// WHAT IS PID?
// PID is a control algorithm that calculates corrections to keep robot
// centered on a line. It uses three components:
// - P (Proportional): Corrects based on current error
// - I (Integral): Corrects based on accumulated past errors (not used here)
// - D (Derivative): Corrects based on rate of error change
// 
// HOW TO TUNE:
// - Kp too high: Robot oscillates (wobbles) on line
// - Kp too low: Robot responds slowly, may lose line
// - Kd too high: Robot moves sluggishly
// - Kd too low: Robot overshoots and oscillates
// 
// TUNING PROCESS:
// 1. Set Kd = 0, adjust Kp until robot follows line with slight wobble
// 2. Increase Kd to dampen wobble and smooth movement
// 3. Fine-tune both for best performance

#define LINE_KP 0.15f              // Proportional constant for line following
                                   // Controls how strongly robot corrects errors
                                   // Typical range: 0.1 to 0.3
                                   
#define LINE_KD 2.0f               // Derivative constant for line following
                                   // Dampens oscillations and smooths movement
                                   // Typical range: 1.0 to 5.0

// ============ SENSOR THRESHOLDS ============
// Threshold values determine when sensors detect objects/lines
// 
// HOW SENSORS WORK:
// - Line sensors: Return 0-2000 (0=very bright, 2000=very dark)
// - Proximity sensors: Return 0-6 (0=nothing, 6=very close object)
// 
// HOW TO ADJUST:
// - If sensor too sensitive: Increase threshold
// - If sensor not sensitive enough: Decrease threshold
// - Test on actual surface/lighting conditions

#define OBSTACLE_THRESHOLD 3       // Proximity sensor threshold for obstacle detection
                                   // Values above this = obstacle detected
                                   // Range: 0-6, typical: 2-4
                                   
#define WALL_DISTANCE_TARGET 5     // Target distance from wall (for wall following)
                                   // Robot tries to maintain this distance
                                   // Range: 3-6, higher = closer to wall
                                   
#define LINE_BLACK_THRESHOLD 500   // Line sensor threshold
                                   // Values above this = black line detected
                                   // Range: 0-2000, typical: 300-700
                                   // Depends on line darkness and lighting

// ============ TIMING ============
// Time-based parameters in milliseconds (1000 ms = 1 second)
// 
// HOW TO ADJUST:
// - Increase for longer delays/timeouts
// - Decrease for faster response/shorter waits

#define MENU_TIMEOUT_MS 10000      // Menu auto-timeout (10 seconds)
                                   // How long menu waits before auto-selecting
                                   // Set to 0 to disable timeout
                                   
#define BUTTON_DEBOUNCE_MS 50      // Button debounce delay (50 milliseconds)
                                   // Prevents multiple triggers from one press
                                   // Typical range: 20-100 ms
                                   
#define SENSOR_READ_INTERVAL_MS 20 // How often to read sensors (20 ms = 50 Hz)
                                   // Faster = more responsive, but more CPU usage
                                   // Typical range: 10-50 ms

// ============ ENCODER CONSTANTS ============
// Encoder tick counts for precise turns and movements
// 
// WHAT ARE ENCODER TICKS?
// Encoders count wheel rotations using magnetic sensors. Each "tick"
// represents a small rotation increment. More ticks = more distance/angle.
// 
// HOW TO CALIBRATE:
// 1. Run robot and measure actual turn angle
// 2. If turn is too small: Increase tick value
// 3. If turn is too large: Decrease tick value
// 4. Repeat until accurate
// 
// FACTORS AFFECTING ACCURACY:
// - Battery level (low battery = less power = smaller turns)
// - Surface friction (carpet vs smooth floor)
// - Motor speed (faster = less accurate)
// - Wheel slippage
// 
// THESE VALUES ARE CALIBRATED FOR:
// - Zumo32U4 robot
// - Fresh batteries
// - Smooth hard floor
// - Medium speed (200-300)
//
// NOTE: Values DOUBLED from original 320/640 because physical turns
// were measuring less than 90 degrees. Adjust based on testing.

#define TURN_90_TICKS 640          // Encoder ticks for 90-degree turn (DOUBLED for testing)
                                   // Used by turnRight90() and turnLeft90()
                                   // Original value was 320, doubled to 640
                                   // Adjust if robot turns more/less than 90°
                                   
#define TURN_180_TICKS 1280        // Encoder ticks for 180-degree turn (DOUBLED for testing)
                                   // Should be approximately 2 × TURN_90_TICKS
                                   // Original value was 640, doubled to 1280
                                   // Used by turn180() function
                                   
#define SQUARE_SIDE_TICKS 900      // Encoder ticks for square side length
                                   // Used in square patterns
                                   // Approximately 30cm at typical speed

// ============ MAZE SETTINGS ============
// Parameters for maze navigation modes (not used in ballistic behaviors)
// 
// These settings control how robot navigates through mazes using
// wall-following or other maze-solving algorithms

#define MAZE_WALL_THRESHOLD 3      // Proximity threshold for maze walls
                                   // Same as OBSTACLE_THRESHOLD typically
                                   
#define MAZE_TURN_SPEED 250        // Speed when turning in maze
                                   // Medium speed for accurate turns
                                   
#define MAZE_FORWARD_SPEED 220     // Speed when driving forward in maze
                                   // Moderate speed for good control

// ============ CALIBRATION ============
// Parameters for sensor calibration routines
// 
// WHAT IS CALIBRATION?
// Line sensors need to see both light and dark surfaces to establish
// their min/max values. Calibration spins robot to expose sensors to
// different surfaces.
// 
// HOW TO ADJUST:
// - More spins = better calibration, but takes longer
// - Fewer spins = faster, but may miss some surfaces

#define CALIBRATION_SPINS 80       // Number of calibration readings to take
                                   // More = better calibration
                                   // Typical range: 50-100
                                   
#define CALIBRATION_DELAY_MS 20    // Delay between calibration readings
                                   // Gives sensors time to settle
                                   // Typical range: 10-30 ms
