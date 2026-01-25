#include "LEDController.h"

void LEDController::init() {
  pinMode(LED_HIT, OUTPUT);
  pinMode(LED_MISS, OUTPUT);
  pinMode(LED_DIRTY, OUTPUT);
}

void LEDController::showStatus(const AccessStatus status) {
  clearAll();
  switch (status) {
    case AccessStatus::HIT_L1:
    case AccessStatus::HIT_L2:
      digitalWrite(LED_HIT, HIGH);
      break;
    case AccessStatus::MISS:
    case AccessStatus::SEG_FAULT:
      digitalWrite(LED_MISS, HIGH);
      break;
    default:
      break;
  }
}

void LEDController::showDirty(const bool isDirty) {
  digitalWrite(LED_DIRTY, isDirty);
}

void LEDController::clearAll() {
  digitalWrite(LED_HIT, LOW);
  digitalWrite(LED_MISS, LOW);
  digitalWrite(LED_DIRTY, LOW);
}
