#pragma once

#include <Arduino.h>

struct L1Line {
  uint8_t physicalAddr;
  uint8_t data;
  bool valid;
  bool dirty;
  uint8_t age;
};

struct L2Line {
  uint8_t physicalAddr;
  bool valid;
  bool dirty;
  uint8_t age;
};

struct PageTableEntry {
  uint8_t physicalAddr;
  bool valid;
};
