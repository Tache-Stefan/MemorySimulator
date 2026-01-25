#pragma once

#include <Arduino.h>

enum class ButtonEvent {
  NONE,
  NEXT,
  READ,
  WRITE
};

class ButtonManager {
public:
  static void init();
  static ButtonEvent poll();

private:
  static constexpr uint8_t BTN_NEXT = A0;
  static constexpr uint8_t BTN_READ = A1;
  static constexpr uint8_t BTN_WRITE = A2;
  static constexpr uint8_t DEBOUNCE_DELAY_MS = 150;

  static uint32_t lastDebounceNext;
  static uint32_t lastDebounceRead;
  static uint32_t lastDebounceWrite;

  static bool debounce(const uint8_t pin);
  static uint32_t getLastDebounceTime(const uint8_t pin);
  static void setLastDebounceTime(const uint8_t pin, const uint32_t time);
};
