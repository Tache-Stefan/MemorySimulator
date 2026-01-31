#include "MenuController.h"
#include "Constants.h"

MenuScreen MenuController::currentScreen = MenuScreen::MEMORY_ACCESS;
uint8_t MenuController::currentAddr = 0;
uint8_t MenuController::currentValue = 0;
uint8_t MenuController::selectedBenchmark = 0;

void MenuController::init() {
  currentScreen = MenuScreen::MEMORY_ACCESS;
  currentAddr = 0;
  currentValue = 0;
  selectedBenchmark = 0;
}

MenuAction MenuController::update() {
  ButtonEvent event = ButtonManager::poll();
  if (event == ButtonEvent::NONE) {
    return MenuAction::NONE;
  }

  switch (currentScreen) {
    case MenuScreen::MEMORY_ACCESS:
      return handleMemoryAccess(event);
    case MenuScreen::POLICY_SELECT:
      return handlePolicySelect(event);
    case MenuScreen::BENCHMARK_SELECT:
      return handleBenchmarkSelect(event);
    case MenuScreen::BENCHMARK_RUNNING:
      return MenuAction::NONE;
    case MenuScreen::BENCHMARK_RESULT:
      return handleBenchmarkResult(event);
    default:
      return MenuAction::NONE;
  }
}

MenuAction MenuController::handleMemoryAccess(const ButtonEvent event) {
  switch (event) {
    case ButtonEvent::NEXT:
      if (currentAddr >= VIRTUAL_SIZE - 1) {
        currentScreen = MenuScreen::POLICY_SELECT;
      } else {
        currentAddr++;
      }
      return MenuAction::NAVIGATE;
    case ButtonEvent::READ:
      return MenuAction::READ;
    case ButtonEvent::WRITE:
      return MenuAction::WRITE;
    default:
      return MenuAction::NONE;
  }
}

MenuAction MenuController::handlePolicySelect(const ButtonEvent event) {
  switch (event) {
    case ButtonEvent::NEXT:
      currentScreen = MenuScreen::BENCHMARK_SELECT;
      return MenuAction::NAVIGATE;
    case ButtonEvent::READ:
      return MenuAction::POLICY_CHANGE;
    default:
      return MenuAction::NONE;
  }
}

MenuAction MenuController::handleBenchmarkSelect(const ButtonEvent event) {
  switch (event) {
    case ButtonEvent::NEXT:
      currentScreen = MenuScreen::MEMORY_ACCESS;
      currentAddr = 0;
      return MenuAction::NAVIGATE;
    case ButtonEvent::READ:
      selectedBenchmark = (selectedBenchmark + 1) % BENCHMARK_PATTERN_COUNT;
      return MenuAction::NAVIGATE;
    case ButtonEvent::WRITE:
      return MenuAction::RUN_BENCHMARK;
    default:
      return MenuAction::NONE;
  }
}

MenuAction MenuController::handleBenchmarkResult(const ButtonEvent event) {
  switch (event) {
    case ButtonEvent::NEXT:
    case ButtonEvent::READ:
    case ButtonEvent::WRITE:
      currentScreen = MenuScreen::BENCHMARK_SELECT;
      return MenuAction::DISMISS_RESULT;
    default:
      return MenuAction::NONE;
  }
}

MenuScreen MenuController::getCurrentScreen() {
  return currentScreen;
}

uint8_t MenuController::getCurrentAddr() {
  return currentAddr;
}

uint8_t MenuController::getCurrentValue() {
  return currentValue;
}

uint8_t MenuController::getSelectedBenchmark() {
  return selectedBenchmark;
}

void MenuController::setCurrentValue(const uint8_t value) {
  currentValue = value;
}

void MenuController::setBenchmarkRunning() {
  currentScreen = MenuScreen::BENCHMARK_RUNNING;
}

void MenuController::setBenchmarkComplete() {
  currentScreen = MenuScreen::BENCHMARK_RESULT;
}
