/*
 * Hardware.cpp - Hardware Interface Implementation
 * 
 * ========== WHAT THIS FILE DOES ==========
 * This file implements all hardware-related functions for the Zumo32U4 robot.
 * It creates and initializes all hardware objects (motors, sensors, display, etc.)
 * and provides helper functions for controlling them.
 * 
 * ========== KEY CONCEPTS ==========
 * - Object instantiation: Creating actual hardware objects (not just declaring them)
 * - Initialization: Setting up hardware before use
 * - Abstraction: Hiding hardware complexity behind simple functions
 * 
 * ========== HARDWARE COMPONENTS ==========
 * - Motors: Two DC motors for left and right wheels
 * - Encoders: Magnetic sensors that count wheel rotations
 * - Line Sensors: 5 IR sensors on bottom for line detection
 * - Proximity Sensors: 3 IR sensors on front for obstacle detection
 * - Display: 128x64 OLED screen for showing information
 * - Buzzer: Speaker for sound effects and feedback
 * - Buttons: 3 buttons (A, B, C) for user input
 * - IMU: Inertial Measurement Unit (accelerometer + gyroscope)
 */

#include "Hardware.h"
#include "Config.h"

// ============ HARDWARE INSTANCES ============
// These lines CREATE the actual hardware objects
// They are declared as "extern" in Hardware.h so other files can use them

Zumo32U4Motors motors;              // Motor controller object
                                    // Controls both left and right motors
                                    // Methods: setSpeeds(left, right)
                                    
Zumo32U4Encoders encoders;          // Encoder reader object
                                    // Reads wheel rotation counts
                                    // Methods: getCountsLeft(), getCountsRight()
                                    
Zumo32U4LineSensors lineSensors;    // Line sensor array object
                                    // 5 IR sensors for line following
                                    // Methods: read(array), calibrate()
                                    
Zumo32U4ProximitySensors prox;      // Proximity sensor object
                                    // 3 IR sensors for obstacle detection
                                    // Methods: read(), countsLeft(), etc.
                                    
Zumo32U4OLED display;               // OLED display object
                                    // 128x64 pixel monochrome screen
                                    // Methods: print(), clear(), gotoXY()
                                    
Zumo32U4Buzzer buzzer;              // Buzzer object
                                    // Piezo speaker for sound
                                    // Methods: play(notes), playFrequency()
                                    
Zumo32U4ButtonA buttonA;            // Button A object (left button)
Zumo32U4ButtonB buttonB;            // Button B object (middle button)
Zumo32U4ButtonC buttonC;            // Button C object (right button)
                                    // Methods: isPressed(), getSingleDebouncedPress()
                                    
Zumo32U4IMU imu;                    // IMU object
                                    // Accelerometer + gyroscope for tilt/motion sensing
                                    // Methods: read(), init(), enableDefault()

// ============ SENSOR DATA ============
// Global variables to store sensor readings
// These are shared across all files that include Hardware.h

unsigned int lineValues[5];         // Array to store 5 line sensor readings
                                    // Values range from 0 (bright) to 2000 (dark)
                                    // Index: [0]=leftmost, [1], [2]=center, [3], [4]=rightmost
                                    
int lastError = 0;                  // Last line following error value
                                    // Used for PID derivative calculation
                                    // Range: -4 to +4 (negative=left, positive=right)

// ============ HARDWARE FUNCTIONS ============

/*
 * initializeHardware() - Initialize all hardware components
 * 
 * WHAT IT DOES:
 * 1. Initializes the OLED display and shows startup message
 * 2. Initializes the IMU (accelerometer + gyroscope)
 * 3. Waits 1 second for everything to stabilize
 * 
 * WHEN TO CALL:
 * MUST be called in setup() before using any hardware.
 * Calling it multiple times is safe but unnecessary.
 * 
 * WHY WE NEED IT:
 * Hardware needs initialization before use. Without this:
 * - Display won't show anything
 * - IMU won't provide readings
 * - System may behave unpredictably
 * 
 * NOTE:
 * Line sensors and proximity sensors are initialized per-mode as needed
 * (not all modes use them, so we save time by not initializing them here)
 */
void initializeHardware() {
  // Initialize display and show startup message
  display.clear();                  // Clear any previous content
  display.print("ZumoOS Pro");      // Show program name
  display.gotoXY(0, 1);             // Move to line 1 (0-indexed)
  display.print("Initializing");    // Show status message
  
  // Initialize I2C communication for IMU
  Wire.begin();                     // Start I2C bus (required for IMU)
  
  // Initialize IMU (Inertial Measurement Unit)
  if (imu.init()) {                 // Try to initialize IMU
    imu.enableDefault();            // Enable with default settings
                                    // If init fails, IMU functions won't work but program continues
  }
  
  // Note: Line sensors and proximity sensors are initialized per-mode
  // Uncomment these lines if you want to initialize them here:
  // lineSensors.initFiveSensors();
  // prox.initThreeSensors();
  
  // Wait for initialization to complete
  delay(1000);                      // 1 second delay
  display.clear();                  // Clear display for next screen
}

/*
 * playStartupSound() - Play sound when robot powers on
 * 
 * Plays a rising tone sequence (C-E-G-C) to indicate successful startup.
 * Provides audio feedback that the system is ready.
 * 
 * MUSIC NOTATION:
 * - c32 = C note, 32nd note duration
 * - e32 = E note, 32nd note duration
 * - g32 = G note, 32nd note duration
 * - c64 = C note (higher octave), 64th note duration
 * 
 * Numbers after notes indicate octave (4=middle C by default)
 */
void playStartupSound() {
  buzzer.play("c32 e32 g32 c64");  // Play C major arpeggio
}

/*
 * playModeChangeSound() - Play sound when mode changes
 * 
 * Plays a short two-note beep to confirm mode selection change.
 * Provides audio feedback when user cycles through menu.
 */
void playModeChangeSound() {
  buzzer.play("g32 c32");           // Quick two-note beep
}

/*
 * playErrorSound() - Play sound for errors
 * 
 * Plays three low C notes to indicate something went wrong.
 * Used for error conditions or failed operations.
 */
void playErrorSound() {
  buzzer.play("c16 c16 c16");       // Three short low beeps
}

/*
 * playSuccessSound() - Play sound for successful completion
 * 
 * Plays a rising tone sequence to indicate successful task completion.
 * Used when calibration completes or mode finishes successfully.
 */
void playSuccessSound() {
  buzzer.play("g32 a32 b32 c64");   // Rising scale
}

// ============ MOTOR HELPERS ============

/*
 * clampSpeed(speed) - Limit motor speed to safe range
 * 
 * @param speed: Desired motor speed (-500 to 500)
 * @return: Clamped speed value
 * 
 * NORMAL BEHAVIOR:
 * Would limit speed to safe range (-400 to 400) to protect motors
 * 
 * CURRENT BEHAVIOR (LUDICROUS MODE):
 * Returns speed unchanged - NO LIMITS!
 * Allows speeds up to 500 for maximum performance
 * 
 * WARNING:
 * Speeds above 400 may:
 * - Damage motors over time
 * - Cause loss of control
 * - Drain battery faster
 * - Reduce encoder accuracy
 * 
 * WHY NO CLAMPING:
 * For SBA assignment demonstration, we want maximum speed.
 * In production code, you should clamp to 400.
 * 
 * TO ENABLE SAFE MODE:
 * Replace "return speed;" with:
 *   if (speed > 400) return 400;
 *   if (speed < -400) return -400;
 *   return speed;
 */
int clampSpeed(int speed) {
  // LUDICROUS SPEED MODE - No limits!
  return speed;  // Return speed unchanged (no clamping)
}

/*
 * stopMotors() - Immediately stop both motors
 * 
 * Sets both motor speeds to 0, bringing robot to a halt.
 * 
 * WHEN TO USE:
 * - End of movement sequence
 * - Emergency stop
 * - Before changing direction
 * - When user presses stop button
 * 
 * NOTE:
 * Robot may coast slightly after stopping due to momentum.
 * On smooth surfaces, this can be several centimeters.
 */
void stopMotors() {
  motors.setSpeeds(0, 0);           // Set both motors to speed 0
}

/*
 * setMotorSpeeds(left, right) - Set motor speeds with safety clamping
 * 
 * @param left: Left motor speed (-500 to 500)
 * @param right: Right motor speed (-500 to 500)
 * 
 * WHAT IT DOES:
 * Sets the speed of left and right motors independently.
 * 
 * CURRENT BEHAVIOR (LUDICROUS MODE):
 * Directly sets speeds without clamping - allows full range!
 * 
 * NORMAL BEHAVIOR:
 * Would clamp speeds using clampSpeed() function for safety.
 * 
 * SPEED VALUES:
 * - Positive: Forward direction
 * - Negative: Reverse direction
 * - 0: Stopped
 * - Typical range: -400 to 400 (safe)
 * - Ludicrous range: -500 to 500 (maximum)
 * 
 * DIFFERENTIAL DRIVE:
 * Different left/right speeds create turns:
 * - Same speed: Straight line
 * - Different speeds: Curved path
 * - Opposite signs: Spin in place
 */
void setMotorSpeeds(int left, int right) {
  motors.setSpeeds(left, right);    // Direct control, no clamping!
}