/*
 * Modes.h - Ballistic Behavior Mode Definitions
 * 
 * ========== WHAT THIS FILE DOES ==========
 * This header file defines all available robot operating modes and
 * declares the functions that implement each ballistic behavior.
 * 
 * ========== BALLISTIC BEHAVIORS EXPLAINED ==========
 * Ballistic behaviors are pre-programmed movement patterns that run
 * without sensor feedback (open-loop control). The robot follows a
 * predetermined path using encoder-based distance and timing control.
 * 
 * The 4 required behaviors are:
 * 1. SPIRAL - Circular path with decreasing radius
 * 2. ZIG-ZAG - Alternating diagonal movements
 * 3. LAWN MOWER - Parallel rows covering rectangular area
 * 4. CIRCLE - Single continuous circular path
 * 
 * ========== HOW TO ADD A NEW MODE ==========
 * 1. Add new enum value to Mode enum (before MODE_COUNT)
 * 2. Declare function: void modeNewBehavior();
 * 3. Implement function in Modes.cpp
 * 4. Add case to runMode() switch statement
 * 5. Add mode name to getModeName() in Menu.cpp
 * 
 * ========== KEY CONCEPTS ==========
 * - Enum: A list of named constants (MODE_SQUARE_SPIRAL = 0, etc.)
 * - Function declaration: Tells compiler function exists (defined elsewhere)
 * - Header guard (#pragma once): Prevents multiple inclusion errors
 */

#pragma once

/*
 * Mode Enumeration
 * 
 * Defines all available ballistic behaviors for the robot.
 * Each mode represents a different movement pattern.
 * 
 * HOW ENUMS WORK:
 * - Each name gets an integer value (starting from 0)
 * - MODE_SQUARE_SPIRAL = 0
 * - MODE_ZIGZAG = 1
 * - MODE_SQUARE_ZIGZAG = 2
 * - MODE_CIRCLE = 3
 * - MODE_COUNT = 4 (total number of modes)
 * 
 * MODE_COUNT is used for menu cycling (wrapping around)
 */
enum Mode {
  // BALLISTIC BEHAVIORS (SBA Assignment)
  MODE_SPIRAL,          // Circular spiral with selectable direction
                        // User chooses inward (to center) or outward (from center)
  
  MODE_SQUARE_SPIRAL,   // Square spiral with 90° turns and selectable direction
                        // User chooses inward or outward
                        
  MODE_ZIGZAG,          // Alternating diagonal movements
                        // Creates "Z" or lightning bolt pattern
                        
  MODE_SQUARE_ZIGZAG,   // Parallel lines covering rectangular area
                        // Like mowing a lawn or printer head movement
                        
  MODE_CIRCLE,          // Continuous circular path with constant radius
                        // Endpoint meets startpoint (closed circle)
  
  MODE_COUNT            // Total number of modes (used for menu cycling)
                        // This is always last and equals the count
};

/*
 * runMode() - Mode dispatcher function
 * 
 * Takes a Mode enum value and executes the corresponding behavior.
 * Uses a switch statement to call the appropriate function.
 * 
 * @param m: The mode to execute (from Mode enum)
 * 
 * Example usage:
 *   Mode selectedMode = MODE_CIRCLE;
 *   runMode(selectedMode);  // Executes modeCircle()
 */
void runMode(Mode m);

// ============ BALLISTIC BEHAVIOR FUNCTION DECLARATIONS ============
// Each function implements one of the required SBA behaviors
// Actual implementation is in Modes.cpp

/*
 * modeSpiral() - Circular Spiral Pattern
 * 
 * NOTE: NOT part of assignment - included for comparison with Square Spiral
 * 
 * WHAT IT DOES:
 * Robot drives in a smooth circular spiral with user-selectable direction.
 * Can spiral inward (to center) or outward (from center).
 * 
 * HOW IT WORKS:
 * - User selects direction with Button A (inward) or C (outward)
 * - Uses differential motor speeds to create circular motion
 * - Gradually changes speed difference to change radius
 * - Creates smooth spiral (like water down a drain or spring unwinding)
 * 
 * ADJUSTABLE PARAMETERS (in Modes.cpp):
 * - outerSpeed: Overall movement speed
 * - startInnerSpeed/endInnerSpeed: Initial and final inner wheel speeds
 * - speedChangeRate: How fast radius changes
 */
void modeSpiral();

/*
 * modeSquareSpiral() - Square Spiral Pattern with 90° Turns
 * 
 * WHAT IT DOES:
 * Robot drives in a square spiral with user-selectable direction.
 * Can spiral inward (to center) or outward (from center).
 * Uses straight lines and 90° turns (not smooth curves).
 * 
 * HOW IT WORKS:
 * - User selects direction with Button A (inward) or C (outward)
 * - Drive straight for current side length
 * - Turn 90 degrees
 * - Increase or decrease side length
 * - Repeat to create square spiral
 * 
 * ADJUSTABLE PARAMETERS (in Modes.cpp):
 * - sideLength: Starting side length
 * - increment: How much to change each side
 * - speed: Movement speed
 */
void modeSquareSpiral();

/*
 * modeZigzag() - Zig-Zag Pattern
 * 
 * WHAT IT DOES:
 * Robot drives forward in straight segments, alternating between
 * right and left turns to create a "Z" or lightning bolt pattern.
 * 
 * HOW IT WORKS:
 * - Drive straight for fixed distance
 * - Turn right by angle X
 * - Drive straight again
 * - Turn left by angle 2X (to reverse direction)
 * - Repeat to create zigzag
 * 
 * ADJUSTABLE PARAMETERS (in Modes.cpp):
 * - segmentLength: Length of each diagonal line
 * - zigzags: Number of zig-zag segments
 * - turnAngleTicks: Sharpness of turns
 */
void modeZigzag();

/*
 * modeSquareZigzag() - Square Zig-Zag (Lawn Mowing) Pattern
 * 
 * WHAT IT DOES:
 * Robot drives in parallel straight lines, turning at the end of each
 * line to systematically cover a rectangular area. Like mowing a lawn
 * or how a printer head moves.
 * 
 * HOW IT WORKS:
 * - Drive straight along a row
 * - Turn 90° perpendicular to row
 * - Move forward by row spacing
 * - Turn 90° again to face along next row (opposite direction)
 * - Repeat until all rows complete
 * 
 * ADJUSTABLE PARAMETERS (in Modes.cpp):
 * - rowLength: Width of area to cover
 * - rows: Number of parallel rows
 * - rowSpacing: Distance between rows
 */
void modeSquareZigzag();

/*
 * modeCircle() - Circular Path
 * 
 * WHAT IT DOES:
 * Robot drives in a continuous circle with constant radius.
 * The endpoint should meet the startpoint (closed circle).
 * 
 * HOW IT WORKS:
 * - Set different speeds for left and right motors
 * - Speed difference determines circle radius
 * - Run for calculated duration to complete one full circle
 * - Outer wheel travels faster than inner wheel
 * 
 * ADJUSTABLE PARAMETERS (in Modes.cpp):
 * - outerSpeed: Speed of outer wheel
 * - innerSpeed: Speed of inner wheel
 * - circleDuration: How long to drive (controls circle completion)
 */
void modeCircle();
