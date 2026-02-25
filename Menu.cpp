/*
 * Menu.cpp - User Interface for Mode Selection
 * 
 * This file implements the menu system that allows users to select
 * which ballistic behavior to run. It handles button input and
 * displays the current selection on the OLED screen.
 * 
 * User Interface:
 * - Button A: Cycle forward through modes
 * - Button C: Cycle backward through modes
 * - Button B: Confirm selection and run mode
 */

#include "Menu.h"
#include "Hardware.h"
#include "Config.h"

/*
 * Mode Names Array
 * Stores short display names for each mode (max 16 chars for OLED)
 * Array index corresponds to Mode enum values
 */
static const char* modeNames[] = {
  "Sq Spiral",    // MODE_SQUARE_SPIRAL
  "Zig-Zag",      // MODE_ZIGZAG
  "Sq Zig-Zag",   // MODE_SQUARE_ZIGZAG
  "Circle"        // MODE_CIRCLE
};

/*
 * getModeName() - Get display name for a mode
 * 
 * @param mode: The mode enum value
 * @return: Pointer to the mode's display name string
 * 
 * Safety: Returns "Unknown" if mode is out of range
 */
const char* getModeName(Mode mode) {
  if (mode >= 0 && mode < MODE_COUNT) {
    return modeNames[mode];
  }
  return "Unknown";
}

/*
 * showMenu() - Display menu header
 * 
 * Shows the control instructions on the OLED display:
 * Line 0: "A/C:Chg B:Run" (button instructions)
 * Line 1: Will show current mode name
 */
void showMenu() {
  display.clear();
  display.print("A/C:Chg B:Run");  // Compact instructions
}

/*
 * selectMode() - Interactive mode selection menu
 * 
 * Displays a menu allowing the user to cycle through available modes
 * and select one to run. The selected mode persists between runs
 * using a static variable.
 * 
 * Controls:
 * - Button A: Next mode (cycles forward)
 * - Button C: Previous mode (cycles backward)
 * - Button B: Confirm and run selected mode
 * 
 * @return: The selected Mode enum value
 * 
 * Implementation Details:
 * - Uses static variable to remember last selected mode
 * - Modulo arithmetic for wraparound cycling
 * - Only updates display when mode changes (reduces flicker)
 * - Debounced button presses prevent accidental double-presses
 */
Mode selectMode() {
  // Static variable persists between function calls
  // Remembers the last selected mode
  static Mode currentMode = MODE_SQUARE_SPIRAL;
  
  // Display menu header
  showMenu();
  
  // Display initial mode immediately on line 1
  display.gotoXY(0, 1);  // Move cursor to line 1
  display.print(getModeName(currentMode));
  delay(100); // Give display time to refresh
  
  // Track last displayed mode to avoid unnecessary redraws
  Mode lastDisplayedMode = currentMode;
  
  // Main menu loop - runs until user presses Button B
  while (true) {
    // Update display only if mode changed (reduces flicker)
    if (currentMode != lastDisplayedMode) {
      display.gotoXY(0, 1);
      display.print("                "); // Clear line (16 spaces for OLED width)
      display.gotoXY(0, 1);
      display.print(getModeName(currentMode));
      lastDisplayedMode = currentMode;
    }
    
    // Handle Button A - Cycle forward through modes
    if (buttonA.getSingleDebouncedPress()) {
      // Increment mode and wrap around using modulo
      currentMode = (Mode)((currentMode + 1) % MODE_COUNT);
      playModeChangeSound();  // Audio feedback
    }
    
    // Handle Button C - Cycle backward through modes
    if (buttonC.getSingleDebouncedPress()) {
      // Decrement mode with wraparound
      // Add MODE_COUNT before modulo to handle negative numbers correctly
      currentMode = (Mode)((currentMode - 1 + MODE_COUNT) % MODE_COUNT);
      playModeChangeSound();  // Audio feedback
    }
    
    // Handle Button B - Confirm selection
    if (buttonB.getSingleDebouncedPress()) {
      displayModeInfo(currentMode);  // Show confirmation screen
      return currentMode;             // Exit menu and return selection
    }
    
    // Small delay to prevent excessive CPU usage
    delay(50);
  }
}

/*
 * displayModeInfo() - Show mode confirmation screen
 * 
 * Displays a confirmation message before starting the selected mode.
 * Gives the user time to prepare and shows how to stop the mode.
 * 
 * @param mode: The mode that is about to start
 * 
 * Display format:
 * Line 0: "Starting:"
 * Line 1: Mode name
 * Line 2: "Press B=Stop"
 */
void displayModeInfo(Mode mode) {
  display.clear();
  display.print("Starting:");
  display.gotoXY(0, 1);
  display.print(getModeName(mode));
  display.gotoXY(0, 2);
  display.print("Press B=Stop");  // Remind user how to stop
  delay(1000);  // 1 second delay before mode starts
}
