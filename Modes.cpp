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
  // Smooth circular spiral with gradually changing radius
  int outerSpeed = 400;          // Outer wheel speed (constant)
  int startInnerSpeed = inward ? 350 : 100;  // Starting inner wheel speed
  int endInnerSpeed = inward ? 100 : 350;    // Ending inner wheel speed
  float speedChangeRate = inward ? -0.5 : 0.5;  // How fast to change speed
  unsigned long loopDelay = 30;  // Delay for smooth spiral
  
  // Main spiral loop
  float currentInnerSpeed = startInnerSpeed;
  
  while (true) {
    // Check limits
    if (inward && currentInnerSpeed <= endInnerSpeed) break;
    if (!inward && currentInnerSpeed >= endInnerSpeed) break;
    
    // Ensure speeds stay in valid range
    int innerSpeed = (int)currentInnerSpeed;
    if (innerSpeed < 50) innerSpeed = 50;
    if (innerSpeed > 500) innerSpeed = 500;
    
    // Set motors to create circular motion
    // Right wheel is outer (faster), left wheel is inner (slower)
    motors.setSpeeds(innerSpeed, outerSpeed);
    
    // Gradually change inner wheel speed for spiral effect
    currentInnerSpeed += speedChangeRate;
    
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
  
  int segmentLength = 900; // Length of each diagonal segment (in encoder ticks)
                           // INCREASE = longer diagonal lines (try 1000-1200)
                           // DECREASE = shorter, tighter zigzag (try 500-700)
  
  int zigzags = 6;         // Number of zig-zag segments to perform
                           // INCREASE = more zigzags, longer pattern (try 10-12)
                           // DECREASE = fewer zigzags, shorter pattern (try 5-6)
  
  int speed = 400;         // Motor speed for straight segments (0-500)
                           // INCREASE = faster movement (max 500)
                           // DECREASE = slower, more controlled (try 300-400)
  
  int turnAngleTicks = 240; // Turn angle in encoder ticks (controls zigzag sharpness)
                            // INCREASE = sharper turns, narrower zigzag (try 250-300)
                            // DECREASE = gentler turns, wider zigzag (try 150-180)
                            // NOTE: TURN_90_TICKS ≈ 320, so 240 ≈ 67 degrees
  
  // Main zig-zag loop
  for (int i = 0; i < zigzags; i++) {
    // Drive forward for one diagonal segment
    driveTicks(segmentLength, speed);
    delay(100);  // Brief pause for stability
    
    // Alternate between right and left turns to create zigzag
    if (i % 2 == 0) {
      // Turn right (creates the "zig")
      resetEncoders();
      while (abs(encoders.getCountsLeft()) < turnAngleTicks) {
        motors.setSpeeds(400, -400);  // Fast turn speed
        if (buttonB.isPressed()) break;
      }
      motors.setSpeeds(0, 0);
      delay(100);  // Brief pause after turn
    } else {
      // Turn left (creates the "zag")
      resetEncoders();
      while (abs(encoders.getCountsLeft()) < turnAngleTicks * 2) {  // Double angle to reverse direction
        motors.setSpeeds(-400, 400);  // Fast turn speed
        if (buttonB.isPressed()) break;
      }
      motors.setSpeeds(0, 0);
      delay(100);  // Brief pause after turn
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
 * modeSquareZigzag() - Square Zig-Zag Pattern
 * 
 * WHAT IS A SQUARE ZIG-ZAG PATTERN?
 * A square zig-zag creates vertical parallel lines by driving forward,
 * turning to move sideways, then driving in the opposite direction.
 * The pattern looks like: | | | (vertical rectangles side by side)
 * 
 * Behavior: Robot drives straight up, makes two 90-degree turns to move
 * to the next column position, then drives straight down. This continues
 * creating parallel vertical lines.
 * 
 * Algorithm:
 * 1. Drive straight forward (creating vertical line)
 * 2. Turn 90° to the side
 * 3. Drive forward by column spacing distance
 * 4. Turn 90° again to face opposite direction (up/down alternates)
 * 5. Repeat until all columns are complete
 * 
 * Key Features:
 * - Parallel vertical lines with consistent spacing
 * - Precise 90° turns only
 * - Alternating directions (up, down, up, down)
 * - Creates rectangular coverage pattern
 * - Similar to scanning or covering an area systematically
 */
void modeSquareZigzag() {
  // Display mode starting
  display.clear();
  display.print("Starting...");
  delay(500);
  
  // ========== ADJUSTABLE PARAMETERS ==========
  // Square zig-zag pattern: Creates vertical parallel lines
  // Pattern looks like: | | | (three vertical lines side by side)
  
  int columnHeight = 1400;  // Height of each vertical line (in encoder ticks)
  int columns = 3;          // Number of vertical columns
  int columnSpacing = 400;  // Distance between columns (in ticks)
  int speed = 350;          // Motor speed for better accuracy
  
  // Main square zig-zag loop
  for (int i = 0; i < columns; i++) {
    // Drive forward (creating vertical line)
    driveTicks(columnHeight, speed);
    delay(150);
    
    // Move to next column (unless this is the last column)
    if (i < columns - 1) {
      // Alternate turn direction to create zig-zag
      if (i % 2 == 0) {
        // Even columns: Turn right, move to next column, turn right again
        // This makes robot face downward for next column
        turnRight90();
        delay(150);
        driveTicks(columnSpacing, speed);
        delay(150);
        turnRight90();
        delay(150);
      } else {
        // Odd columns: Turn left, move to next column, turn left again
        // This makes robot face upward for next column
        turnLeft90();
        delay(150);
        driveTicks(columnSpacing, speed);
        delay(150);
        turnLeft90();
        delay(150);
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
