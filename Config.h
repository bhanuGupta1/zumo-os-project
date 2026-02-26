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
 * 
 * ========== CATEGORIES ==========
 * - Motor speeds: How fast robot moves in different modes
 * - Timing: Delays and timeouts
 * - Encoder calibration: Distance and angle measurements
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

#define ENCODER_SPEED 250
// Speed for encoder-based movement
// Used in ballistic behaviors for straight lines

// ============ TIMING ============
// Time-based parameters in milliseconds (1000 ms = 1 second)
// 
// HOW TO ADJUST:
// - Increase for longer delays/timeouts
// - Decrease for faster response/shorter waits

#define MENU_TIMEOUT_MS 10000
// Menu auto-timeout (10 seconds)
// How long menu waits before auto-selecting
// Set to 0 to disable timeout

#define BUTTON_DEBOUNCE_MS 50
// Button debounce delay (50 milliseconds)
// Prevents multiple triggers from one press
// Typical range: 20-100 ms

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

#define TURN_90_TICKS 640
// Encoder ticks for 90-degree turn (DOUBLED for testing)
// Used by turnRight90() and turnLeft90()
// Original value was 320, doubled to 640
// Adjust if robot turns more/less than 90°

#define TURN_180_TICKS 1280
// Encoder ticks for 180-degree turn (DOUBLED for testing)
// Should be approximately 2 × TURN_90_TICKS
// Original value was 640, doubled to 1280
// Used by turn180() function

#define SQUARE_SIDE_TICKS 900
// Encoder ticks for square side length
// Used in square patterns
// Approximately 30cm at typical speed

// ============ TURN SPEED ============
// Speed used for turning operations

#define TURN_SPEED 250
// Speed when turning (used by turnRight90, turnLeft90, turn180)
// Medium speed for accurate turns
// Adjust if turns are not precise

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

#define CALIBRATION_SPINS 80
// Number of calibration readings to take
// More = better calibration
// Typical range: 50-100

#define CALIBRATION_DELAY_MS 20
// Delay between calibration readings
// Gives sensors time to settle
// Typical range: 10-30 ms
