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

// Costs (Simulated Cycles)
constexpr uint8_t COST_L1_HIT = 1;
constexpr uint8_t COST_L2_HIT = 10;
constexpr uint8_t COST_MAIN_HIT = 100;
constexpr uint8_t COST_WRITEBACK = 10;
constexpr uint8_t COST_SEGFAULT = 50;

constexpr uint8_t BENCHMARK_PATTERN_COUNT = 6;
constexpr uint8_t BENCHMARK_ACCESS_COUNT = 125;
