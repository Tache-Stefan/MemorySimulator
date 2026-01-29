#pragma once

#include <Arduino.h>
#include "CacheController.h"
#include "MenuController.h"
#include "ButtonManager.h"
#include "LEDController.h"
#include "LCDRenderer.h"
#include "MemoryHAL.h"

class Simulation {
public:
  static void init();
  static void update();

private:
  static CacheController cache;
  
  static void handleRead();
  static void handleWrite();
  static void refreshDisplay();
  static void refreshLEDs();
};
