#include "Simulation.h"

CacheController Simulation::cache;

void Simulation::init() {
  cache.init();
  MenuController::init();
  LEDController::init();
  LCDRenderer::init();
  MemoryHAL::init();
  ButtonManager::init();

  refreshDisplay();
  Serial.begin(9600);
}

void Simulation::update() {
  MenuAction action = MenuController::update();

  switch (action) {
    case MenuAction::INCREMENT_ADDR:
      refreshDisplay();
      break;
    case MenuAction::READ:
      handleRead();
      break;
    case MenuAction::WRITE:
      handleWrite();
      break;
    default:
      break;
  }
}

void Simulation::handleRead() {
  uint8_t addr = MenuController::getCurrentAddr();
  uint8_t value = cache.readByte(addr);
  MenuController::setCurrentValue(value);

  refreshLEDs();
  refreshDisplay();
}

void Simulation::handleWrite() {
  uint8_t addr = MenuController::getCurrentAddr();
  uint8_t value = MenuController::getCurrentValue();
  cache.writeByte(addr, value + 1);

  refreshLEDs();
  refreshDisplay();
}

void Simulation::refreshDisplay() {
  uint8_t addr = MenuController::getCurrentAddr();
  uint8_t value = MenuController::getCurrentValue();
  AccessStatus status = cache.getLastStatus();

  LCDRenderer::showMemoryState(addr, value, status);
}

void Simulation::refreshLEDs() {
  LEDController::showStatus(cache.getLastStatus());
  LEDController::showDirty(cache.hasDirtyL1());
}
