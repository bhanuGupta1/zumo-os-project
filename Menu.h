#pragma once
#include "Modes.h"

void showMenu();
Mode selectMode();
void displayModeInfo(Mode mode);
const char* getModeName(Mode mode);