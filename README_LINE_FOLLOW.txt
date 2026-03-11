═══════════════════════════════════════════════════════════════════════════
ZUMO ROBOT - LINE FOLLOWING WITH HEDWIG'S THEME
═══════════════════════════════════════════════════════════════════════════

Student: Bhanu
Date: March 4, 2026
Special Feature: Plays Harry Potter's Hedwig's Theme while following line!

═══════════════════════════════════════════════════════════════════════════
WHAT THIS PROGRAM DOES:
═══════════════════════════════════════════════════════════════════════════

This is a simplified version with ONLY line following mode.

FEATURES:
✅ Automatic sensor calibration
✅ Maximum speed line following (500)
✅ PID control for smooth tracking
✅ Plays Hedwig's Theme music continuously while moving! 🦉🎵

═══════════════════════════════════════════════════════════════════════════
HOW TO USE:
═══════════════════════════════════════════════════════════════════════════

1. Upload code to Zumo robot
2. Place robot on black line (white background)
3. Power on robot
4. Press Button B to start
5. Robot auto-calibrates (spins in place)
6. Robot follows line at MAX SPEED
7. Hedwig's Theme plays continuously! 🎶
8. Press Button B to stop

═══════════════════════════════════════════════════════════════════════════
HEDWIG'S THEME NOTES:
═══════════════════════════════════════════════════════════════════════════

The robot plays the CORRECT iconic Harry Potter Hedwig's Theme:

MELODY: E - B - D - C - B - A - G (hold) - E - D (hold) - C - B (hold)

MUSIC FORMAT:
!T120 = Tempo 120 (moderate speed)
L4 = Default note length (quarter notes)
O5 = Octave 5 (middle range)
8 = Eighth note (shorter)
4 = Quarter note (longer/held)

The music loops automatically when it finishes!
Plays continuously while robot follows the line.

═══════════════════════════════════════════════════════════════════════════
TECHNICAL DETAILS:
═══════════════════════════════════════════════════════════════════════════

SPEED: 500 (maximum)
PID CONTROL:
  - Kp = 0.2 (proportional gain)
  - Kd = 3.0 (derivative gain)

SENSORS: 5 line sensors
CALIBRATION: 80 spins

MUSIC: Hedwig's Theme from Harry Potter
  - 22 notes total
  - Plays continuously in loop
  - 300ms per note

═══════════════════════════════════════════════════════════════════════════
ADJUSTING SETTINGS:
═══════════════════════════════════════════════════════════════════════════

TO CHANGE SPEED:
File: Config.h
Line: #define LINE_SPEED 500
Change to: 400, 350, 300 (slower)

TO CHANGE MUSIC SPEED:
File: Modes.cpp
Line: if (millis() - lastNoteTime > 300)
Change 300 to: 200 (faster), 400 (slower)

TO CHANGE PID:
File: Config.h
Lines: 
  #define LINE_KP 0.2f
  #define LINE_KD 3.0f

═══════════════════════════════════════════════════════════════════════════
TROUBLESHOOTING:
═══════════════════════════════════════════════════════════════════════════

ROBOT WOBBLES:
- Increase LINE_KD or decrease LINE_KP

ROBOT LOSES LINE:
- Increase LINE_KP
- Decrease speed

NO MUSIC:
- Check buzzer connection
- Verify buzzer is not muted

ROBOT TOO FAST:
- Decrease LINE_SPEED in Config.h

═══════════════════════════════════════════════════════════════════════════
END OF README
═══════════════════════════════════════════════════════════════════════════
