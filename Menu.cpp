#include "Menu.h"
#include "Hardware.h"
#include "Config.h"

static const char* modeNames[] = {
  "Line Follow"
};

const char* getModeName(Mode mode) {
  if (mode >= 0 && mode < MODE_COUNT) {
    return modeNames[mode];
  }
  return "Unknown";
}

void showMenu() {
  display.clear();
  display.print("A/C:Chg B:Run");
}

Mode selectMode() {
  static Mode currentMode = MODE_LINE_FOLLOW;
  showMenu();
  display.gotoXY(0, 1);
  display.print(getModeName(currentMode));
  delay(100);
  Mode lastDisplayedMode = currentMode;
  
  while (true) {
    if (currentMode != lastDisplayedMode) {
      display.gotoXY(0, 1);
      display.print("                ");
      display.gotoXY(0, 1);
      display.print(getModeName(currentMode));
      lastDisplayedMode = currentMode;
    }
    
    if (buttonA.getSingleDebouncedPress()) {
      currentMode = (Mode)((currentMode + 1) % MODE_COUNT);
      playModeChangeSound();
    }
    
    if (buttonC.getSingleDebouncedPress()) {
      currentMode = (Mode)((currentMode - 1 + MODE_COUNT) % MODE_COUNT);
      playModeChangeSound();
    }
    
    if (buttonB.getSingleDebouncedPress()) {
      displayModeInfo(currentMode);
      return currentMode;
    }
    
    delay(50);
  }
}

void displayModeInfo(Mode mode) {
  display.clear();
  display.print("Starting:");
  display.gotoXY(0, 1);
  display.print(getModeName(mode));
  display.gotoXY(0, 2);
  display.print("Press B=Stop");
  delay(1000);
}
