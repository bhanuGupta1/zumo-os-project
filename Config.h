#pragma once

// ============================================================
//  ZumoOS V6 — Smooth Dead Zone + Spiral Search
// ============================================================

// --- Encoder & Movement ---
#define ENCODER_SPEED       250
#define TURN_90_TICKS       640
#define TURN_180_TICKS      1280
#define SQUARE_SIDE_TICKS   900
#define TURN_SPEED          250

// --- UI / Timing ---
#define MENU_TIMEOUT_MS     10000
#define BUTTON_DEBOUNCE_MS  50

// --- Calibration ---
#define CALIBRATION_SPINS      100
#define CALIBRATION_DELAY_MS   15
#define CALIBRATION_SPEED      120

// ============================================================
//  PID — Simple linear PD. The smooth dead zone handles the rest.
// ============================================================
#define LINE_KP             0.15f
#define LINE_KD             1.5f

// ============================================================
//  SMOOTH DEAD ZONE
//
//  Instead of: error < threshold → 0, error >= threshold → full
//  We use:     error smoothly ramps from 0% to 100% over a range
//
//  DEAD_INNER: errors below this → 0% correction (silent zone)
//  DEAD_OUTER: errors above this → 100% correction (full PD)
//  Between inner and outer → smooth linear ramp
//
//  This eliminates the boundary oscillation that causes jitter.
// ============================================================
#define DEAD_INNER          150       // Below this: zero correction
#define DEAD_OUTER          400       // Above this: full correction
                                      // 150–400: smooth ramp

// ============================================================
//  SPEED
// ============================================================
#define SPEED_STRAIGHT      220       // Faster cruise
#define SPEED_CURVE         80        // Tight curve minimum
#define SPEED_ERROR_THRESH  500       // Start slowing above this error
#define SPEED_REDUCE_RATE   0.10f

// ============================================================
//  CORNER HANDLING
// ============================================================
#define CORNER_SENSOR_THRESH 500
#define CORNER_BOOST         100      // Strong differential kick
#define CORNER_SPEED         80       // Slow into corners

// ============================================================
//  SENSOR FILTERING
// ============================================================
#define EMA_ALPHA           0.85f     // Light filter — less lag, less oscillation

// ============================================================
//  LINE LOSS
// ============================================================
#define LINE_LOST_THRESH    80

// ============================================================
//  SPIRAL SEARCH — Faster and wider
// ============================================================
#define SPIRAL_INNER_START   30       // Tighter initial spiral
#define SPIRAL_OUTER_START   180      // Faster outer wheel
#define SPIRAL_RAMP_RATE     3        // Expand faster (was 2)
#define SPIRAL_STEP_MS       25       // Faster steps (was 30)
#define SPIRAL_MAX_STEPS     90       // More steps before giving up
#define SPIRAL_BACKUP_MS     120      // Shorter backup
#define SPIRAL_BACKUP_SPD    90

// ============================================================
//  INTERSECTION
// ============================================================
#define INTERSECTION_THRESH  700
#define INTERSECTION_COUNT   4
#define INTERSECTION_BRAKE   130      // Faster through intersections

// ============================================================
//  MOTOR LIMITS
// ============================================================
#define MOTOR_MAX            300
#define MOTOR_MIN           -300

// ============================================================
//  DISPLAY
// ============================================================
#define DISPLAY_UPDATE_MS    300
