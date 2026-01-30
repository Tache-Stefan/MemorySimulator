#include "MenuController.h"
#include "Constants.h"

MenuScreen MenuController::currentScreen = MenuScreen::MEMORY_ACCESS;
uint8_t MenuController::currentAddr = 0;
uint8_t MenuController::currentValue = 0;

void MenuController::init() {
  currentScreen = MenuScreen::MEMORY_ACCESS;
  currentAddr = 0;
  currentValue = 0;
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
      currentScreen = MenuScreen::MEMORY_ACCESS;
      currentAddr = 0;
      return MenuAction::NAVIGATE;
    case ButtonEvent::READ:
    case ButtonEvent::WRITE:
      return MenuAction::POLICY_CHANGE;
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

void MenuController::setCurrentValue(const uint8_t value) {
  currentValue = value;
}
