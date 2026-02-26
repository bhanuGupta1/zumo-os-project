/*
 * Modes.cpp - Ballistic Behavior Implementations
 * 
 * ========== WHAT THIS FILE DOES ==========
 * This file contains all 4 ballistic behaviors for the SBA assignment:
 * 
 * 1. SPIRAL - Circular spiral with decreasing radius
 *    - Smooth continuous curve (no corners)
 *    - Starts wide, spirals inward to center
 *    - Like water going down a drain
 * 
 * 2. ZIG-ZAG - Diagonal back-and-forth pattern
 *    - Straight segments with sharp turns
 *    - Alternates between two angles
 *    - Creates "Z" or lightning bolt shape
 * 
 * 3. SQUARE ZIG-ZAG (LAWN MOWER) - Parallel rows pattern
 *    - Straight parallel lines with 90° turns
 *    - Covers rectangular area systematically
 *    - Like mowing a lawn or printer head movement
 * 
 * 4. CIRCLE - Single continuous circular path
 *    - Constant radius throughout
 *    - Endpoint meets startpoint
 *    - Smooth curve with no corners
 * 
 * ========== HOW TO ADJUST BEHAVIORS ==========
 * Each mode has an "ADJUSTABLE PARAMETERS" section at the top.
 * Look for these variables to customize the behavior:
 * 
 * SPEED:
 * - Change "speed" or "baseSpeed" variable (0-500)
 * - Safe limit: 400, Ludicrous mode: 500
 * - Higher = faster, Lower = more controlled
 * 
 * SIZE/DISTANCE:
 * - Change "segmentLength", "rowLength", "startRadiusDiff" variables
 * - Measured in encoder ticks (roughly 1 tick = 0.1mm)
 * - Higher = larger pattern, Lower = smaller pattern
 * 
 * ANGLES:
 * - For zig-zag: adjust "turnAngleTicks" value
 * - For lawn mower: uses turnRight90() and turnLeft90() (fixed 90°)
 * - Higher ticks = sharper turn, Lower ticks = gentler turn
 * 
 * CIRCLE/SPIRAL DIAMETER:
 * - Adjust speed difference between wheels
 * - Larger difference = tighter curve (smaller diameter)
 * - Smaller difference = wider curve (larger diameter)
 * 
 * ========== KEY CONCEPTS ==========
 * - Encoder ticks: Measure wheel rotation for accurate distance control
 * - Motor speed control: Set left/right motor speeds independently
 * - Differential drive: Different wheel speeds create curved paths
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
 * modeSquareSpiral() - Circular Spiral Pattern Implementation
 * 
 * WHAT IS A SPIRAL?
 * A spiral is a continuous curved path that circles around a center point
 * while gradually moving closer to (or away from) that center. Think of
 * a snail shell or a coiled rope.
 * 
 * Behavior: Robot drives in a circular spiral pattern, starting with a
 * large radius and gradually decreasing the radius until reaching the center.
 * Creates a smooth inward spiral like water going down a drain.
 * 
 * Algorithm:
 * 1. Drive in a circle using differential motor speeds
 * 2. Gradually decrease the speed difference to tighten the radius
 * 3. Continue until radius becomes very small (center reached)
 * 
 * Key Features:
 * - Smooth continuous curve (no sharp turns or straight lines)
 * - Radius decreases gradually and continuously
 * - Ends at center point
 * - Creates a true spiral shape (not a square with corners)
 */
void modeSquareSpiral() {
  // Display mode name on OLED
  display.clear();
  display.print("Bhanu");
  delay(500);
  
  // ========== ADJUSTABLE PARAMETERS ==========
  // HOW TO CHANGE THE SPIRAL:
  
  int baseSpeed = 500;           // Base motor speed (0-500)
                                 // INCREASE = faster spiral (max 500)
                                 // DECREASE = slower, smoother (try 300-400)
  
  int startRadiusDiff = 250;     // Initial speed difference (controls starting radius)
                                 // INCREASE = tighter starting circle (try 280-320)
                                 // DECREASE = wider starting circle (try 180-220)
  
  int radiusDecrement = 2;       // How fast radius shrinks per loop iteration
                                 // INCREASE = faster spiral inward (try 3-5)
                                 // DECREASE = slower, more gradual spiral (try 1)
  
  unsigned long loopDelay = 50;  // Delay between radius adjustments (milliseconds)
                                 // INCREASE = slower spiral (try 80-100)
                                 // DECREASE = faster spiral (try 20-40)
  
  // Main spiral loop - gradually decrease radius
  int currentRadiusDiff = startRadiusDiff;
  
  while (currentRadiusDiff > 10) {  // Stop when radius becomes very small (center)
    // Calculate motor speeds for current radius
    int outerSpeed = baseSpeed;
    int innerSpeed = baseSpeed - currentRadiusDiff;
    
    // Ensure inner speed doesn't go negative
    if (innerSpeed < 0) innerSpeed = 0;
    
    // Set motors to create circular motion
    motors.setSpeeds(outerSpeed, innerSpeed);
    
    // Gradually decrease radius for spiral effect
    currentRadiusDiff -= radiusDecrement;
    
    // Small delay for smooth spiral
    delay(loopDelay);
    
    // Allow user to stop early
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
 * WHAT IS A ZIG-ZAG?
 * A zig-zag is a pattern that moves forward in one direction, then makes
 * a sharp turn and moves forward in a different direction, creating a
 * "Z" or "lightning bolt" shape. The robot alternates between two angles,
 * creating a back-and-forth diagonal pattern.
 * 
 * Behavior: Robot drives forward, turns sharply to the right, drives forward,
 * turns sharply to the left, and repeats. This creates a series of connected
 * diagonal lines that zigzag across the space.
 * 
 * Algorithm:
 * 1. Drive straight for a fixed distance
 * 2. Turn right by a sharp angle (e.g., 60-90 degrees)
 * 3. Drive straight for the same distance
 * 4. Turn left by double the angle to change direction
 * 5. Repeat to create zigzag pattern
 * 
 * Key Features:
 * - Straight diagonal segments of equal length
 * - Sharp turns between segments (not gradual curves)
 * - Alternating turn directions create the "Z" shape
 * - Overall pattern moves in one general direction while zigzagging
 */
void modeZigzag() {
  // Display mode name
  display.clear();
  display.print("Bhanu");
  delay(500);
  
  // ========== ADJUSTABLE PARAMETERS ==========
  // HOW TO CHANGE THE ZIG-ZAG:
  
  int segmentLength = 800; // Length of each diagonal segment (in encoder ticks)
                           // INCREASE = longer diagonal lines (try 1000-1200)
                           // DECREASE = shorter, tighter zigzag (try 500-700)
  
  int zigzags = 8;         // Number of zig-zag segments to perform
                           // INCREASE = more zigzags, longer pattern (try 10-12)
                           // DECREASE = fewer zigzags, shorter pattern (try 5-6)
  
  int speed = 500;         // Motor speed for straight segments (0-500)
                           // INCREASE = faster movement (max 500)
                           // DECREASE = slower, more controlled (try 300-400)
  
  int turnAngleTicks = 200; // Turn angle in encoder ticks (controls zigzag sharpness)
                            // INCREASE = sharper turns, narrower zigzag (try 250-300)
                            // DECREASE = gentler turns, wider zigzag (try 150-180)
                            // NOTE: TURN_90_TICKS ≈ 340, so 200 ≈ 60 degrees
  
  // Main zig-zag loop
  for (int i = 0; i < zigzags; i++) {
    // Drive forward for one diagonal segment
    driveTicks(segmentLength, speed);
    
    // Alternate between right and left turns to create zigzag
    if (i % 2 == 0) {
      // Turn right (creates the "zig")
      resetEncoders();
      while (abs(encoders.getCountsLeft()) < turnAngleTicks) {
        motors.setSpeeds(500, -500);  // LUDICROUS turn speed!
        if (buttonB.isPressed()) break;
      }
      motors.setSpeeds(0, 0);
    } else {
      // Turn left (creates the "zag")
      resetEncoders();
      while (abs(encoders.getCountsLeft()) < turnAngleTicks * 2) {  // Double angle to reverse direction
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
 * WHAT IS A LAWN MOWING PATTERN?
 * A lawn mowing pattern (also called boustrophedon or square zigzag) is
 * a systematic way to cover a rectangular area by driving in parallel
 * straight lines, like mowing a lawn. The robot drives along one row,
 * moves to the next row, then drives back in the opposite direction.
 * 
 * Behavior: Robot drives straight along a row, makes two 90-degree turns
 * to position itself at the start of the next parallel row, then drives
 * back in the opposite direction. This continues until the entire area
 * is covered with parallel rows.
 * 
 * Algorithm:
 * 1. Drive straight along current row (full length)
 * 2. Turn 90° perpendicular to the row
 * 3. Drive forward by row spacing distance
 * 4. Turn 90° again to face along the next row (opposite direction)
 * 5. Repeat until all rows are complete
 * 
 * Key Features:
 * - Parallel straight rows with consistent spacing
 * - Precise 90° turns only (no diagonal movement)
 * - Alternating row directions (left-to-right, then right-to-left)
 * - Complete area coverage with no gaps or overlaps
 * - Efficient systematic coverage pattern
 * - Similar to how a lawn mower or printer head moves
 */
void modeSquareZigzag() {
  // Display mode name
  display.clear();
  display.print("Bhanu");
  delay(500);
  
  // ========== ADJUSTABLE PARAMETERS ==========
  // HOW TO CHANGE THE LAWN MOWING PATTERN:
  
  int rowLength = 1000;    // Length of each row (in encoder ticks)
                           // INCREASE = longer rows, wider area (try 1200-1500)
                           // DECREASE = shorter rows, narrower area (try 700-900)
                           // NOTE: This is the width of the area being covered
  
  int rows = 6;            // Number of parallel rows to drive
                           // INCREASE = more rows, taller area (try 8-10)
                           // DECREASE = fewer rows, smaller area (try 4-5)
                           // NOTE: This is the height of the area being covered
  
  int rowSpacing = 250;    // Distance between parallel rows (in ticks)
                           // INCREASE = wider spacing, less overlap (try 300-400)
                           // DECREASE = tighter spacing, more coverage (try 150-200)
                           // NOTE: Should match robot width for complete coverage
  
  int speed = 500;         // Motor speed (0-500, safe limit is 400)
                           // INCREASE = faster movement (max 500)
                           // DECREASE = slower, more controlled (try 300-400)
  
  // Main lawn-mowing loop
  for (int i = 0; i < rows; i++) {
    // Drive forward along the current row (full length)
    driveTicks(rowLength, speed);
    
    // Move to next row (unless this is the last row)
    if (i < rows - 1) {
      // Alternate turn direction based on even/odd row
      // This creates the back-and-forth pattern
      if (i % 2 == 0) {
        // Even rows: Turn right, move to next row, turn right again
        // This positions robot for left-to-right on next row
        turnRight90();
        driveTicks(rowSpacing, speed);
        turnRight90();
      } else {
        // Odd rows: Turn left, move to next row, turn left again
        // This positions robot for right-to-left on next row
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
  display.print("Bhanu");
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
