#pragma once

#include <Arduino.h>
#include "ButtonManager.h"
#include "Constants.h"

enum class MenuScreen {
  MEMORY_ACCESS,
  POLICY_SELECT
};

enum class MenuAction {
  NONE,
  NAVIGATE,
  READ,
  WRITE,
  POLICY_CHANGE
};

class MenuController {
public:
  static void init();
  static MenuAction update();

  static MenuScreen getCurrentScreen();
  static uint8_t getCurrentAddr();
  static uint8_t getCurrentValue();
  static void setCurrentValue(const uint8_t value);

private:
  static MenuScreen currentScreen;
  static uint8_t currentAddr;
  static uint8_t currentValue;

  static MenuAction handleMemoryAccess(const ButtonEvent event);
  static MenuAction handlePolicySelect(const ButtonEvent event);
};
