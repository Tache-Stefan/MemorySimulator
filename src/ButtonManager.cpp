#include "ButtonManager.h"

uint32_t ButtonManager::lastDebounceNext = 0;
uint32_t ButtonManager::lastDebounceRead = 0;
uint32_t ButtonManager::lastDebounceWrite = 0;

void ButtonManager::init() {
  pinMode(BTN_NEXT, INPUT_PULLUP);
  pinMode(BTN_READ, INPUT_PULLUP);
  pinMode(BTN_WRITE, INPUT_PULLUP);
}

ButtonEvent ButtonManager::poll() {
  if (debounce(BTN_NEXT)) {
    return ButtonEvent::NEXT;
  }
  if (debounce(BTN_READ)) {
    return ButtonEvent::READ;
  }
  if (debounce(BTN_WRITE)) {
    return ButtonEvent::WRITE;
  }
  return ButtonEvent::NONE;
}

bool ButtonManager::debounce(const uint8_t pin) {
  if (digitalRead(pin) == LOW) {
    uint32_t now = millis();
    if (now - getLastDebounceTime(pin) > DEBOUNCE_DELAY_MS) {
      setLastDebounceTime(pin, now);
      return true;
    }
  }
  return false;
}

uint32_t ButtonManager::getLastDebounceTime(const uint8_t pin) {
  switch (pin) {
    case BTN_NEXT:
      return lastDebounceNext;
    case BTN_READ:
      return lastDebounceRead;
    case BTN_WRITE:
      return lastDebounceWrite;
    default:
      return 0;
  }
}

void ButtonManager::setLastDebounceTime(const uint8_t pin, const uint32_t time) {
  switch (pin) {
    case BTN_NEXT:
      lastDebounceNext = time;
      break;
    case BTN_READ:
      lastDebounceRead = time;
      break;
    case BTN_WRITE:
      lastDebounceWrite = time;
      break;
  }
}
