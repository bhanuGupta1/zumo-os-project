/*
 * Modes.h - Ballistic Behavior Mode Definitions
 * 
 * This header file defines the available robot operating modes
 * and declares the functions that implement each behavior.
 */

#pragma once

/*
 * Mode Enumeration
 * Defines all available ballistic behaviors for the robot
 * Each mode represents a different movement pattern
 */
enum Mode {
  // BALLISTIC BEHAVIORS (SBA Assignment)
  MODE_SQUARE_SPIRAL,   // Drives in expanding square pattern with 90° turns
  MODE_ZIGZAG,          // Alternating diagonal movements (zig-zag pattern)
  MODE_SQUARE_ZIGZAG,   // Parallel lines covering a square area (lawn mowing)
  MODE_CIRCLE,          // Continuous circular path with constant radius
  
  MODE_COUNT            // Total number of modes (used for menu cycling)
};

/*
 * runMode() - Mode dispatcher function
 * Takes a Mode enum value and executes the corresponding behavior
 * 
 * @param m: The mode to execute (from Mode enum)
 */
void runMode(Mode m);

// Ballistic Behavior Function Declarations
// Each function implements one of the required SBA behaviors

/*
 * modeSquareSpiral() - Square Spiral Pattern
 * Robot drives in straight lines with 90° turns, gradually increasing
 * the length of each side to create an outward spiral pattern
 */
void modeSquareSpiral();

/*
 * modeZigzag() - Zig-Zag Pattern
 * Robot drives forward, then alternates between left and right turns
 * to create a zig-zag (back-and-forth diagonal) movement pattern
 */
void modeZigzag();

/*
 * modeSquareZigzag() - Square Zig-Zag (Lawn Mowing) Pattern
 * Robot drives in parallel straight lines, turning at the end of each line
 * to systematically cover a rectangular area (like mowing a lawn)
 */
void modeSquareZigzag();

/*
 * modeCircle() - Circular Path
 * Robot drives in a continuous circle by setting different speeds
 * for left and right motors. The speed ratio determines the radius.
 */
void modeCircle();
