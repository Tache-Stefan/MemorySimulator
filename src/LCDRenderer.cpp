#include "LCDRenderer.h"

LiquidCrystal LCDRenderer::screen(RS, EN, D4, D5, D6, D7);

void LCDRenderer::init() {
  screen.begin(SCREEN_WIDTH, SCREEN_HEIGHT);
  screen.noBlink();
  screen.clear();
}

void LCDRenderer::showMemoryState(const uint8_t virtualAddr, const uint8_t value, const AccessStatus status) {
  screen.clear();
  
  screen.setCursor(0, 0);
  screen.print("A:0x");
  screen.print(virtualAddr, HEX);
  screen.print(" V:0x");
  screen.print(value, HEX);
  
  screen.setCursor(0, 1);
  screen.print(statusToString(status));
}

void LCDRenderer::showPolicySelect(const char* currentPolicy) {
  screen.clear();
  
  screen.setCursor(0, 0);
  screen.print("Eviction Policy:");
  
  screen.setCursor(0, 1);
  screen.print("> ");
  screen.print(currentPolicy);
  screen.print(" <");
}

void LCDRenderer::showBenchmarkSelect(const uint8_t index) {
  screen.clear();
  
  screen.setCursor(0, 0);
  screen.print("Run Benchmark:");
  
  screen.setCursor(0, 1);
  screen.print(Benchmark::getPatternName(static_cast<BenchmarkPattern>(index)));
}

void LCDRenderer::showBenchmarkRunning(const char* patternName) {
  screen.clear();
  
  screen.setCursor(0, 0);
  screen.print("Running...");
  
  screen.setCursor(0, 1);
  screen.print(patternName);
}

void LCDRenderer::showBenchmarkResult(const BenchmarkResult& result) {
  screen.clear();
  
  if (result.pattern == BenchmarkPattern::COMPARATIVE) {
    screen.setCursor(0, 0);
    screen.print("Best:");
    switch (result.bestPolicy) {
      case EvictionPolicy::LRU: screen.print("LRU"); break;
      case EvictionPolicy::LFU: screen.print("LFU"); break;
      case EvictionPolicy::MRU: screen.print("MRU"); break;
    }
    screen.print(" ");
    screen.print(result.bestHitRate, 1);
    screen.print("%");
    
    screen.setCursor(0, 1);
    screen.print("L:");
    screen.print(static_cast<int>(result.policyHitRates[0]));
    screen.print(" F:");
    screen.print(static_cast<int>(result.policyHitRates[1]));
    screen.print(" M:");
    screen.print(static_cast<int>(result.policyHitRates[2]));
  } else {
    screen.setCursor(0, 0);
    screen.print("Hit:");
    screen.print(result.getHitRate(), 1);
    screen.print("% ");
    
    screen.print("L1:");
    screen.print(result.l1Hits);

    screen.setCursor(0, 1);
    screen.print("Cyc:");
    screen.print(result.getAvgCycles(), 1);
    screen.print(" ");

    const char* patternName = Benchmark::getPatternName(result.pattern);
    for (uint8_t i = 0; i < 4 && patternName[i] != '\0'; ++i) {
      screen.print(patternName[i]);
    }
  }
}

const char* LCDRenderer::statusToString(const AccessStatus status) {
  switch (status) {
    case AccessStatus::IDLE:
      return "Status: IDLE     ";
    case AccessStatus::HIT_L1:
      return "Status: HIT L1   ";
    case AccessStatus::HIT_L2:
      return "Status: HIT L2   ";
    case AccessStatus::MISS:
      return "Status: MISS     ";
    case AccessStatus::SEG_FAULT:
      return "Status: SEG FAULT";
    case AccessStatus::WRITE_BACK:
      return "Status: WRITE BK ";
    default:
      return "Status: UNKNOWN  ";
  }
}
