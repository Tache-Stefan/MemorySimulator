#pragma once

#include <Arduino.h>
#include "CacheController.h"

class LEDController {
public:
  static void init();
  static void showStatus(const AccessStatus status);
  static void showDirty(const bool isDirty);
  static void clearAll();

private:
  static constexpr uint8_t LED_HIT = 8;
  static constexpr uint8_t LED_MISS = 9;
  static constexpr uint8_t LED_DIRTY = 12;
};
