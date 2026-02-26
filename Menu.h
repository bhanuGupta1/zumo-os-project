/*
 * Menu.h - Menu System Declarations
 * 
 * ========== WHAT THIS FILE DOES ==========
 * This header file declares functions for the robot's menu system.
 * The menu allows users to select which ballistic behavior to run
 * using the three buttons on the Zumo32U4.
 * 
 * ========== HOW THE MENU WORKS ==========
 * 1. Display shows current mode name
 * 2. User presses Button A or C to cycle through modes
 * 3. User presses Button B to select and run the current mode
 * 4. Robot executes the selected ballistic behavior
 * 5. After completion, menu appears again
 * 
 * ========== BUTTON FUNCTIONS ==========
 * - Button A: Previous mode (cycle backward)
 * - Button B: Select and run current mode
 * - Button C: Next mode (cycle forward)
 * 
 * ========== KEY CONCEPTS ==========
 * - Mode selection: User can choose from 4 ballistic behaviors
 * - Non-blocking: Menu responds immediately to button presses
 * - Persistent: Returns to menu after each behavior completes
 */

#pragma once
#include "Modes.h"

/*
 * showMenu() - Display the main menu on OLED
 * Shows current mode name and button instructions
 * Format: "A/C:Chg B:Run"
 *         "[Mode Name]"
 */
void showMenu();

/*
 * selectMode() - Let user select a mode using buttons
 * 
 * @return: The selected Mode enum value
 * 
 * How it works:
 * - Displays current mode name
 * - Button A/C cycle through available modes
 * - Button B confirms selection and returns
 * - Plays sound when mode changes
 */
Mode selectMode();

/*
 * displayModeInfo(mode) - Show information about a mode
 * 
 * @param mode: The mode to display info for
 * 
 * Displays the mode name on the OLED screen
 * Used during mode selection to show current choice
 */
void displayModeInfo(Mode mode);

/*
 * getModeName(mode) - Get the display name for a mode
 * 
 * @param mode: The mode enum value
 * @return: String containing the mode's display name
 * 
 * Mode names:
 * - MODE_SQUARE_SPIRAL → "Spiral"
 * - MODE_ZIGZAG → "Zig-Zag"
 * - MODE_SQUARE_ZIGZAG → "Lawn Mower"
 * - MODE_CIRCLE → "Circle"
 */
const char* getModeName(Mode mode);