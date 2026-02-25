/*
 * ZumoOS - Ballistic Behavior Robot Control System
 * 
 * This is the main entry point for the Zumo robot operating system.
 * It implements 4 ballistic behaviors for autonomous robot movement:
 * - Square Spiral: Robot drives in an expanding square pattern
 * - Zig-Zag: Robot moves in alternating diagonal directions
 * - Square Zig-Zag: Robot covers an area in parallel lines (like mowing a lawn)
 * - Circle: Robot drives in a perfect circular path
 * 
 * Author: [Your Name]
 * Date: November 2025
 * Course: IA7621001 Automation and Robotics
 */

#include "Hardware.h"  // Hardware initialization and motor control functions
#include "Menu.h"      // User interface for mode selection
#include "Modes.h"     // Ballistic behavior implementations
#include "Config.h"    // Configuration constants (speeds, timings, etc.)

/*
 * setup() - Arduino initialization function
 * Runs once when the robot is powered on or reset
 */
void setup() {
  Serial.begin(9600);        // Initialize serial communication at 9600 baud
  initializeHardware();      // Initialize motors, sensors, display, and IMU
  playStartupSound();        // Play a sound to indicate successful startup
  delay(500);                // Brief pause for system stabilization
}

/*
 * loop() - Arduino main loop function
 * Runs continuously after setup() completes
 * 
 * Flow:
 * 1. Display menu and wait for user to select a mode
 * 2. Execute the selected ballistic behavior
 * 3. Display completion message
 * 4. Repeat (return to mode selection)
 */
void loop() {
  Mode selectedMode = selectMode();  // Show menu and get user's mode choice
  runMode(selectedMode);             // Execute the selected ballistic behavior
  
  // Show completion message
  display.clear();
  display.print("Mode Done!");
  delay(1000);
}
