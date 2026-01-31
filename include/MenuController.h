#pragma once

#include <Arduino.h>
#include "ButtonManager.h"
#include "Constants.h"

enum class MenuScreen {
  MEMORY_ACCESS,
  POLICY_SELECT,
  BENCHMARK_SELECT,
  BENCHMARK_RUNNING,
  BENCHMARK_RESULT
};

enum class MenuAction {
  NONE,
  NAVIGATE,
  READ,
  WRITE,
  POLICY_CHANGE,
  RUN_BENCHMARK,
  DISMISS_RESULT
};

class MenuController {
public:
  static void init();
  static MenuAction update();

  static MenuScreen getCurrentScreen();
  static uint8_t getCurrentAddr();
  static uint8_t getCurrentValue();
  static uint8_t getSelectedBenchmark();
  static void setCurrentValue(const uint8_t value);
  static void setBenchmarkRunning();
  static void setBenchmarkComplete();

private:
  static MenuScreen currentScreen;
  static uint8_t currentAddr;
  static uint8_t currentValue;
  static uint8_t selectedBenchmark;

  static MenuAction handleMemoryAccess(const ButtonEvent event);
  static MenuAction handlePolicySelect(const ButtonEvent event);
  static MenuAction handleBenchmarkSelect(const ButtonEvent event);
  static MenuAction handleBenchmarkResult(const ButtonEvent event);
};
