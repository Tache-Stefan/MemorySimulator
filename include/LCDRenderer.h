#pragma once

#include <Arduino.h>
#include <LiquidCrystal.h>
#include "CacheController.h"

class LCDRenderer {
public:
  static void init();
  static void showMemoryState(const uint8_t virtualAddr, const uint8_t value, const AccessStatus status);

private:
  static LiquidCrystal screen;

  static constexpr uint8_t RS = 2;
  static constexpr uint8_t EN = 3;
  static constexpr uint8_t D4 = 4;
  static constexpr uint8_t D5 = 5;
  static constexpr uint8_t D6 = 6;
  static constexpr uint8_t D7 = 7;
  static constexpr uint8_t SCREEN_WIDTH = 16;
  static constexpr uint8_t SCREEN_HEIGHT = 2;

  static const char* statusToString(const AccessStatus status);
};
