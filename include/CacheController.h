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

struct CacheStats {
  uint32_t l1Hits;
  uint32_t l2Hits;
  uint32_t misses;
  uint32_t writeBacks;
  uint32_t segFaults;
  uint32_t totalCycles;

  void reset() {
    l1Hits = 0;
    l2Hits = 0;
    misses = 0;
    writeBacks = 0;
    segFaults = 0;
    totalCycles = 0;
  }
};

class CacheController {
public:
  void init();
  void reset();

  uint8_t readByte(const uint8_t virtualAddr, const bool simulateDelay = true);
  void writeByte(const uint8_t virtualAddr, const uint8_t data, const bool simulateDelay = true);

  bool hasDirtyL1() const;
  AccessStatus getLastStatus() const;

  const CacheStats& getStats() const;

  void setEvictionPolicy(const EvictionPolicy policy);
  EvictionPolicy getEvictionPolicy() const;
  const char* getPolicyName() const;

private:
  L1Line l1[L1_SIZE];
  L2Line l2[L2_SIZE];
  PageTableEntry pageTable[VIRTUAL_SIZE];
  CacheStats stats;
  AccessStatus lastStatus = AccessStatus::IDLE;
  EvictionPolicy currentPolicy = EvictionPolicy::LRU;

  int16_t translateAddr(const uint8_t virtualAddr);

  int16_t findInL1(const uint8_t physicalAddr);
  int16_t findEvictionCandidateL1();
  void updateAccessL1(const uint8_t hitIndex);
  void insertL1Clean(const uint8_t physicalAddr, const uint8_t data, const bool simulateDelay = true);

  bool checkL2(const uint8_t physicalAddr, uint8_t& data);
  void updateL2(const uint8_t physicalAddr, const uint8_t data, bool markDirty = false, const bool simulateDelay = true);
  int16_t findInL2(const uint8_t physicalAddr);
  int16_t findEvictionCandidateL2();
  void updateAccessL2(const uint8_t hitIndex);
  void flushL2Entry(const uint8_t index, const bool simulateDelay = true);
};
