#pragma once

#include <Arduino.h>
#include "ButtonManager.h"

enum class MenuAction {
  NONE,
  INCREMENT_ADDR,
  READ,
  WRITE
};

class MenuController {
public:
  static void init();
  static MenuAction update();

  static uint8_t getCurrentAddr();
  static uint8_t getCurrentValue();
  static void setCurrentValue(const uint8_t value);

private:
  static uint8_t currentAddr;
  static uint8_t currentValue;
};
