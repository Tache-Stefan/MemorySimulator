#pragma once

#include <Arduino.h>
#include "Constants.h"
#include "MemoryTypes.h"
#include "MemoryHAL.h"

enum class AccessStatus {
  IDLE,
  HIT_L1,
  HIT_L2,
  MISS,
  SEG_FAULT,
  WRITE_BACK
};

class CacheController {
public:
  void init();

  uint8_t readByte(const uint8_t virtualAddr);
  void writeByte(const uint8_t virtualAddr, const uint8_t data);

  AccessStatus getLastStatus() const;

private:
  L1Line l1[L1_SIZE];
  L2Line l2[L2_SIZE];
  PageTableEntry pageTable[VIRTUAL_SIZE];
  AccessStatus lastStatus = AccessStatus::IDLE;

  int16_t translateAddr(const uint8_t virtualAddr);

  int16_t findInL1(const uint8_t physicalAddr);
  int16_t findEmptyOrOldestL1();
  void flushL1Entry(const uint8_t index);
  void updateAgeL1(const uint8_t hitIndex);
  void insertL1Clean(const uint8_t physicalAddr, const uint8_t data);

  bool checkL2(const uint8_t physicalAddr, uint8_t& data);
  void updateL2(const uint8_t physicalAddr, const uint8_t data, bool markDirty = false);
  int16_t findInL2(const uint8_t physicalAddr);
  int16_t findEmptyOrOldestL2();
  void updateAgeL2(const uint8_t hitIndex);
  void flushL2Entry(const uint8_t index);
};
