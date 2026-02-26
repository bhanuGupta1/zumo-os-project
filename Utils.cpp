/*
 * Utils.cpp - Utility Functions Implementation
 * 
 * ========== WHAT THIS FILE DOES ==========
 * This file implements helper functions used throughout the project.
 * These are reusable functions that provide common functionality like:
 * - Sensor calibration
 * - Encoder-based movement (precise distance and turns)
 * - Math helpers (mapping, constraining values)
 * - Timing utilities (timeouts, delays)
 * - System diagnostics
 * 
 * ========== WHY SEPARATE FILE ==========
 * Keeping utility functions in a separate file:
 * - Makes code more organized and readable
 * - Allows reusing functions across multiple modes
 * - Makes testing and debugging easier
 * - Follows "DRY" principle (Don't Repeat Yourself)
 * 
 * ========== KEY CONCEPTS ==========
 * - Encoder-based movement: Using wheel rotation counts for precision
 * - Calibration: Teaching sensors to recognize light/dark surfaces
 * - Blocking functions: Functions that wait until complete
 * - Non-blocking checks: Functions that return immediately
 */

#include "Utils.h"
#include "Hardware.h"
#include "Config.h"

// ============ UTILITY FUNCTIONS ============

/*
 * waitForButtonPress() - Wait for any button to be pressed
 * 
 * WHAT IT DOES:
 * Displays "Press any btn" message and blocks until user presses
 * and releases any of the three buttons (A, B, or C).
 * 
 * WHEN TO USE:
 * - Before starting calibration
 * - Waiting for user confirmation
 * - Pausing between test sequences
 * 
 * HOW IT WORKS:
 * 1. Shows message on display line 3
 * 2. Loops until any button is pressed
 * 3. Waits for button to be released (prevents double-trigger)
 * 4. Returns control to calling function
 * 
 * NOTE: This is a BLOCKING function - program waits here until button press
 */
void waitForButtonPress() {
  display.gotoXY(0, 3);             // Move cursor to line 3 (bottom line)
  display.print("Press any btn");   // Show instruction message
  
  // Wait loop - continues until any button is pressed
  while (!buttonA.isPressed() && !buttonB.isPressed() && !buttonC.isPressed()) {
    delay(10);                      // Small delay to prevent excessive CPU usage
  }
  
  // Wait for button release - prevents double-triggering
  while (buttonA.isPressed() || buttonB.isPressed() || buttonC.isPressed()) {
    delay(10);                      // Small delay
  }
}

/*
 * calibrateLineSensors() - Calibrate line sensors
 * 
 * WHAT IT DOES:
 * Spins the robot while reading line sensors to establish minimum
 * and maximum values for each sensor. This allows accurate detection
 * of black lines on white surfaces (or vice versa).
 * 
 * WHY CALIBRATION IS NEEDED:
 * - Lighting conditions vary (bright room vs dim room)
 * - Surface reflectivity varies (glossy vs matte)
 * - Sensor sensitivity varies between units
 * - Calibration adapts to current conditions
 * 
 * HOW IT WORKS:
 * 1. Initializes 5 line sensors
 * 2. Spins robot in place (one wheel forward, one backward)
 * 3. Takes multiple sensor readings while spinning
 * 4. Records minimum (brightest) and maximum (darkest) values
 * 5. Uses these values to normalize future readings
 * 
 * WHEN TO USE:
 * - Before line following modes
 * - When lighting conditions change
 * - When moving to different surface
 * 
 * USER INSTRUCTIONS:
 * Place robot so sensors see both black line and white surface during spin
 */
void calibrateLineSensors() {
  display.clear();
  display.print("Calibrating");
  display.gotoXY(0, 1);
  display.print("Line Sensors");
  
  // Initialize the 5 line sensors
  lineSensors.initFiveSensors();
  
  // Spin robot for calibration
  // Left motor forward, right motor backward = clockwise spin
  motors.setSpeeds(150, -150);
  
  // Take multiple calibration readings while spinning
  for (int i = 0; i < CALIBRATION_SPINS; i++) {
    lineSensors.calibrate();        // Read sensors and update min/max values
    delay(CALIBRATION_DELAY_MS);    // Wait between readings
  }
  
  // Stop spinning
  motors.setSpeeds(0, 0);
  
  // Show completion message
  display.gotoXY(0, 2);
  display.print("Complete!");
  playSuccessSound();               // Audio feedback
  delay(500);
}

/*
 * calibrateProximitySensors() - Initialize proximity sensors
 * 
 * WHAT IT DOES:
 * Initializes the 3 front-facing IR proximity sensors used for
 * obstacle detection.
 * 
 * HOW PROXIMITY SENSORS WORK:
 * - Emit IR light pulses
 * - Measure reflected light
 * - More reflection = closer object
 * - Return values 0-6 (0=nothing, 6=very close)
 * 
 * WHEN TO USE:
 * - Before obstacle avoidance modes
 * - Before sumo mode
 * - Before wall following
 * 
 * NOTE:
 * Unlike line sensors, proximity sensors don't need calibration spin.
 * They just need to be initialized.
 */
void calibrateProximitySensors() {
  display.clear();
  display.print("Calibrating");
  display.gotoXY(0, 1);
  display.print("Prox Sensors");
  
  // Initialize the 3 proximity sensors (left, front, right)
  prox.initThreeSensors();
  
  // Show completion message
  display.gotoXY(0, 2);
  display.print("Complete!");
  playSuccessSound();               // Audio feedback
  delay(500);
}

/*
 * showSensorReadings() - Display live sensor values on OLED
 * 
 * WHAT IT DOES:
 * Reads all sensors and displays their current values on the OLED screen.
 * Useful for debugging and verifying sensor operation.
 * 
 * DISPLAY FORMAT:
 * Line 0: L:[val] [val]  (Left two line sensors)
 * Line 1: M:[val]        (Middle line sensor)
 * Line 2: R:[val] [val]  (Right two line sensors)
 * Line 3: P:[L] [F] [R]  (Proximity: Left, Front, Right)
 * 
 * LINE SENSOR VALUES:
 * - Range: 0-2000
 * - 0 = very bright (white surface)
 * - 2000 = very dark (black line)
 * 
 * PROXIMITY SENSOR VALUES:
 * - Range: 0-6
 * - 0 = no object detected
 * - 6 = object very close
 * 
 * WHEN TO USE:
 * - Debugging sensor issues
 * - Verifying calibration
 * - Testing sensor placement
 * - Checking for sensor damage
 */
void showSensorReadings() {
  // Read all line sensors into lineValues array
  lineSensors.read(lineValues);
  
  // Read all proximity sensors (stores internally)
  prox.read();
  
  // Display line sensor values
  display.clear();
  display.print("L:");                      // Left sensors label
  display.print(lineValues[0]);             // Leftmost sensor
  display.print(" ");
  display.print(lineValues[1]);             // Left-center sensor
  
  display.gotoXY(0, 1);
  display.print("M:");                      // Middle sensor label
  display.print(lineValues[2]);             // Center sensor
  
  display.gotoXY(0, 2);
  display.print("R:");                      // Right sensors label
  display.print(lineValues[3]);             // Right-center sensor
  display.print(" ");
  display.print(lineValues[4]);             // Rightmost sensor
  
  // Display proximity sensor values
  display.gotoXY(0, 3);
  display.print("P:");                      // Proximity label
  display.print(prox.countsLeftWithLeftLeds());    // Left proximity
  display.print(" ");
  display.print(prox.countsFrontWithLeftLeds());   // Front proximity
  display.print(" ");
  display.print(prox.countsRightWithRightLeds());  // Right proximity
}

/*
 * performSystemCheck() - Test all robot systems
 * 
 * WHAT IT DOES:
 * Runs a quick diagnostic test of motors and buzzer to verify
 * everything is working correctly.
 * 
 * TEST SEQUENCE:
 * 1. Motors forward for 0.5 seconds
 * 2. Motors backward for 0.5 seconds
 * 3. Motors stop
 * 4. Buzzer plays test tone
 * 5. Success sound plays
 * 
 * WHEN TO USE:
 * - After hardware assembly
 * - After battery replacement
 * - When troubleshooting issues
 * - Before important demonstrations
 * 
 * WHAT TO LOOK FOR:
 * - Both wheels should spin forward, then backward
 * - Buzzer should make sound
 * - Display should show progress messages
 * - If anything doesn't work, check connections
 */
void performSystemCheck() {
  display.clear();
  display.print("System Check");
  
  // Test motors - forward
  display.gotoXY(0, 1);
  display.print("Motors...");
  motors.setSpeeds(100, 100);       // Both motors forward at low speed
  delay(500);                       // Run for 0.5 seconds
  
  // Test motors - backward
  motors.setSpeeds(-100, -100);     // Both motors backward
  delay(500);                       // Run for 0.5 seconds
  
  // Stop motors
  motors.setSpeeds(0, 0);
  
  // Test buzzer
  display.gotoXY(0, 2);
  display.print("Buzzer...");
  buzzer.play("c32");               // Play test tone
  delay(500);
  
  // Show completion
  display.gotoXY(0, 3);
  display.print("Complete!");
  playSuccessSound();
  delay(1000);
}

// ============ MATH HELPERS ============

/*
 * mapRange() - Map a value from one range to another
 * 
 * @param value: Input value to map
 * @param fromLow: Lower bound of input range
 * @param fromHigh: Upper bound of input range
 * @param toLow: Lower bound of output range
 * @param toHigh: Upper bound of output range
 * @return: Mapped value in output range
 * 
 * WHAT IT DOES:
 * Converts a value from one range to another proportionally.
 * Similar to Arduino's built-in map() function.
 * 
 * EXAMPLE:
 * mapRange(50, 0, 100, 0, 200) returns 100
 * (50 is halfway in 0-100 range, so result is halfway in 0-200 range)
 * 
 * COMMON USES:
 * - Converting sensor readings to motor speeds
 * - Scaling joystick input to motor output
 * - Converting percentages to actual values
 * 
 * FORMULA:
 * output = (value - fromLow) * (toHigh - toLow) / (fromHigh - fromLow) + toLow
 */
int mapRange(int value, int fromLow, int fromHigh, int toLow, int toHigh) {
  return (value - fromLow) * (toHigh - toLow) / (fromHigh - fromLow) + toLow;
}

/*
 * constrainFloat() - Limit a float value to a range
 * 
 * @param value: Value to constrain
 * @param min: Minimum allowed value
 * @param max: Maximum allowed value
 * @return: Constrained value
 * 
 * WHAT IT DOES:
 * Ensures a value stays within specified minimum and maximum bounds.
 * If value is below min, returns min.
 * If value is above max, returns max.
 * Otherwise, returns value unchanged.
 * 
 * EXAMPLE:
 * constrainFloat(150.5, 0.0, 100.0) returns 100.0
 * constrainFloat(50.5, 0.0, 100.0) returns 50.5
 * constrainFloat(-10.5, 0.0, 100.0) returns 0.0
 * 
 * COMMON USES:
 * - Preventing motor speeds from exceeding limits
 * - Keeping PID output in valid range
 * - Ensuring sensor values are reasonable
 */
float constrainFloat(float value, float min, float max) {
  if (value < min) return min;      // Value too low, return minimum
  if (value > max) return max;      // Value too high, return maximum
  return value;                     // Value in range, return unchanged
}

// ============ TIMING HELPERS ============

/*
 * isTimeoutReached() - Check if a timeout has elapsed
 * 
 * @param startTime: Start time in milliseconds (from millis())
 * @param timeoutMs: Timeout duration in milliseconds
 * @return: true if timeout elapsed, false otherwise
 * 
 * WHAT IT DOES:
 * Checks if specified time has passed since startTime.
 * Non-blocking - returns immediately with true/false.
 * 
 * HOW TO USE:
 * unsigned long startTime = millis();
 * // ... do stuff ...
 * if (isTimeoutReached(startTime, 5000)) {
 *   // 5 seconds have passed
 * }
 * 
 * WHY USE THIS:
 * - Non-blocking (doesn't stop program)
 * - More readable than manual time calculations
 * - Handles millis() overflow correctly
 * 
 * NOTE:
 * millis() returns milliseconds since program started.
 * It overflows (wraps to 0) after ~49 days.
 */
bool isTimeoutReached(unsigned long startTime, unsigned long timeoutMs) {
  return (millis() - startTime) > timeoutMs;
}

/*
 * delayWithButtonCheck() - Delay with ability to interrupt
 * 
 * @param delayMs: Delay duration in milliseconds
 * 
 * WHAT IT DOES:
 * Like delay() but can be interrupted by pressing Button B.
 * Useful for delays that user might want to skip.
 * 
 * HOW IT WORKS:
 * 1. Records start time
 * 2. Loops until delay time elapsed
 * 3. Checks Button B every 10ms
 * 4. Exits early if Button B pressed
 * 
 * WHEN TO USE:
 * - Pauses between movements
 * - Waiting for user to read display
 * - Any delay that should be skippable
 * 
 * DIFFERENCE FROM delay():
 * - delay(): Blocks for exact time, cannot interrupt
 * - delayWithButtonCheck(): Can exit early with button press
 */
void delayWithButtonCheck(unsigned long delayMs) {
  unsigned long startTime = millis();           // Record start time
  
  // Loop until timeout or button press
  while (!isTimeoutReached(startTime, delayMs)) {
    if (buttonB.isPressed()) break;             // Exit if Button B pressed
    delay(10);                                  // Small delay to reduce CPU usage
  }
}

// ============ ENCODER HELPERS ============
// These functions provide precise movement control using wheel encoders
// Encoders count wheel rotations for accurate distance and angle control

/*
 * resetEncoders() - Reset encoder counts to zero
 * 
 * WHAT IT DOES:
 * Resets both left and right encoder counts to 0.
 * 
 * WHEN TO USE:
 * Call this before starting any encoder-based movement.
 * This establishes a new "starting point" for counting.
 * 
 * WHY RESET:
 * Encoders accumulate counts continuously. Resetting allows
 * measuring distance/angle from current position.
 * 
 * EXAMPLE:
 * resetEncoders();
 * driveTicks(1000, 300);  // Drive 1000 ticks from current position
 */
void resetEncoders() {
  encoders.getCountsAndResetLeft();     // Read and reset left encoder
  encoders.getCountsAndResetRight();    // Read and reset right encoder
}

/*
 * driveTicks() - Drive forward for specified encoder ticks
 * 
 * @param ticks: Number of encoder ticks to drive
 * @param speed: Motor speed (both wheels same speed)
 * 
 * WHAT IT DOES:
 * Drives robot straight forward until both encoders reach target tick count.
 * 
 * HOW IT WORKS:
 * 1. Resets encoders to 0
 * 2. Sets both motors to same speed (straight line)
 * 3. Loops until both encoders reach target ticks
 * 4. Stops motors
 * 5. Waits 200ms for robot to settle
 * 
 * DISTANCE CALCULATION:
 * Approximately 1 tick = 0.1mm
 * So 1000 ticks ≈ 10cm
 * (Exact value depends on wheel diameter and encoder resolution)
 * 
 * INTERRUPTION:
 * Can be stopped early by pressing Button B.
 * 
 * WHEN TO USE:
 * - Driving straight segments in ballistic behaviors
 * - Moving specific distances
 * - Any time you need precise straight-line movement
 */
void driveTicks(int ticks, int speed) {
  resetEncoders();                              // Start counting from 0
  
  // Loop until both encoders reach target
  while (encoders.getCountsLeft() < ticks && encoders.getCountsRight() < ticks) {
    motors.setSpeeds(speed, speed);             // Both motors same speed = straight
    if (buttonB.isPressed()) break;             // Allow early exit
  }
  
  motors.setSpeeds(0, 0);                       // Stop motors
  delay(200);                                   // Wait for robot to settle
}

/*
 * turnRight90() - Turn exactly 90 degrees clockwise
 * 
 * WHAT IT DOES:
 * Spins robot 90 degrees to the right (clockwise) in place.
 * 
 * HOW IT WORKS:
 * 1. Resets encoders
 * 2. Sets motors to opposite speeds (left forward, right backward)
 * 3. Loops until left encoder reaches TURN_90_TICKS
 * 4. Stops motors
 * 5. Waits for robot to settle
 * 
 * WHY ONLY CHECK LEFT ENCODER:
 * When spinning in place, both wheels travel same distance.
 * Checking one encoder is sufficient.
 * 
 * CALIBRATION:
 * TURN_90_TICKS is defined in Config.h (default 320).
 * If robot turns more/less than 90°, adjust this value.
 * 
 * WHEN TO USE:
 * - Square patterns
 * - Lawn mower pattern
 * - Any time you need precise 90° right turn
 */
void turnRight90() {
  resetEncoders();                              // Start counting from 0
  
  // Loop until turn complete
  while (abs(encoders.getCountsLeft()) < TURN_90_TICKS) {
    motors.setSpeeds(MAZE_TURN_SPEED, -MAZE_TURN_SPEED);  // Spin right
    if (buttonB.isPressed()) break;             // Allow early exit
  }
  
  motors.setSpeeds(0, 0);                       // Stop motors
  delay(200);                                   // Wait for robot to settle
}

/*
 * turnLeft90() - Turn exactly 90 degrees counter-clockwise
 * 
 * WHAT IT DOES:
 * Spins robot 90 degrees to the left (counter-clockwise) in place.
 * 
 * HOW IT WORKS:
 * Same as turnRight90() but with opposite motor directions.
 * Left motor backward, right motor forward = spin left.
 * 
 * WHEN TO USE:
 * - Square patterns
 * - Lawn mower pattern
 * - Any time you need precise 90° left turn
 */
void turnLeft90() {
  resetEncoders();                              // Start counting from 0
  
  // Loop until turn complete
  while (abs(encoders.getCountsLeft()) < TURN_90_TICKS) {
    motors.setSpeeds(-MAZE_TURN_SPEED, MAZE_TURN_SPEED);  // Spin left
    if (buttonB.isPressed()) break;             // Allow early exit
  }
  
  motors.setSpeeds(0, 0);                       // Stop motors
  delay(200);                                   // Wait for robot to settle
}

/*
 * turn180() - Turn exactly 180 degrees
 * 
 * WHAT IT DOES:
 * Spins robot 180 degrees (complete reversal) in place.
 * 
 * HOW IT WORKS:
 * Same as turnRight90() but uses TURN_180_TICKS (approximately 2x TURN_90_TICKS).
 * 
 * WHEN TO USE:
 * - Reversing direction
 * - Maze solving (dead end)
 * - Any time you need to turn around
 */
void turn180() {
  resetEncoders();                              // Start counting from 0
  
  // Loop until turn complete
  while (abs(encoders.getCountsLeft()) < TURN_180_TICKS) {
    motors.setSpeeds(MAZE_TURN_SPEED, -MAZE_TURN_SPEED);  // Spin right
    if (buttonB.isPressed()) break;             // Allow early exit
  }
  
  motors.setSpeeds(0, 0);                       // Stop motors
  delay(200);                                   // Wait for robot to settle
}