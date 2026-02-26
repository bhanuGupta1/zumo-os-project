/*
 * Modes.cpp - Ballistic Behavior Implementations
 * 
 * ========== WHAT THIS FILE DOES ==========
 * This file contains all 4 ballistic behaviors for the SBA assignment:
 * 1. Square Spiral - Spirals inward to center with 90° turns
 * 2. Zig-Zag - Alternating diagonal movements
 * 3. Square Zig-Zag - Lawn mowing pattern with parallel rows
 * 4. Circle - Continuous circular path
 * 
 * ========== HOW TO ADJUST BEHAVIORS ==========
 * Each mode has an "ADJUSTABLE PARAMETERS" section at the top.
 * Look for these variables to customize the behavior:
 * 
 * SPEED:
 * - Change "speed" variable (0-500)
 * - Safe limit: 400, Ludicrous mode: 500
 * - Higher = faster, Lower = more controlled
 * 
 * DISTANCE/SIZE:
 * - Change "sideLength", "segmentLength", "rowLength" variables
 * - Measured in encoder ticks (roughly 1 tick = 0.1mm)
 * - Higher = larger pattern, Lower = smaller pattern
 * 
 * ANGLES:
 * - Turns use turnRight90() and turnLeft90() for 90° turns
 * - For zig-zag angles, adjust "TURN_90_TICKS + 100" value
 * 
 * CIRCLE DIAMETER:
 * - Adjust "outerSpeed" and "innerSpeed" difference
 * - Larger difference = tighter circle (smaller diameter)
 * - Smaller difference = wider circle (larger diameter)
 * 
 * ========== KEY CONCEPTS ==========
 * - Encoder ticks: Measure wheel rotation for accurate distance control
 * - Motor speed control: Set left/right motor speeds independently
 * - Timing-based movement: Use millis() for time-based patterns
 */

#include "Modes.h"      // Mode definitions and function declarations
#include "Hardware.h"   // Access to motors, encoders, display, buttons
#include "Config.h"     // Speed and distance constants
#include "Utils.h"      // Helper functions (driveTicks, turnRight90, etc.)

/*
 * runMode() - Mode Dispatcher
 * 
 * This function acts as a switch/router that calls the appropriate
 * ballistic behavior function based on the selected mode.
 * 
 * @param m: The mode enum value selected by the user
 * 
 * Implementation: Uses a switch statement for efficient mode selection
 */
void runMode(Mode m) {
  switch(m) {
    case MODE_SQUARE_SPIRAL: modeSquareSpiral(); break;
    case MODE_ZIGZAG: modeZigzag(); break;
    case MODE_SQUARE_ZIGZAG: modeSquareZigzag(); break;
    case MODE_CIRCLE: modeCircle(); break;
    default: break;  // Safety: do nothing if invalid mode
  }
}

// ============ BALLISTIC BEHAVIORS (SBA) ============

/*
 * modeSquareSpiral() - Square Spiral Pattern Implementation
 * 
 * Behavior: Robot drives in a square spiral pattern, starting large
 * and gradually spiraling inward to the center. Each side is shorter
 * than the previous, creating a tighter spiral.
 * 
 * Algorithm:
 * 1. Drive straight for current side length
 * 2. Turn 90 degrees right
 * 3. Decrease side length
 * 4. Repeat until reaching center
 * 
 * Key Features:
 * - Clean 90° turns using encoder-based turnRight90()
 * - Straight lines using encoder-based driveTicks()
 * - Tight spacing between spiral loops
 * - Spirals inward to center point
 * - User can stop anytime with Button B
 */
void modeSquareSpiral() {
  // Display mode name on OLED
  display.clear();
  display.print("LUDICROUS!");
  delay(500);
  
  // ========== ADJUSTABLE PARAMETERS ==========
  // HOW TO CHANGE THE SPIRAL:
  
  int sideLength = 1200;  // Starting side length (in encoder ticks)
                          // INCREASE = larger starting spiral (try 1500-2000)
                          // DECREASE = smaller starting spiral (try 800-1000)
  
  int decrement = 60;     // How much to shrink each side (in ticks)
                          // INCREASE = looser spiral, fewer loops (try 80-100)
                          // DECREASE = tighter spiral, more loops (try 40-50)
  
  int speed = 500;        // Motor speed (0-500, safe limit is 400)
                          // INCREASE = faster movement (max 500)
                          // DECREASE = slower, more controlled (try 300-400)
  
  // Main spiral loop - continue until side length becomes too small
  while (sideLength > 100) {
    // Drive straight for current side length
    driveTicks(sideLength, speed);
    
    // Turn 90 degrees right (clockwise)
    turnRight90();
    
    // Decrease side length for next iteration (creates inward spiral)
    sideLength -= decrement;
    
    // Allow user to stop the pattern early
    if (buttonB.isPressed()) break;
  }
  
  // Ensure motors are stopped
  stopMotors();
  
  // Display completion message
  display.clear();
  display.print("Complete!");
  delay(1000);
}

/*
 * modeZigzag() - Zig-Zag Pattern Implementation
 * 
 * Behavior: Robot drives forward, then alternates between left and right
 * turns to create a zig-zag (back-and-forth) pattern.
 * 
 * Algorithm:
 * 1. Drive straight segment
 * 2. Turn right ~120° (even iterations)
 * 3. Drive straight segment
 * 4. Turn left ~120° (odd iterations)
 * 5. Repeat
 * 
 * Key Features:
 * - Consistent segment lengths
 * - Alternating turn directions using modulo operator (i % 2)
 * - Smooth, uniform turns
 * - Symmetrical pattern
 */
void modeZigzag() {
  // Display mode name
  display.clear();
  display.print("LUDICROUS!");
  delay(500);
  
  // ========== ADJUSTABLE PARAMETERS ==========
  // HOW TO CHANGE THE ZIG-ZAG:
  
  int segmentLength = 800; // Length of each straight segment (in encoder ticks)
                           // INCREASE = longer diagonal lines (try 1000-1200)
                           // DECREASE = shorter, tighter zigzag (try 500-700)
  
  int zigzags = 8;         // Number of zig-zag segments to perform
                           // INCREASE = more zigzags, longer pattern (try 10-12)
                           // DECREASE = fewer zigzags, shorter pattern (try 5-6)
  
  int speed = 500;         // Motor speed (0-500, safe limit is 400)
                           // INCREASE = faster movement (max 500)
                           // DECREASE = slower, more controlled (try 300-400)
  
  // Turn angle adjustment: Change "TURN_90_TICKS + 100" below
  // INCREASE +100 = sharper angle, narrower zigzag (try +150)
  // DECREASE +100 = wider angle, broader zigzag (try +50)
  
  // Main zig-zag loop
  for (int i = 0; i < zigzags; i++) {
    // Drive forward for one segment
    driveTicks(segmentLength, speed);
    
    // Alternate between left and right turns
    if (i % 2 == 0) {
      // Turn right (~120 degrees)
      resetEncoders();
      while (abs(encoders.getCountsLeft()) < TURN_90_TICKS + 100) {
        motors.setSpeeds(500, -500);  // LUDICROUS turn speed!
        if (buttonB.isPressed()) break;
      }
      motors.setSpeeds(0, 0);
    } else {
      // Turn left (~120 degrees)
      resetEncoders();
      while (abs(encoders.getCountsLeft()) < TURN_90_TICKS + 100) {
        motors.setSpeeds(-500, 500);  // LUDICROUS turn speed!
        if (buttonB.isPressed()) break;
      }
      motors.setSpeeds(0, 0);
    }
    
    // Allow early exit
    if (buttonB.isPressed()) break;
  }
  
  stopMotors();
  display.clear();
  display.print("Complete!");
  delay(1000);
}

/*
 * modeSquareZigzag() - Square Zig-Zag (Lawn Mowing) Pattern
 * 
 * Behavior: Robot drives in parallel straight lines, turning at the end
 * of each line to systematically cover a rectangular area. Similar to
 * mowing a lawn in rows.
 * 
 * Algorithm:
 * 1. Drive straight along a row
 * 2. Turn 90° and move to next row position
 * 3. Turn 90° again to face along the new row
 * 4. Repeat (alternating direction each row)
 * 
 * Key Features:
 * - Straight parallel lines
 * - Precise 90° turns
 * - Even spacing between rows
 * - Complete area coverage with no gaps or overlaps
 * - Alternating row directions (left-to-right, then right-to-left)
 */
void modeSquareZigzag() {
  // Display mode name
  display.clear();
  display.print("LUDICROUS!");
  delay(500);
  
  // ========== ADJUSTABLE PARAMETERS ==========
  // HOW TO CHANGE THE LAWN MOWING PATTERN:
  
  int rowLength = 1000;    // Length of each row (in encoder ticks)
                           // INCREASE = longer rows (try 1200-1500)
                           // DECREASE = shorter rows (try 700-900)
  
  int rows = 6;            // Number of parallel rows to drive
                           // INCREASE = more rows, larger area (try 8-10)
                           // DECREASE = fewer rows, smaller area (try 4-5)
  
  int rowSpacing = 250;    // Distance between parallel rows (in ticks)
                           // INCREASE = wider spacing between rows (try 300-400)
                           // DECREASE = tighter spacing, more coverage (try 150-200)
  
  int speed = 500;         // Motor speed (0-500, safe limit is 400)
                           // INCREASE = faster movement (max 500)
                           // DECREASE = slower, more controlled (try 300-400)
  
  // Main lawn-mowing loop
  for (int i = 0; i < rows; i++) {
    // Drive forward along the current row
    driveTicks(rowLength, speed);
    
    // Move to next row (unless this is the last row)
    if (i < rows - 1) {
      // Alternate turn direction based on even/odd row
      if (i % 2 == 0) {
        // Even rows: Turn right, move to next row, turn right again
        turnRight90();
        driveTicks(rowSpacing, speed);
        turnRight90();
      } else {
        // Odd rows: Turn left, move to next row, turn left again
        turnLeft90();
        driveTicks(rowSpacing, speed);
        turnLeft90();
      }
    }
    
    // Allow early exit
    if (buttonB.isPressed()) break;
  }
  
  stopMotors();
  display.clear();
  display.print("Complete!");
  delay(1000);
}

/*
 * modeCircle() - Circular Path Implementation
 * 
 * Behavior: Robot drives in a continuous circle with constant radius.
 * The endpoint should meet the startpoint (closed circle, not a spiral).
 * 
 * Algorithm:
 * 1. Set different speeds for left and right motors
 * 2. The speed ratio determines the circle radius
 * 3. Run for calculated duration to complete one full circle
 * 
 * Key Concepts:
 * - Differential drive: Different wheel speeds create curved paths
 * - Outer wheel travels faster than inner wheel
 * - Speed ratio = radius of curvature
 * - Time-based control ensures circle closes properly
 * 
 * Key Features:
 * - Smooth continuous circle (not a spiral)
 * - Constant radius throughout
 * - Endpoint meets startpoint (±5cm tolerance)
 * - No flat sections or irregular shapes
 */
void modeCircle() {
  // Display mode name
  display.clear();
  display.print("LUDICROUS!");
  delay(500);
  
  // ========== ADJUSTABLE PARAMETERS ==========
  // HOW TO CHANGE THE CIRCLE:
  
  int outerSpeed = 450;  // Outer wheel speed (faster wheel)
                         // INCREASE = faster circle, larger radius (try 480-500)
                         // DECREASE = slower circle (try 350-400)
  
  int innerSpeed = 250;  // Inner wheel speed (slower wheel)
                         // INCREASE = larger circle radius (try 280-320)
                         // DECREASE = tighter circle, smaller radius (try 200-230)
                         // NOTE: Speed difference (outerSpeed - innerSpeed) controls radius
                         //       Larger difference = tighter circle
                         //       Smaller difference = wider circle
  
  unsigned long circleDuration = 7000; // How long to drive in circle (milliseconds)
                                       // INCREASE = more than one full circle (try 9000-12000)
                                       // DECREASE = partial circle (try 4000-6000)
                                       // NOTE: Adjust this to make circle close perfectly
  unsigned long startTime = millis();
  
  // Main circle loop - run until duration elapsed
  while (millis() - startTime < circleDuration) {
    // Set motor speeds to maintain circular path
    motors.setSpeeds(outerSpeed, innerSpeed);
    
    // Allow user to stop early
    if (buttonB.isPressed()) break;
  }
  
  // Stop motors at end of circle
  stopMotors();
  
  // Display completion message
  display.clear();
  display.print("Complete!");
  delay(1000);
}
