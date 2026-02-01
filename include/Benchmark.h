#pragma once

#include <Arduino.h>
#include "CacheController.h"
#include "Constants.h"

enum class BenchmarkPattern {
  SEQUENTIAL, // 0, 1, 2, ...
  RANDOM,    
  TEMPORAL,   // small set of addr
  STRIDED,    // every Nth addr
  HOTSPOT,    // frequent access to few addr
  COMPARATIVE
};

struct BenchmarkResult {
    BenchmarkPattern pattern;
    EvictionPolicy policy;
    uint32_t totalAccesses;
    uint32_t l1Hits;
    uint32_t l2Hits;
    uint32_t misses;
    uint32_t totalCycles;

    // Comparative
    EvictionPolicy bestPolicy;
    double bestHitRate;
    double policyHitRates[POLICY_COUNT];

    double getHitRate() const {
        return totalAccesses > 0 ? 
        static_cast<double>(l1Hits + l2Hits) / totalAccesses * 100.0
        : 0.0;
    }

    double getAvgCycles() const {
        return totalAccesses > 0 ? 
        static_cast<double>(totalCycles) / totalAccesses
        : 0.0;
    }
};

class Benchmark {
public:
  static void init(CacheController* cachePtr);

  static BenchmarkResult run(const BenchmarkPattern pattern, const uint16_t numAccesses = BENCHMARK_ACCESS_COUNT);
  static const char* getPatternName(const BenchmarkPattern pattern);
  static BenchmarkPattern getPatternByIndex(const uint8_t index);

  static void printResultSerial(const BenchmarkResult& result);

private:
  static CacheController* cache;
  static uint16_t seed;

  static uint8_t nextSequential(const uint16_t index);
  static uint8_t nextRandom();
  static uint8_t nextTemporal(const uint16_t index);
  static uint8_t nextStrided(const uint16_t index);
  static uint8_t nextHotspot(const uint16_t index);

  static BenchmarkResult runComparative(const uint16_t numAccesses);
  static BenchmarkResult runSinglePattern(const BenchmarkPattern pattern, const uint16_t numAccesses);
};
