/*
 * Modes.cpp - Ballistic Behavior Implementations
 * 
 * ========== WHAT THIS FILE DOES ==========
 * This file contains all 5 ballistic behaviors for the SBA assignment:
 * 
 * 1. SPIRAL - Circular spiral with selectable direction
 *    - Smooth continuous curve (no corners)
 *    - User chooses inward (to center) or outward (from center)
 *    - Like water going down a drain or spring unwinding
 * 
 * 2. SQUARE SPIRAL - Square spiral with 90° turns and selectable direction
 *    - Straight lines with sharp 90° corners
 *    - User chooses inward or outward
 *    - Creates square/rectangular spiral pattern
 * 
 * 3. ZIG-ZAG - Diagonal back-and-forth pattern
 *    - Straight segments with sharp turns
 *    - Alternates between two angles
 *    - Creates "Z" or lightning bolt shape
 * 
 * 4. SQUARE ZIG-ZAG (LAWN MOWER) - Parallel rows pattern
 *    - Straight parallel lines with 90° turns
 *    - Covers rectangular area systematically
 *    - Like mowing a lawn or printer head movement
 * 
 * 5. CIRCLE - Single continuous circular path
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
    case MODE_SPIRAL: modeSpiral(); break;
    case MODE_SQUARE_SPIRAL: modeSquareSpiral(); break;
    case MODE_ZIGZAG: modeZigzag(); break;
    case MODE_SQUARE_ZIGZAG: modeSquareZigzag(); break;
    case MODE_CIRCLE: modeCircle(); break;
    default: break;  // Safety: do nothing if invalid mode
  }
}

// ============ BALLISTIC BEHAVIORS (SBA) ============

/*
 * modeSpiral() - Circular Spiral Pattern Implementation
 * 
 * WHAT IS A SPIRAL?
 * A spiral is a continuous curved path that circles around a center point
 * while gradually moving closer to (or away from) that center. Think of
 * a snail shell or a coiled rope.
 * 
 * Behavior: Robot drives in a circular spiral pattern with user-selectable direction.
 * Can spiral inward (to center) or outward (from center).
 * Creates a smooth spiral like water going down a drain or spring unwinding.
 * 
 * Algorithm:
 * 1. User selects direction: Button A = inward, Button C = outward
 * 2. Drive in a circle using differential motor speeds
 * 3. Gradually change the speed difference to change radius
 * 4. Continue until radius limit reached
 * 
 * Key Features:
 * - Smooth continuous curve (no sharp turns or straight lines)
 * - Radius changes gradually and continuously
 * - User-selectable direction (inward or outward)
 * - Creates a true spiral shape (not a square with corners)
 */
void modeSpiral() {
  // Display direction selection prompt
  display.clear();
  display.print("A:In C:Out");
  display.gotoXY(0, 1);
  display.print("B:Cancel");
  
  // Wait for direction selection
  bool directionSelected = false;
  bool inward = true;  // Default to inward
  
  while (!directionSelected) {
    if (buttonA.getSingleDebouncedPress()) {
      inward = true;
      directionSelected = true;
    }
    if (buttonC.getSingleDebouncedPress()) {
      inward = false;
      directionSelected = true;
    }
    if (buttonB.getSingleDebouncedPress()) {
      return;  // Cancel
    }
    delay(50);
  }
  
  // Show selected direction
  display.clear();
  display.print(inward ? "Inward" : "Outward");
  delay(1000);
  
  // ========== ADJUSTABLE PARAMETERS ==========
  int baseSpeed = 500;           // Base motor speed (0-500)
  int startRadiusDiff = inward ? 250 : 20;  // Start wide for inward, tight for outward
  int radiusChange = 1;          // How fast radius changes per iteration
  unsigned long loopDelay = 30;  // Delay between radius adjustments (ms)
  int minRadius = 10;            // Minimum radius (tightest circle)
  int maxRadius = 280;           // Maximum radius (widest circle)
  
  // Main spiral loop
  int currentRadiusDiff = startRadiusDiff;
  
  while (true) {
    // Check limits
    if (inward && currentRadiusDiff < minRadius) break;
    if (!inward && currentRadiusDiff > maxRadius) break;
    
    // Calculate motor speeds for current radius
    int outerSpeed = baseSpeed;
    int innerSpeed = baseSpeed - currentRadiusDiff;
    
    // Ensure inner speed doesn't go negative or too high
    if (innerSpeed < 0) innerSpeed = 0;
    if (innerSpeed > 500) innerSpeed = 500;
    
    // Set motors to create circular motion
    motors.setSpeeds(outerSpeed, innerSpeed);
    
    // Change radius for spiral effect
    if (inward) {
      currentRadiusDiff -= radiusChange;  // Decrease for inward
    } else {
      currentRadiusDiff += radiusChange;  // Increase for outward
    }
    
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
 * modeSquareSpiral() - Square Spiral Pattern with 90° Turns
 * 
 * WHAT IS A SQUARE SPIRAL?
 * A square spiral is a pattern made of straight lines and 90-degree turns,
 * where each side gets progressively longer (outward) or shorter (inward).
 * Unlike a circular spiral, this has distinct corners and straight edges.
 * 
 * Behavior: Robot drives in straight lines with 90° turns, with user-selectable
 * direction. Can spiral inward (to center) or outward (from center).
 * 
 * Algorithm:
 * 1. User selects direction: Button A = inward, Button C = outward
 * 2. Drive straight for current side length
 * 3. Turn 90 degrees right
 * 4. Increase or decrease side length
 * 5. Repeat until limit reached
 * 
 * Key Features:
 * - Straight lines with sharp 90° turns
 * - Each side length changes by fixed increment
 * - User-selectable direction (inward or outward)
 * - Creates square/rectangular spiral pattern
 */
void modeSquareSpiral() {
  // Display direction selection prompt
  display.clear();
  display.print("A:In C:Out");
  display.gotoXY(0, 1);
  display.print("B:Cancel");
  
  // Wait for direction selection
  bool directionSelected = false;
  bool inward = true;  // Default to inward
  
  while (!directionSelected) {
    if (buttonA.getSingleDebouncedPress()) {
      inward = true;
      directionSelected = true;
    }
    if (buttonC.getSingleDebouncedPress()) {
      inward = false;
      directionSelected = true;
    }
    if (buttonB.getSingleDebouncedPress()) {
      return;  // Cancel
    }
    delay(50);
  }
  
  // Show selected direction
  display.clear();
  display.print(inward ? "Inward" : "Outward");
  delay(1000);
  
  // ========== ADJUSTABLE PARAMETERS ==========
  int sideLength = inward ? 1400 : 200;  // Start large for inward, small for outward
  int increment = 70;            // How much to change each side
  int speed = 500;               // Motor speed
  int minSide = 150;             // Minimum side length
  int maxSide = 1500;            // Maximum side length
  
  // Main square spiral loop
  while (true) {
    // Check limits
    if (inward && sideLength < minSide) break;
    if (!inward && sideLength > maxSide) break;
    
    // Drive straight for current side length
    driveTicks(sideLength, speed);
    
    // Turn 90 degrees right (clockwise)
    turnRight90();
    
    // Change side length for next iteration
    if (inward) {
      sideLength -= increment;  // Decrease for inward spiral
    } else {
      sideLength += increment;  // Increase for outward spiral
    }
    
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
  // Display mode starting
  display.clear();
  display.print("Starting...");
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
  // Display mode starting
  display.clear();
  display.print("Starting...");
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
  // Display mode starting
  display.clear();
  display.print("Starting...");
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
