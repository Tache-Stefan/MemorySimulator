#include "MenuController.h"
#include "Constants.h"

uint8_t MenuController::currentAddr = 0;
uint8_t MenuController::currentValue = 0;

void MenuController::init() {
  currentAddr = 0;
  currentValue = 0;
}

MenuAction MenuController::update() {
  ButtonEvent event = ButtonManager::poll();

  switch (event) {
    case ButtonEvent::NEXT:
      currentAddr = (currentAddr + 1) % VIRTUAL_SIZE;
      return MenuAction::INCREMENT_ADDR;
    case ButtonEvent::READ:
      return MenuAction::READ;
    case ButtonEvent::WRITE:
      return MenuAction::WRITE;
    default:
      return MenuAction::NONE;
  }
}

uint8_t MenuController::getCurrentAddr(){
  return currentAddr;
}

uint8_t MenuController::getCurrentValue(){
  return currentValue;
}

void MenuController::setCurrentValue(const uint8_t value) {
  currentValue = value;
}
