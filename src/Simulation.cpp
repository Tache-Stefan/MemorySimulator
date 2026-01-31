#include "Simulation.h"

CacheController Simulation::cache;
BenchmarkResult Simulation::lastBenchmarkResult;

void Simulation::init() {
  cache.init();
  MenuController::init();
  LEDController::init();
  LCDRenderer::init();
  MemoryHAL::init();
  ButtonManager::init();
  Benchmark::init(&cache);

  refreshDisplay();
  Serial.begin(9600);
}

void Simulation::update() {
  MenuAction action = MenuController::update();

  switch (action) {
    case MenuAction::NAVIGATE:
    case MenuAction::DISMISS_RESULT:
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
    case MenuAction::RUN_BENCHMARK:
      handleRunBenchmark();
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

void Simulation::handleRunBenchmark() {
  uint8_t index = MenuController::getSelectedBenchmark();
  BenchmarkPattern pattern = Benchmark::getPatternByIndex(index);

  MenuController::setBenchmarkRunning();
  LCDRenderer::showBenchmarkRunning(Benchmark::getPatternName(pattern));

  lastBenchmarkResult = Benchmark::run(pattern);

  Serial.println();
  Benchmark::printResultSerial(lastBenchmarkResult);

  MenuController::setBenchmarkComplete();
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
    case MenuScreen::BENCHMARK_SELECT:
      LCDRenderer::showBenchmarkSelect(MenuController::getSelectedBenchmark());
      break;
    case MenuScreen::BENCHMARK_RUNNING:
      break;
    case MenuScreen::BENCHMARK_RESULT:
      LCDRenderer::showBenchmarkResult(lastBenchmarkResult);
      break;
  }
}

void Simulation::refreshLEDs() {
  LEDController::showStatus(cache.getLastStatus());
  LEDController::showDirty(cache.hasDirtyL1());
}
