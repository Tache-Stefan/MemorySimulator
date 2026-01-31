#include "CacheController.h"

void CacheController::init() {
  randomSeed(analogRead(A3));
  for (uint8_t i = 0; i < VIRTUAL_SIZE; ++i) {
    pageTable[i].valid = true;
    pageTable[i].physicalAddr = 10 + i * 5 + random(0, 5);

    // Simulate SegFault
    if (i == 14) {
      pageTable[i].valid = false;
    }
  }

  for (int i = 0; i < L1_SIZE; ++i) {
    l1[i].valid = false;
    l1[i].dirty = false;
    l1[i].age = 0;
    l1[i].frequency = 0;
  }

  for (int i = 0; i < L2_SIZE; ++i) {
    l2[i].valid = false;
    l2[i].dirty = false;
    l2[i].age = 0;
    l2[i].frequency = 0;
  }

  lastStatus = AccessStatus::IDLE;
  currentPolicy = EvictionPolicy::LRU;
  stats.reset();
}

void CacheController::reset() {
  for (uint8_t i = 0; i < L1_SIZE; ++i) {
    if (l1[i].valid && l1[i].dirty) {
      updateL2(l1[i].physicalAddr, l1[i].data, true, false);
      l1[i].dirty = false;
    }
  }

  for (uint8_t i = 0; i < L2_SIZE; ++i) {
    if (l2[i].valid && l2[i].dirty) {
      flushL2Entry(i, false);
    }
  }

  for (uint8_t i = 0; i < L1_SIZE; ++i) {
    l1[i].valid = false;
    l1[i].dirty = false;
    l1[i].age = 0;
    l1[i].frequency = 0;
  }

  for (uint8_t i = 0; i < L2_SIZE; ++i) {
    l2[i].valid = false;
    l2[i].dirty = false;
    l2[i].age = 0;
    l2[i].frequency = 0;
  }

  lastStatus = AccessStatus::IDLE;
  stats.reset();
}

void CacheController::setEvictionPolicy(const EvictionPolicy policy) {
  currentPolicy = policy;
}

EvictionPolicy CacheController::getEvictionPolicy() const {
  return currentPolicy;
}

const CacheStats& CacheController::getStats() const {
  return stats;
}

const char* CacheController::getPolicyName() const {
  switch (currentPolicy) {
    case EvictionPolicy::LRU:
      return "LRU";
    case EvictionPolicy::LFU:
      return "LFU";
    case EvictionPolicy::MRU:
      return "MRU";
    default:
      return "???";
  }
}

uint8_t CacheController::readByte(const uint8_t virtualAddr, const bool simulateDelay) {
  int16_t physicalAddr = translateAddr(virtualAddr);
  if (physicalAddr == -1) {
    lastStatus = AccessStatus::SEG_FAULT;
    stats.segFaults++;
    stats.totalCycles += COST_SEGFAULT;
    return 0xFF;
  }
  physicalAddr = static_cast<uint8_t>(physicalAddr);

  stats.totalCycles += COST_L1_HIT;

  int16_t l1Index = findInL1(physicalAddr);
  if (l1Index != -1) {
    updateAccessL1(l1Index);
    lastStatus = AccessStatus::HIT_L1;
    stats.l1Hits++;
    return l1[l1Index].data;
  }

  stats.totalCycles += COST_L2_HIT;

  uint8_t data;
  if (checkL2(physicalAddr, data)) {
    lastStatus = AccessStatus::HIT_L2;
    stats.l2Hits++;
    insertL1Clean(physicalAddr, data, simulateDelay);
    return data;
  }

  stats.totalCycles += COST_MAIN_HIT;
  stats.misses++;

  data = MemoryHAL::readMain(physicalAddr, simulateDelay);
  insertL1Clean(physicalAddr, data, simulateDelay);
  
  lastStatus = AccessStatus::MISS;
  return data;
}

void CacheController::writeByte(const uint8_t virtualAddr, const uint8_t data, const bool simulateDelay) {
  int16_t physicalAddr = translateAddr(virtualAddr);
  if (physicalAddr == -1) {
    lastStatus = AccessStatus::SEG_FAULT;
    stats.segFaults++;
    stats.totalCycles += COST_SEGFAULT;
    return;
  }
  physicalAddr = static_cast<uint8_t>(physicalAddr);

  stats.totalCycles += COST_L1_HIT;

  int16_t l1Index = findInL1(physicalAddr);
  if (l1Index != -1) {
    l1[l1Index].data = data;
    l1[l1Index].dirty = true;
    updateAccessL1(l1Index);
    lastStatus = AccessStatus::HIT_L1;
    stats.l1Hits++;
    return;
  }

  l1Index = findEvictionCandidateL1();
  if (l1[l1Index].valid) {
    stats.totalCycles += COST_L2_HIT;
    if (l1[l1Index].dirty) {
      stats.writeBacks++;
      stats.totalCycles += COST_WRITEBACK;
    }
    updateL2(l1[l1Index].physicalAddr, l1[l1Index].data, l1[l1Index].dirty, simulateDelay);
  }

  l1[l1Index].physicalAddr = physicalAddr;
  l1[l1Index].data = data;
  l1[l1Index].valid = true;
  l1[l1Index].dirty = true;
  l1[l1Index].frequency = 1;
  updateAccessL1(l1Index);
  stats.misses++;

  lastStatus = AccessStatus::MISS;
}

int16_t CacheController::translateAddr(const uint8_t virtualAddr) {
  if (virtualAddr >= VIRTUAL_SIZE || !pageTable[virtualAddr].valid) {
    return -1;
  }
  return pageTable[virtualAddr].physicalAddr;
}

int16_t CacheController::findInL1(const uint8_t physicalAddr) {
  for (uint8_t i = 0; i < L1_SIZE; ++i) {
    if (l1[i].valid && l1[i].physicalAddr == physicalAddr) {
      return i;
    }
  }
  return -1;
}

int16_t CacheController::findEvictionCandidateL1() {
  for (uint8_t i = 0; i < L1_SIZE; ++i) {
    if (!l1[i].valid) {
      return i;
    }
  }

  int16_t candidateIndex = 0;

  switch(currentPolicy) {
    case EvictionPolicy::LRU:
      for (uint8_t i = 1; i < L1_SIZE; ++i) {
        if (l1[i].age > l1[candidateIndex].age) {
          candidateIndex = i;
        }
      }
      break;
    case EvictionPolicy::LFU:
      for (uint8_t i = 1; i < L1_SIZE; ++i) {
        if (l1[i].frequency < l1[candidateIndex].frequency) {
          candidateIndex = i;
        }
      }
      break;
    case EvictionPolicy::MRU:
      for (uint8_t i = 1; i < L1_SIZE; ++i) {
        if (l1[i].age < l1[candidateIndex].age) {
          candidateIndex = i;
        }
      }
      break;
  }

  return candidateIndex;
}

void CacheController::updateAccessL1(const uint8_t hitIndex) {
  for (uint8_t i = 0; i < L1_SIZE; ++i) {
    if (l1[i].valid && i != hitIndex) {
      l1[i].age++;
    }
  }
  l1[hitIndex].age = 0;

  if (l1[hitIndex].frequency < 255) {
    l1[hitIndex].frequency++;
  }
}

void CacheController::insertL1Clean(const uint8_t physicalAddr, const uint8_t data, const bool simulateDelay) {
  int16_t l1Index = findEvictionCandidateL1();
  
  if (l1[l1Index].valid) {
    stats.totalCycles += COST_L2_HIT;
    if (l1[l1Index].dirty) {
      stats.writeBacks++;
      stats.totalCycles += COST_WRITEBACK;
    }
    updateL2(l1[l1Index].physicalAddr, l1[l1Index].data, l1[l1Index].dirty, simulateDelay);
  }

  l1[l1Index].physicalAddr = physicalAddr;
  l1[l1Index].data = data;
  l1[l1Index].valid = true;
  l1[l1Index].dirty = false;
  l1[l1Index].frequency = 1;
  updateAccessL1(l1Index);
}

bool CacheController::checkL2(const uint8_t physicalAddr, uint8_t& data) {
  int16_t l2Index = findInL2(physicalAddr);
  if (l2Index != -1) {
    data = MemoryHAL::readL2(l2Index);
    updateAccessL2(l2Index);
    return true;
  }
  return false;
}

void CacheController::updateL2(const uint8_t physicalAddr, const uint8_t data, bool markDirty, const bool simulateDelay) {
  int16_t l2Index = findInL2(physicalAddr);

  if (l2Index != -1) {
    MemoryHAL::writeL2(l2Index, data);
    l2[l2Index].dirty = markDirty;
    updateAccessL2(l2Index);
    return;
  }

  l2Index = findEvictionCandidateL2();
  if (l2[l2Index].valid && l2[l2Index].dirty) {
    stats.totalCycles += COST_MAIN_HIT;
    stats.writeBacks++;
    flushL2Entry(l2Index, simulateDelay);
  }

  l2[l2Index].physicalAddr = physicalAddr;
  l2[l2Index].valid = true;
  l2[l2Index].dirty = markDirty;
  l2[l2Index].frequency = 1;
  MemoryHAL::writeL2(l2Index, data);
  updateAccessL2(l2Index);
}

int16_t CacheController::findInL2(const uint8_t physicalAddr) {
  for (uint8_t i = 0; i < L2_SIZE; ++i) {
    if (l2[i].valid && l2[i].physicalAddr == physicalAddr) {
      return i;
    }
  }
  return -1;
}

int16_t CacheController::findEvictionCandidateL2() {
  for (uint8_t i = 0; i < L2_SIZE; ++i) {
    if (!l2[i].valid) {
      return i;
    }
  }

  int16_t candidateIndex = 0;

  switch(currentPolicy) {
    case EvictionPolicy::LRU:
      for (uint8_t i = 1; i < L2_SIZE; ++i) {
        if (l2[i].age > l2[candidateIndex].age) {
          candidateIndex = i;
        }
      }
      break;
    case EvictionPolicy::LFU:
      for (uint8_t i = 1; i < L2_SIZE; ++i) {
        if (l2[i].frequency < l2[candidateIndex].frequency) {
          candidateIndex = i;
        }
      }
      break;
    case EvictionPolicy::MRU:
      for (uint8_t i = 1; i < L2_SIZE; ++i) {
        if (l2[i].age < l2[candidateIndex].age) {
          candidateIndex = i;
        }
      }
      break;
  }

  return candidateIndex;
}

void CacheController::updateAccessL2(const uint8_t hitIndex) {
  for (uint8_t i = 0; i < L2_SIZE; ++i) {
    if (l2[i].valid && i != hitIndex) {
      l2[i].age++;
    }
  }
  l2[hitIndex].age = 0;

  if (l2[hitIndex].frequency < 255) {
    l2[hitIndex].frequency++;
  }
}

void CacheController::flushL2Entry(const uint8_t index, const bool simulateDelay) {
  uint8_t data = MemoryHAL::readL2(index);
  MemoryHAL::writeMain(l2[index].physicalAddr, data, simulateDelay);
  l2[index].dirty = false;
}

bool CacheController::hasDirtyL1() const {
  for (uint8_t i = 0; i < L1_SIZE; ++i) {
    if (l1[i].valid && l1[i].dirty) {
      return true;
    }
  }
  return false;
}

AccessStatus CacheController::getLastStatus() const {
  return lastStatus;
}
