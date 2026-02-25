#include "Maze.h"
#include "Hardware.h"
#include "Config.h"
#include "Utils.h"

// ============ ENCODER TURN HELPERS ============
static void turnInPlaceTicks(int leftSpeed, int rightSpeed, int ticks) {
  encoders.getCountsAndResetLeft();
  encoders.getCountsAndResetRight();
  
  while (abs(encoders.getCountsLeft()) < ticks || abs(encoders.getCountsRight()) < ticks) {
    motors.setSpeeds(leftSpeed, rightSpeed);
  }
  motors.setSpeeds(0,0);
  delay(150);
}

// ============ WALL-MAZE: RIGHT HAND RULE ============
void mazeRightHandLoop() {
  prox.initThreeSensors();
  
  while (!buttonB.getSingleDebouncedPress()) {
    prox.read();
    int left  = prox.countsLeftWithLeftLeds();
    int front = prox.countsFrontWithLeftLeds();
    int right = prox.countsRightWithRightLeds();
    
    bool wallRight = right > MAZE_WALL_THRESHOLD;
    bool wallFront = front > MAZE_WALL_THRESHOLD;
    
    // RIGHT HAND RULE:
    // if right is open -> turn right
    // else if front is open -> forward
    // else -> turn left
    if (!wallRight) {
      // turn right 90
      turnInPlaceTicks(MAZE_TURN_SPEED, -MAZE_TURN_SPEED, TURN_90_TICKS);
    }
    else if (!wallFront) {
      // go forward with slight right-wall correction
      int error = right - WALL_DISTANCE_TARGET;  // desired right distance
      int correction = error * 40;
      motors.setSpeeds(clampSpeed(MAZE_FORWARD_SPEED - correction), 
                      clampSpeed(MAZE_FORWARD_SPEED + correction));
    }
    else {
      // turn left 90
      turnInPlaceTicks(-MAZE_TURN_SPEED, MAZE_TURN_SPEED, TURN_90_TICKS);
    }
  }
  
  motors.setSpeeds(0,0);
}

// ============ LINE MAZE: JUNCTION DECISION ============
static bool isBlack(unsigned int v) {
  return v < LINE_BLACK_THRESHOLD;
}

void lineMazeLoop() {
  lineSensors.initFiveSensors();
  
  // quick calibration spin
  motors.setSpeeds(150, -150);
  for (int i=0; i<CALIBRATION_SPINS; i++) { 
    lineSensors.calibrate(); 
    delay(CALIBRATION_DELAY_MS); 
  }
  motors.setSpeeds(0,0);
  delay(200);
  
  // simple policy: prefer Right, then Straight, then Left (right-hand rule on line)
  while (!buttonB.getSingleDebouncedPress()) {
    lineSensors.read(lineValues);
    
    bool leftOn  = isBlack(lineValues[0]) || isBlack(lineValues[1]);
    bool midOn   = isBlack(lineValues[2]);
    bool rightOn = isBlack(lineValues[3]) || isBlack(lineValues[4]);
    
    // Detect junction: many sensors see black (or both sides)
    bool junction = (leftOn && rightOn) || (leftOn && midOn) || (rightOn && midOn);
    
    if (junction) {
      // stop, decide turn
      motors.setSpeeds(0,0);
      delay(80);
      
      // Prefer RIGHT if available
      if (rightOn) {
        turnInPlaceTicks(MAZE_TURN_SPEED, -MAZE_TURN_SPEED, TURN_90_TICKS);
      } else if (midOn) {
        motors.setSpeeds(MAZE_FORWARD_SPEED, MAZE_FORWARD_SPEED);
        delay(200);
      } else if (leftOn) {
        turnInPlaceTicks(-MAZE_TURN_SPEED, MAZE_TURN_SPEED, TURN_90_TICKS);
      } else {
        // lost: spin search
        motors.setSpeeds(200, -200);
        delay(200);
      }
    } else {
      // regular line follow (simple P)
      int pos = lineSensors.readLine(lineValues);
      int error = pos - 2000;
      int correction = (int)(LINE_KP * error);
      
      motors.setSpeeds(clampSpeed(MAZE_FORWARD_SPEED + correction), 
                      clampSpeed(MAZE_FORWARD_SPEED - correction));
    }
  }
  
  motors.setSpeeds(0,0);
}