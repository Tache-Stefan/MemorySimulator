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
