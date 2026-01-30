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
    case MenuAction::NAVIGATE:
      refreshDisplay();
      break;
    case MenuAction::READ:
      handleRead();
      break;
    case MenuAction::WRITE:
      handleWrite();
      break;
    case MenuAction::POLICY_CHANGE:
      handlePolicyChange();
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
  uint8_t value = cache.readByte(addr);
  cache.writeByte(addr, value + 1);

  refreshLEDs();
  refreshDisplay();
}

void Simulation::handlePolicyChange() {
  EvictionPolicy currentPolicy = cache.getEvictionPolicy();
  EvictionPolicy newPolicy;

  switch (currentPolicy) {
    case EvictionPolicy::LRU:
      newPolicy = EvictionPolicy::LFU;
      break;
    case EvictionPolicy::LFU:
      newPolicy = EvictionPolicy::MRU;
      break;
    case EvictionPolicy::MRU:
      newPolicy = EvictionPolicy::LRU;
      break;
    default:
      newPolicy = EvictionPolicy::LRU;
      break;
  }

  cache.reset();
  cache.setEvictionPolicy(newPolicy);
  refreshDisplay();
}

void Simulation::refreshDisplay() {
  MenuScreen screen = MenuController::getCurrentScreen();

  switch (screen) {
    case MenuScreen::MEMORY_ACCESS:
      LCDRenderer::showMemoryState(
        MenuController::getCurrentAddr(),
        MenuController::getCurrentValue(),
        cache.getLastStatus()
      );
      break;
    case MenuScreen::POLICY_SELECT:
      LCDRenderer::showPolicySelect(cache.getPolicyName());
      break;
  }
}

void Simulation::refreshLEDs() {
  LEDController::showStatus(cache.getLastStatus());
  LEDController::showDirty(cache.hasDirtyL1());
}
