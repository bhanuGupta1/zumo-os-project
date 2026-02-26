/*
 * ZumoOS.ino - Main Program Entry Point
 * 
 * ========== ASSIGNMENT INFORMATION ==========
 * Author: Bhanu
 * Date: February 27, 2026
 * Assignment: SBA - Ballistic Behaviors for Autonomous Robot
 * 
 * ========== WHAT THIS FILE DOES ==========
 * This is the MAIN file of the Zumo robot program. It's the entry point
 * that Arduino runs when the robot powers on. This file coordinates all
 * other components of the system.
 * 
 * ========== PROGRAM STRUCTURE ==========
 * Arduino programs have two required functions:
 * 1. setup() - Runs ONCE when robot powers on
 * 2. loop() - Runs CONTINUOUSLY forever after setup()
 * 
 * ========== WHAT THIS PROGRAM DOES ==========
 * This program implements 5 ballistic behaviors for autonomous robot movement:
 * 
 * 1. SPIRAL - Circular spiral with selectable direction (inward/outward)
 *    Robot creates smooth curved spiral, user chooses direction
 * 
 * 2. SQUARE SPIRAL - Square spiral with 90° turns and selectable direction
 *    Robot creates square spiral with straight lines and corners
 * 
 * 3. ZIG-ZAG - Alternating diagonal movements
 *    Robot creates "Z" or lightning bolt pattern
 * 
 * 4. LAWN MOWER (Square Zig-Zag) - Parallel rows pattern
 *    Robot covers rectangular area like mowing a lawn
 * 
 * 5. CIRCLE - Single continuous circular path
 *    Robot drives in perfect circle, endpoint meets startpoint
 * 
 * ========== HOW IT WORKS ==========
 * 1. Robot powers on → setup() runs
 * 2. Hardware initializes (motors, sensors, display)
 * 3. Startup sound plays
 * 4. loop() starts running forever:
 *    a. Show menu on display
 *    b. Wait for user to select mode with buttons
 *    c. Run selected ballistic behavior
 *    d. Show completion message
 *    e. Repeat (go back to menu)
 * 
 * ========== FILE ORGANIZATION ==========
 * This project is split into multiple files for organization:
 * - ZumoOS.ino: Main program (this file)
 * - Hardware.h/cpp: Hardware initialization and control
 * - Menu.h/cpp: User interface for mode selection
 * - Modes.h/cpp: Ballistic behavior implementations
 * - Utils.h/cpp: Helper functions (movement, sensors, etc.)
 * - Config.h: Configuration constants (speeds, timings, etc.)
 * 
 * ========== KEY CONCEPTS ==========
 * - #include: Brings in code from other files
 * - setup(): Initialization function (runs once)
 * - loop(): Main program loop (runs forever)
 * - Functions: Reusable blocks of code (like selectMode(), runMode())
 * - Comments: Text that explains code (ignored by compiler)
 */

#include "Hardware.h"  // Hardware initialization and motor control functions
#include "Menu.h"      // User interface for mode selection
#include "Modes.h"     // Ballistic behavior implementations
#include "Config.h"    // Configuration constants (speeds, timings, etc.)

/*
 * setup() - Arduino Initialization Function
 * 
 * WHEN IT RUNS:
 * This function runs ONCE when the robot is powered on or reset button is pressed.
 * 
 * WHAT IT DOES:
 * 1. Initializes serial communication (for debugging via USB)
 * 2. Initializes all hardware (motors, sensors, display, buttons)
 * 3. Plays startup sound (confirms system is ready)
 * 4. Waits briefly for system to stabilize
 * 
 * WHY WE NEED IT:
 * Hardware must be initialized before use. Without this, motors won't work,
 * display won't show anything, and sensors won't read correctly.
 * 
 * DEBUGGING:
 * Serial.begin(9600) allows sending debug messages to computer via USB.
 * Use Serial.println("message") anywhere in code to debug.
 */
void setup() {
  Serial.begin(9600);        // Initialize serial communication at 9600 baud
                             // Allows debugging via Serial Monitor in Arduino IDE
                             
  initializeHardware();      // Initialize motors, sensors, display, and IMU
                             // Defined in Hardware.cpp
                             // MUST be called before using any hardware
                             
  playStartupSound();        // Play a sound to indicate successful startup
                             // Provides audio feedback that robot is ready
                             // Defined in Hardware.cpp
                             
  delay(500);                // Brief pause for system stabilization (500 ms = 0.5 seconds)
                             // Gives hardware time to fully initialize
}

/*
 * loop() - Arduino Main Loop Function
 * 
 * WHEN IT RUNS:
 * This function runs CONTINUOUSLY forever after setup() completes.
 * It never exits - when it reaches the end, it immediately starts again from the top.
 * 
 * WHAT IT DOES:
 * 1. Shows menu on display with available modes
 * 2. Waits for user to select a mode using buttons A/B/C
 * 3. Executes the selected ballistic behavior
 * 4. Shows completion message
 * 5. Repeats (goes back to step 1)
 * 
 * PROGRAM FLOW:
 * loop() → selectMode() → runMode() → display "Done" → loop() → ...
 * 
 * WHY IT'S A LOOP:
 * This allows robot to run multiple behaviors without resetting.
 * User can select and run different modes repeatedly.
 * 
 * HOW TO STOP:
 * - Press Button B during behavior execution
 * - Power off the robot
 * - Press reset button (restarts from setup())
 */
void loop() {
  // Step 1: Show menu and get user's mode choice
  // selectMode() displays menu and waits for button press
  // Returns a Mode enum value (MODE_SQUARE_SPIRAL, MODE_ZIGZAG, etc.)
  Mode selectedMode = selectMode();
  
  // Step 2: Execute the selected ballistic behavior
  // runMode() looks at the Mode value and calls the appropriate function
  // (modeSquareSpiral(), modeZigzag(), modeSquareZigzag(), or modeCircle())
  runMode(selectedMode);
  
  // Step 3: Show completion message
  display.clear();              // Clear the OLED display
  display.print("Mode Done!");  // Show completion message
  delay(1000);                  // Wait 1 second so user can see message
  
  // Step 4: Loop automatically repeats (goes back to selectMode())
}
