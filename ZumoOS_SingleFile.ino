/*
 * ============================================================================
 * ZUMO ROBOT - BALLISTIC BEHAVIOURS (Single File Version)
 * ============================================================================
 * 
 * Student: Bhanu
 * Date: February 27, 2026
 * Assignment: SBA - Ballistic Behaviors for Autonomous Robot
 * 
 * ============================================================================
 * WHAT THIS PROGRAM DOES:
 * ============================================================================
 * This program implements 4 REQUIRED ballistic behaviors:
 * 
 * 1. SQUARE SPIRAL - Robot drives in a square spiral pattern (inward/outward)
 *    - Straight lines with 90° turns
 *    - Each side gets shorter (inward) or longer (outward)
 * 
 * 2. ZIG-ZAG - Robot creates a zigzag pattern like /\/\/\
 *    - Diagonal lines with alternating turns
 *    - Creates a "lightning bolt" shape
 * 
 * 3. SQUARE ZIG-ZAG - Robot creates vertical parallel lines like | | |
 *    - Drives up, moves sideways, drives down, moves sideways
 *    - Covers area systematically
 * 
 * 4. CIRCLE - Robot drives in a perfect circle
 *    - Constant radius throughout
 *    - End point meets start point
 * 
 * BONUS: SPIRAL - Smooth circular spiral (for comparison with square spiral)
 * 
 * ============================================================================
 * HOW TO USE:
 * ============================================================================
 * 1. Upload this code to Zumo robot
 * 2. Power on robot
 * 3. Use buttons to select mode:
 *    - Button A: Next mode
 *    - Button C: Previous mode
 *    - Button B: Run selected mode
 * 4. During execution, press Button B to stop
 * 
 * ============================================================================
 */

#include <Wire.h>
#include <Zumo32U4.h>

// ============================================================================
// HARDWARE OBJECTS - These control the robot's physical components
// ============================================================================
Zumo32U4Motors motors;          // Controls left and right motors
Zumo32U4Encoders encoders;      // Counts wheel rotations for precise movement
Zumo32U4OLED display;           // 128x64 screen for showing information
Zumo32U4Buzzer buzzer;          // Speaker for sound feedback
Zumo32U4ButtonA buttonA;        // Left button
Zumo32U4ButtonB buttonB;        // Middle button (also used to stop)
Zumo32U4ButtonC buttonC;        // Right button

// ============================================================================
// CONFIGURATION CONSTANTS - Adjust these to tune robot behavior
// ============================================================================
#define TURN_90_TICKS 640       // Encoder ticks for 90° turn (doubled for accuracy)
#define TURN_180_TICKS 1280     // Encoder ticks for 180° turn
#define TURN_SPEED 250          // Speed when turning

// ============================================================================
// MODE ENUMERATION - Defines all available modes
// ============================================================================
enum Mode {
  MODE_SPIRAL,          // Bonus: Circular spiral (for comparison)
  MODE_SQUARE_SPIRAL,   // Required: Square spiral
  MODE_ZIGZAG,          // Required: Zigzag pattern
  MODE_SQUARE_ZIGZAG,   // Required: Square zigzag (vertical lines)
  MODE_CIRCLE,          // Required: Circle
  MODE_COUNT            // Total number of modes
};

// Mode names for display
const char* modeNames[] = {
  "Spiral",
  "Sq Spiral",
  "Zig-Zag",
  "Sq Zig-Zag",
  "Circle"
};

// ============================================================================
// SETUP - Runs once when robot powers on
// ============================================================================
void setup() {
  // Initialize display
  display.clear();
  display.print("ZumoOS");
  display.gotoXY(0, 1);
  display.print("Ready!");
  
  // Play startup sound
  buzzer.play("c32 e32 g32 c64");
  delay(1000);
}

// ============================================================================
// MAIN LOOP - Runs continuously forever
// ============================================================================
void loop() {
  // Show menu and get user's choice
  Mode selectedMode = selectMode();
  
  // Run the selected mode
  runMode(selectedMode);
  
  // Show completion message
  display.clear();
  display.print("Done!");
  delay(1000);
}

// ============================================================================
// MENU SYSTEM - Allows user to select which mode to run
// ============================================================================
Mode selectMode() {
  static Mode currentMode = MODE_SQUARE_SPIRAL;  // Remember last selection
  
  // Show menu
  display.clear();
  display.print("A/C:Chg B:Run");
  display.gotoXY(0, 1);
  display.print(modeNames[currentMode]);
  
  // Wait for button press
  while (true) {
    // Button A: Next mode
    if (buttonA.getSingleDebouncedPress()) {
      currentMode = (Mode)((currentMode + 1) % MODE_COUNT);
      display.gotoXY(0, 1);
      display.print("                ");  // Clear line
      display.gotoXY(0, 1);
      display.print(modeNames[currentMode]);
      buzzer.play("g32");
    }
    
    // Button C: Previous mode
    if (buttonC.getSingleDebouncedPress()) {
      currentMode = (Mode)((currentMode - 1 + MODE_COUNT) % MODE_COUNT);
      display.gotoXY(0, 1);
      display.print("                ");  // Clear line
      display.gotoXY(0, 1);
      display.print(modeNames[currentMode]);
      buzzer.play("g32");
    }
    
    // Button B: Run selected mode
    if (buttonB.getSingleDebouncedPress()) {
      display.clear();
      display.print("Starting...");
      delay(500);
      return currentMode;
    }
    
    delay(50);
  }
}

// ============================================================================
// MODE DISPATCHER - Calls the appropriate mode function
// ============================================================================
void runMode(Mode m) {
  switch(m) {
    case MODE_SPIRAL:        modeSpiral();       break;
    case MODE_SQUARE_SPIRAL: modeSquareSpiral(); break;
    case MODE_ZIGZAG:        modeZigzag();       break;
    case MODE_SQUARE_ZIGZAG: modeSquareZigzag(); break;
    case MODE_CIRCLE:        modeCircle();       break;
  }
}

// ============================================================================
// HELPER FUNCTIONS - Used by all modes
// ============================================================================

// Reset encoder counts to zero
void resetEncoders() {
  encoders.getCountsAndResetLeft();
  encoders.getCountsAndResetRight();
}

// Drive straight for specified encoder ticks
void driveTicks(int ticks, int speed) {
  resetEncoders();
  while (encoders.getCountsLeft() < ticks && encoders.getCountsRight() < ticks) {
    motors.setSpeeds(speed, speed);
    if (buttonB.isPressed()) break;  // Allow early stop
  }
  motors.setSpeeds(0, 0);
  delay(200);  // Let robot settle
}

// Turn right 90 degrees
void turnRight90() {
  resetEncoders();
  while (abs(encoders.getCountsLeft()) < TURN_90_TICKS) {
    motors.setSpeeds(TURN_SPEED, -TURN_SPEED);  // Right wheel backward, left forward
    if (buttonB.isPressed()) break;
  }
  motors.setSpeeds(0, 0);
  delay(200);
}

// Turn left 90 degrees
void turnLeft90() {
  resetEncoders();
  while (abs(encoders.getCountsLeft()) < TURN_90_TICKS) {
    motors.setSpeeds(-TURN_SPEED, TURN_SPEED);  // Left wheel backward, right forward
    if (buttonB.isPressed()) break;
  }
  motors.setSpeeds(0, 0);
  delay(200);
}

// ============================================================================
// MODE 1: SPIRAL (BONUS - NOT REQUIRED)
// ============================================================================
// Creates a smooth circular spiral by gradually changing wheel speeds
// User can choose inward (to center) or outward (from center)
// ============================================================================
void modeSpiral() {
  // Ask user for direction
  display.clear();
  display.print("A:In C:Out");
  
  bool inward = true;
  while (true) {
    if (buttonA.getSingleDebouncedPress()) { inward = true; break; }
    if (buttonC.getSingleDebouncedPress()) { inward = false; break; }
    if (buttonB.getSingleDebouncedPress()) return;  // Cancel
    delay(50);
  }
  
  display.clear();
  display.print(inward ? "Inward" : "Outward");
  delay(1000);
  
  // Spiral parameters
  int outerSpeed = 400;                              // Outer wheel (constant)
  float innerSpeed = inward ? 350.0 : 100.0;         // Inner wheel (changes)
  float speedChange = inward ? -0.5 : 0.5;           // How fast to change
  float endSpeed = inward ? 100.0 : 350.0;           // When to stop
  
  // Main spiral loop
  while (true) {
    // Check if we've reached the end
    if (inward && innerSpeed <= endSpeed) break;
    if (!inward && innerSpeed >= endSpeed) break;
    
    // Set motor speeds (creates circular motion)
    motors.setSpeeds((int)innerSpeed, outerSpeed);
    
    // Gradually change inner wheel speed (creates spiral)
    innerSpeed += speedChange;
    
    delay(30);
    if (buttonB.isPressed()) break;
  }
  
  motors.setSpeeds(0, 0);
}

// ============================================================================
// MODE 2: SQUARE SPIRAL (REQUIRED)
// ============================================================================
// Creates a square spiral with straight lines and 90° turns
// Each side gets shorter (inward) or longer (outward)
// ============================================================================
void modeSquareSpiral() {
  // Ask user for direction
  display.clear();
  display.print("A:In C:Out");
  
  bool inward = true;
  while (true) {
    if (buttonA.getSingleDebouncedPress()) { inward = true; break; }
    if (buttonC.getSingleDebouncedPress()) { inward = false; break; }
    if (buttonB.getSingleDebouncedPress()) return;  // Cancel
    delay(50);
  }
  
  display.clear();
  display.print(inward ? "Inward" : "Outward");
  delay(1000);
  
  // Square spiral parameters
  int sideLength = inward ? 1400 : 200;  // Starting side length
  int increment = 70;                     // How much to change each side
  int speed = 500;                        // Motor speed
  int minSide = 150;                      // Minimum side length
  int maxSide = 1500;                     // Maximum side length
  
  // Main square spiral loop
  while (true) {
    // Check limits
    if (inward && sideLength < minSide) break;
    if (!inward && sideLength > maxSide) break;
    
    // Drive straight for current side length
    driveTicks(sideLength, speed);
    
    // Turn 90 degrees right
    turnRight90();
    
    // Change side length for next iteration
    if (inward) {
      sideLength -= increment;  // Get smaller
    } else {
      sideLength += increment;  // Get larger
    }
    
    if (buttonB.isPressed()) break;
  }
  
  motors.setSpeeds(0, 0);
}

// ============================================================================
// MODE 3: ZIG-ZAG (REQUIRED)
// ============================================================================
// Creates a zigzag pattern with diagonal lines
// Alternates between right and left turns
// Pattern looks like: /\/\/\/\
// ============================================================================
void modeZigzag() {
  // Zigzag parameters
  int segmentLength = 900;      // Length of each diagonal line
  int zigzags = 6;              // Number of zigzag segments
  int speed = 400;              // Motor speed
  int turnAngle = 480;          // Turn angle (about 67 degrees)
  
  // Main zigzag loop
  for (int i = 0; i < zigzags; i++) {
    // Drive forward
    driveTicks(segmentLength, speed);
    delay(100);
    
    // Alternate between right and left turns
    if (i % 2 == 0) {
      // Turn right (creates the "zig")
      resetEncoders();
      while (abs(encoders.getCountsLeft()) < turnAngle) {
        motors.setSpeeds(400, -400);
        if (buttonB.isPressed()) break;
      }
      motors.setSpeeds(0, 0);
      delay(100);
    } else {
      // Turn left (creates the "zag")
      // Turn twice as much to reverse direction
      resetEncoders();
      while (abs(encoders.getCountsLeft()) < turnAngle * 2) {
        motors.setSpeeds(-400, 400);
        if (buttonB.isPressed()) break;
      }
      motors.setSpeeds(0, 0);
      delay(100);
    }
    
    if (buttonB.isPressed()) break;
  }
  
  motors.setSpeeds(0, 0);
}

// ============================================================================
// MODE 4: SQUARE ZIG-ZAG (REQUIRED)
// ============================================================================
// Creates vertical parallel lines by driving up and down
// Pattern looks like: | | |
// Useful for covering a rectangular area systematically
// ============================================================================
void modeSquareZigzag() {
  // Square zigzag parameters
  int columnHeight = 1400;      // Height of each vertical line
  int columns = 3;              // Number of columns
  int columnSpacing = 400;      // Distance between columns
  int speed = 350;              // Motor speed
  
  // Main square zigzag loop
  for (int i = 0; i < columns; i++) {
    // Drive forward (creates vertical line)
    driveTicks(columnHeight, speed);
    delay(150);
    
    // Move to next column (unless this is the last one)
    if (i < columns - 1) {
      if (i % 2 == 0) {
        // Even columns: Turn right, move sideways, turn right
        // This makes robot face downward for next column
        turnRight90();
        delay(150);
        driveTicks(columnSpacing, speed);
        delay(150);
        turnRight90();
        delay(150);
      } else {
        // Odd columns: Turn left, move sideways, turn left
        // This makes robot face upward for next column
        turnLeft90();
        delay(150);
        driveTicks(columnSpacing, speed);
        delay(150);
        turnLeft90();
        delay(150);
      }
    }
    
    if (buttonB.isPressed()) break;
  }
  
  motors.setSpeeds(0, 0);
}

// ============================================================================
// MODE 5: CIRCLE (REQUIRED)
// ============================================================================
// Creates a perfect circle by setting different wheel speeds
// Outer wheel goes faster than inner wheel
// Runs for a fixed time to complete one full circle
// ============================================================================
void modeCircle() {
  // Circle parameters
  int outerSpeed = 450;                 // Outer wheel (faster)
  int innerSpeed = 250;                 // Inner wheel (slower)
  unsigned long duration = 7000;        // How long to drive (7 seconds)
  
  // Record start time
  unsigned long startTime = millis();
  
  // Main circle loop - run until time is up
  while (millis() - startTime < duration) {
    // Set motor speeds to create circular path
    motors.setSpeeds(outerSpeed, innerSpeed);
    
    // Allow early stop
    if (buttonB.isPressed()) break;
  }
  
  motors.setSpeeds(0, 0);
}

// ============================================================================
// END OF PROGRAM
// ============================================================================
