#include "Hardware.h"
#include "Menu.h"
#include "Modes.h"
#include "Config.h"

void setup() {
  Serial.begin(9600);
  initializeHardware();
  playStartupSound();
  delay(500);
}

void loop() {
  display.clear();
  display.print("ZumoOS Pro");
  display.gotoXY(0, 1);
  display.print("Advanced LF");
  display.gotoXY(0, 3);
  display.print("Press B");

  while (!buttonB.getSingleDebouncedPress()) {
    delay(50);
  }

  modeLineFollow();
}
