/*
 * Modes.cpp - Ballistic Behavior Implementations
 * 
 * This file contains the implementation of all 4 ballistic behaviors
 * required for the SBA assignment. Each mode uses encoder-based
 * movement for precise control and repeatability.
 * 
 * Key Concepts:
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
 * Behavior: Robot drives in a square spiral pattern, starting small
 * and gradually expanding outward. Each side is longer than the previous.
 * 
 * Algorithm:
 * 1. Drive straight for current side length
 * 2. Turn 90 degrees right
 * 3. Increase side length
 * 4. Repeat
 * 
 * Key Features:
 * - Clean 90° turns using encoder-based turnRight90()
 * - Straight lines using encoder-based driveTicks()
 * - Even spacing throughout the spiral
 * - User can stop anytime with Button B
 */
void modeSquareSpiral() {
  // Display mode name on OLED
  display.clear();
  display.print("LUDICROUS!");
  
  // Play Star Wars theme while starting
  buzzer.play("a4 a4 a4 f8 c8 a4 f8 c8 a2");
  delay(1000);
  
  // Configuration parameters - BEYOND LIMITS!
  int sideLength = 400;  // Starting side length
  int increment = 150;   // Larger increments for dramatic spiral
  int maxSides = 16;     // More sides for longer demonstration
  int speed = 500;       // BEYOND THE LIMIT!
  
  // Main spiral loop
  for (int i = 0; i < maxSides; i++) {
    // Drive straight for current side length
    driveTicks(sideLength, speed);
    
    // Turn 90 degrees right (clockwise)
    turnRight90();
    
    // Increase side length for next iteration (creates expanding spiral)
    sideLength += increment;
    
    // Allow user to stop the pattern early
    if (buttonB.isPressed()) break;
  }
  
  // Ensure motors are stopped
  stopMotors();
  
  // Play success sound
  buzzer.play("g32 a32 b32 c64");
  
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
  
  // Play Imperial March
  buzzer.play("a4 a4 a4 f8 c8 a4 f8 c8 a2");
  delay(1000);
  
  // Configuration parameters - BEYOND LIMITS!
  int segmentLength = 800; // Longer segments
  int zigzags = 8;         // More zigzags
  int speed = 500;         // BEYOND THE LIMIT!
  
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
  buzzer.play("g32 a32 b32 c64");
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
  
  // Play Cantina Band
  buzzer.play("f8 f8 f8 a8 c4 r8 a8 c4");
  delay(1000);
  
  // Configuration parameters - BEYOND LIMITS!
  int rowLength = 1000;    // Longer rows
  int rows = 6;            // More rows
  int rowSpacing = 250;    // Wider spacing
  int speed = 500;         // BEYOND THE LIMIT!
  
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
  buzzer.play("g32 a32 b32 c64");
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
  
  // Play Throne Room theme
  buzzer.play("c4 e4 g4 c4 e4 g4");
  delay(1000);
  
  // Motor speed configuration - BEYOND LIMITS!
  int outerSpeed = 450;  // LUDICROUS outer wheel speed!
  int innerSpeed = 250;  // LUDICROUS inner wheel speed!
  
  // Adjusted duration for complete circle at ludicrous speed
  unsigned long circleDuration = 7000; // 7 seconds at ludicrous speed
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
  
  // Play success sound
  buzzer.play("g32 a32 b32 c64");
  
  // Display completion message
  display.clear();
  display.print("Complete!");
  delay(1000);
}
