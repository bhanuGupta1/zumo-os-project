/*
 * ═══════════════════════════════════════════════════════════════════════════
 * ZUMO ROBOT - BALLISTIC BEHAVIOURS (VIVA EXPLANATION VERSION)
 * ═══════════════════════════════════════════════════════════════════════════
 * 
 * Student: Bhanu | Date: Feb 27, 2026 | Assignment: SBA Ballistic Behaviors
 * 
 * ═══════════════════════════════════════════════════════════════════════════
 * WHAT THIS PROGRAM DOES (In Simple Terms):
 * ═══════════════════════════════════════════════════════════════════════════
 * 
 * This program makes a Zumo robot perform 4 different movement patterns:
 * 
 * 1. SQUARE SPIRAL → Robot drives in a square that gets bigger or smaller
 * 2. ZIG-ZAG → Robot drives in a zigzag pattern like /\/\/\
 * 3. SQUARE ZIG-ZAG → Robot drives in vertical lines like | | |
 * 4. CIRCLE → Robot drives in a perfect circle
 * 
 * User selects which pattern using buttons, then robot performs it.
 * 
 * ═══════════════════════════════════════════════════════════════════════════
 */

// Include the Zumo library (gives us access to motors, buttons, display, etc.)
#include <Zumo32U4.h>

// ═══════════════════════════════════════════════════════════════════════════
// STEP 1: CREATE HARDWARE OBJECTS
// ═══════════════════════════════════════════════════════════════════════════
// These objects let us control the robot's physical parts

Zumo32U4Motors motors;      // Controls the 2 motors (left and right wheels)
Zumo32U4Encoders encoders;  // Counts wheel rotations (for precise distance)
Zumo32U4OLED display;       // The small screen on the robot
Zumo32U4Buzzer buzzer;      // The speaker (for beep sounds)
Zumo32U4ButtonA buttonA;    // Left button
Zumo32U4ButtonB buttonB;    // Middle button
Zumo32U4ButtonC buttonC;    // Right button

// ═══════════════════════════════════════════════════════════════════════════
// STEP 2: DEFINE CONSTANTS (Numbers that don't change)
// ═══════════════════════════════════════════════════════════════════════════

#define TURN_90 640    // How many encoder ticks = 90 degree turn
                       // (We doubled from 320 because robot was turning too little)

// ═══════════════════════════════════════════════════════════════════════════
// STEP 3: SETUP FUNCTION (Runs ONCE when robot turns on)
// ═══════════════════════════════════════════════════════════════════════════

void setup() {
  display.clear();              // Clear the screen
  display.print("Ready!");      // Show "Ready!" on screen
  buzzer.play("c32 e32 g32");   // Play a happy startup sound (C-E-G notes)
  delay(1000);                  // Wait 1 second (1000 milliseconds)
}

// ═══════════════════════════════════════════════════════════════════════════
// STEP 4: LOOP FUNCTION (Runs FOREVER after setup)
// ═══════════════════════════════════════════════════════════════════════════

void loop() {
  // Step 4.1: Show menu and let user pick a mode
  int mode = showMenu();
  
  // Step 4.2: Run the mode user selected
  if (mode == 1) squareSpiral();      // Run square spiral
  if (mode == 2) zigzag();            // Run zigzag
  if (mode == 3) squareZigzag();      // Run square zigzag
  if (mode == 4) circle();            // Run circle
  
  // Step 4.3: Show "Done!" message
  display.clear();
  display.print("Done!");
  delay(1000);
  
  // Now loop() starts again from the top (goes back to menu)
}

// ═══════════════════════════════════════════════════════════════════════════
// HELPER FUNCTION: Show Menu and Get User Choice
// ═══════════════════════════════════════════════════════════════════════════
// This function shows a menu on the screen and waits for user to pick a mode
// Returns: 1, 2, 3, or 4 (which mode user selected)

int showMenu() {
  int choice = 1;  // Start with mode 1 selected
  
  // Keep looping until user presses button B to confirm
  while (true) {
    // Show current choice on screen
    display.clear();
    display.print("Mode: ");
    display.print(choice);
    display.gotoXY(0, 1);  // Move to line 2
    
    // Show mode name based on choice
    if (choice == 1) display.print("Sq Spiral");
    if (choice == 2) display.print("Zig-Zag");
    if (choice == 3) display.print("Sq Zig-Zag");
    if (choice == 4) display.print("Circle");
    
    // Check which button was pressed
    if (buttonA.getSingleDebouncedPress()) {
      choice++;              // Button A = next mode
      if (choice > 4) choice = 1;  // Wrap around to 1 after 4
      buzzer.play("g32");    // Beep to confirm
    }
    
    if (buttonC.getSingleDebouncedPress()) {
      choice--;              // Button C = previous mode
      if (choice < 1) choice = 4;  // Wrap around to 4 before 1
      buzzer.play("g32");    // Beep to confirm
    }
    
    if (buttonB.getSingleDebouncedPress()) {
      return choice;         // Button B = confirm and return choice
    }
    
    delay(50);  // Small delay to prevent button reading too fast
  }
}

// ═══════════════════════════════════════════════════════════════════════════
// HELPER FUNCTION: Drive Straight
// ═══════════════════════════════════════════════════════════════════════════
// Makes robot drive straight forward for a specific distance
// Parameters:
//   ticks = how far to drive (in encoder counts)
//   speed = how fast to drive (0-500)

void drive(int ticks, int speed) {
  // Reset encoder counters to zero
  encoders.getCountsAndResetLeft();
  encoders.getCountsAndResetRight();
  
  // Keep driving until we've gone far enough
  while (encoders.getCountsLeft() < ticks) {
    motors.setSpeeds(speed, speed);  // Both wheels same speed = straight
    if (buttonB.isPressed()) break;  // Let user stop early
  }
  
  motors.setSpeeds(0, 0);  // Stop motors
  delay(200);              // Wait for robot to settle
}

// ═══════════════════════════════════════════════════════════════════════════
// HELPER FUNCTION: Turn Right 90 Degrees
// ═══════════════════════════════════════════════════════════════════════════

void turnR() {
  // Reset encoder counters
  encoders.getCountsAndResetLeft();
  encoders.getCountsAndResetRight();
  
  // Keep turning until we've turned 90 degrees
  while (abs(encoders.getCountsLeft()) < TURN_90) {
    motors.setSpeeds(250, -250);  // Left forward, right backward = turn right
    if (buttonB.isPressed()) break;
  }
  
  motors.setSpeeds(0, 0);  // Stop
  delay(200);
}

// ═══════════════════════════════════════════════════════════════════════════
// HELPER FUNCTION: Turn Left 90 Degrees
// ═══════════════════════════════════════════════════════════════════════════

void turnL() {
  // Reset encoder counters
  encoders.getCountsAndResetLeft();
  encoders.getCountsAndResetRight();
  
  // Keep turning until we've turned 90 degrees
  while (abs(encoders.getCountsLeft()) < TURN_90) {
    motors.setSpeeds(-250, 250);  // Left backward, right forward = turn left
    if (buttonB.isPressed()) break;
  }
  
  motors.setSpeeds(0, 0);  // Stop
  delay(200);
}

// ═══════════════════════════════════════════════════════════════════════════
// MODE 1: SQUARE SPIRAL
// ═══════════════════════════════════════════════════════════════════════════
// Robot drives in a square pattern that gets smaller and smaller
// 
// HOW IT WORKS:
// 1. Start with a big square (side = 1400 ticks)
// 2. Drive one side, turn right 90°
// 3. Make next side a bit shorter (subtract 70 ticks)
// 4. Repeat until square is too small (side < 150 ticks)
// 
// RESULT: Creates an inward square spiral pattern

void squareSpiral() {
  int side = 1400;  // Start with big square (about 140cm)
  
  // Keep making squares until too small
  while (side > 150) {
    drive(side, 400);  // Drive one side (speed = 400)
    turnR();           // Turn right 90 degrees
    side = side - 70;  // Make next side shorter
    
    if (buttonB.isPressed()) break;  // Let user stop
  }
  
  motors.setSpeeds(0, 0);  // Make sure motors are stopped
}

// ═══════════════════════════════════════════════════════════════════════════
// MODE 2: ZIG-ZAG
// ═══════════════════════════════════════════════════════════════════════════
// Robot drives in a zigzag pattern like: /\/\/\
// 
// HOW IT WORKS:
// 1. Drive forward
// 2. Turn right a bit (about 67 degrees)
// 3. Drive forward
// 4. Turn left a lot (about 134 degrees) - this reverses direction
// 5. Repeat 6 times
// 
// RESULT: Creates a zigzag pattern

void zigzag() {
  int angle = 480;  // Turn angle (about 67 degrees)
  
  // Do 6 zigzag segments
  for (int i = 0; i < 6; i++) {
    drive(900, 400);  // Drive forward (900 ticks, speed 400)
    delay(100);
    
    // Alternate between right and left turns
    if (i % 2 == 0) {
      // Even numbers (0,2,4): Turn right
      encoders.getCountsAndResetLeft();
      while (abs(encoders.getCountsLeft()) < angle) {
        motors.setSpeeds(400, -400);  // Turn right
        if (buttonB.isPressed()) break;
      }
      motors.setSpeeds(0, 0);
      delay(100);
    } else {
      // Odd numbers (1,3,5): Turn left (double angle to reverse)
      encoders.getCountsAndResetLeft();
      while (abs(encoders.getCountsLeft()) < angle * 2) {
        motors.setSpeeds(-400, 400);  // Turn left
        if (buttonB.isPressed()) break;
      }
      motors.setSpeeds(0, 0);
      delay(100);
    }
    
    if (buttonB.isPressed()) break;
  }
  
  motors.setSpeeds(0, 0);
}

// ═══════════════════════════════════════════════════════════════════════════
// MODE 3: SQUARE ZIG-ZAG (Vertical Lines)
// ═══════════════════════════════════════════════════════════════════════════
// Robot creates vertical parallel lines like: | | |
// 
// HOW IT WORKS:
// 1. Drive up (1400 ticks)
// 2. Turn right, move sideways (400 ticks), turn right again
//    → Now facing down
// 3. Drive down (1400 ticks)
// 4. Turn left, move sideways (400 ticks), turn left again
//    → Now facing up
// 5. Repeat for 3 columns total
// 
// RESULT: Creates 3 vertical parallel lines

void squareZigzag() {
  int height = 1400;    // Height of each line
  int spacing = 400;    // Distance between lines
  
  // Create 3 vertical lines
  for (int i = 0; i < 3; i++) {
    drive(height, 350);  // Drive up (or down)
    delay(150);
    
    // Move to next column (unless this is the last one)
    if (i < 2) {  // Only do this for first 2 columns
      if (i == 0) {
        // After 1st column: Turn right, move, turn right (face down)
        turnR();
        delay(150);
        drive(spacing, 350);
        delay(150);
        turnR();
        delay(150);
      } else {
        // After 2nd column: Turn left, move, turn left (face up)
        turnL();
        delay(150);
        drive(spacing, 350);
        delay(150);
        turnL();
        delay(150);
      }
    }
    
    if (buttonB.isPressed()) break;
  }
  
  motors.setSpeeds(0, 0);
}

// ═══════════════════════════════════════════════════════════════════════════
// MODE 4: CIRCLE
// ═══════════════════════════════════════════════════════════════════════════
// Robot drives in a perfect circle
// 
// HOW IT WORKS:
// 1. Set left wheel to speed 450 (faster)
// 2. Set right wheel to speed 250 (slower)
// 3. Because left wheel is faster, robot curves to the right
// 4. Run for 7 seconds (7000 milliseconds)
// 5. After 7 seconds, robot completes one full circle
// 
// RESULT: Creates a circular path where end meets start

void circle() {
  unsigned long start = millis();  // Record start time (in milliseconds)
  
  // Keep driving in circle for 7 seconds
  while (millis() - start < 7000) {
    motors.setSpeeds(450, 250);  // Left faster than right = circle
    if (buttonB.isPressed()) break;
  }
  
  motors.setSpeeds(0, 0);  // Stop
}

// ═══════════════════════════════════════════════════════════════════════════
// END OF PROGRAM
// ═══════════════════════════════════════════════════════════════════════════
// 
// SUMMARY FOR VIVA:
// ─────────────────
// 1. Program starts with setup() - initializes robot
// 2. Then loop() runs forever:
//    - Shows menu
//    - User picks mode with buttons
//    - Robot performs selected pattern
//    - Shows "Done!"
//    - Repeats
// 
// 3. Four main patterns:
//    - Square Spiral: Square that gets smaller
//    - Zig-Zag: Diagonal zigzag pattern
//    - Square Zig-Zag: Vertical parallel lines
//    - Circle: Perfect circular path
// 
// 4. Helper functions:
//    - drive() = drive straight
//    - turnR() = turn right 90°
//    - turnL() = turn left 90°
//    - showMenu() = display menu and get user choice
// 
// TOTAL LINES: ~300 (much shorter and easier to explain!)
// ═══════════════════════════════════════════════════════════════════════════
