#include "Benchmark.h"

CacheController* Benchmark::cache = nullptr;
uint16_t Benchmark::seed = 123;

void Benchmark::init(CacheController* cachePtr) {
  cache = cachePtr;
}

BenchmarkResult Benchmark::run(const BenchmarkPattern pattern, const uint16_t numAccesses) {
  BenchmarkResult result;
  result.pattern = pattern;
  result.policy = cache->getEvictionPolicy();
  result.totalAccesses = numAccesses;
  
  cache->reset();
  randomSeed(seed);

  for (uint16_t i = 0; i < numAccesses; ++i) {
    uint8_t addr = 0;

    switch (pattern) {
      case BenchmarkPattern::SEQUENTIAL:
        addr = nextSequential(i);
        break;
      case BenchmarkPattern::RANDOM:
        addr = nextRandom();
        break;
      case BenchmarkPattern::TEMPORAL:
        addr = nextTemporal(i);
        break;
      case BenchmarkPattern::STRIDED:
        addr = nextStrided(i);
        break;
      case BenchmarkPattern::HOTSPOT:
        addr = nextHotspot(i);
        break;
      default:
        addr = 0;
        break;
    }

    if (random(0, 2) == 0) {
      cache->readByte(addr, false);
    } else {
      cache->writeByte(addr, static_cast<uint8_t>(i & 0xFF), false);
    }
  }

  const CacheStats& stats = cache->getStats();
  result.l1Hits = stats.l1Hits;
  result.l2Hits = stats.l2Hits;
  result.misses = stats.misses;
  result.totalCycles = stats.totalCycles;

  return result;
}

const char* Benchmark::getPatternName(const BenchmarkPattern pattern) {
  switch (pattern) {
    case BenchmarkPattern::SEQUENTIAL: return "SEQUENTIAL";
    case BenchmarkPattern::RANDOM:     return "RANDOM";
    case BenchmarkPattern::TEMPORAL:   return "TEMPORAL";
    case BenchmarkPattern::STRIDED:    return "STRIDED";
    case BenchmarkPattern::HOTSPOT:    return "HOTSPOT";
    default:                           return "UNKNOWN";
  }
}

BenchmarkPattern Benchmark::getPatternByIndex(const uint8_t index) {
  return static_cast<BenchmarkPattern>(index % BENCHMARK_PATTERN_COUNT);
}

void Benchmark::printResultSerial(const BenchmarkResult& result) {
  Serial.println(F("=== Benchmark Result ==="));
  Serial.print(F("Pattern: "));
  Serial.println(getPatternName(result.pattern));
  Serial.print(F("Policy: "));
  switch (result.policy) {
    case EvictionPolicy::LRU: Serial.println(F("LRU")); break;
    case EvictionPolicy::LFU: Serial.println(F("LFU")); break;
    case EvictionPolicy::MRU: Serial.println(F("MRU")); break;
    default:                  Serial.println(F("UNKNOWN")); break;
  }
  Serial.print(F("L1 Hits: "));
  Serial.println(result.l1Hits);
  Serial.print(F("L2 Hits: "));
  Serial.println(result.l2Hits);
  Serial.print(F("Misses: "));
  Serial.println(result.misses);
  Serial.print(F("Hit Rate: "));
  Serial.print(result.getHitRate(), 1);
  Serial.println(F("%"));
  Serial.print(F("Avg Cycles: "));
  Serial.println(result.getAvgCycles(), 1);
  Serial.println();
}

uint8_t Benchmark::nextSequential(const uint16_t index) {
  return static_cast<uint8_t>(index % VIRTUAL_SIZE);
}

uint8_t Benchmark::nextRandom() {
  return static_cast<uint8_t>(random(0, VIRTUAL_SIZE));
}

uint8_t Benchmark::nextTemporal(const uint16_t index) {
  static const uint8_t workingSet[] = {0, 1, 2, 3, 0, 1, 0, 2};
  return workingSet[index % 8];
}

uint8_t Benchmark::nextStrided(const uint16_t index) {
  return static_cast<uint8_t>((index * 2) % VIRTUAL_SIZE);
}

uint8_t Benchmark::nextHotspot(const uint16_t index) {
  uint16_t r = random(0, 100);
  if (r < 80) {
    return static_cast<uint8_t>(index % 4);
  } else {
    return static_cast<uint8_t>(random(4, VIRTUAL_SIZE));
  }
}
