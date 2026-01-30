#pragma once

#include <Arduino.h>

constexpr uint8_t L1_SIZE = 4;
constexpr uint8_t L2_SIZE = 8;
constexpr uint8_t EEPROM_ADDR = 0x50;
constexpr uint8_t VIRTUAL_SIZE = 20;

constexpr uint8_t POLICY_COUNT = 3;
enum class EvictionPolicy {
  LRU,
  LFU,
  MRU
};
