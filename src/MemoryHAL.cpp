#include "MemoryHAL.h"

SoftwareWire MemoryHAL::diskBus(SoftwareWire(SOFT_SDA, SOFT_SCL));

void MemoryHAL::init() {
  Wire.begin();
  diskBus.begin();
}

uint8_t MemoryHAL::readL2(const uint16_t addr) {
  return readI2C(Wire, addr);
}

void MemoryHAL::writeL2(const uint16_t addr, const uint8_t data) {
  writeI2C(Wire, addr, data);
}

uint8_t MemoryHAL::readMain(const uint16_t addr, const bool simulateDelay) {
  if (simulateDelay) {
    delay(MAIN_MEMORY_DELAY_MS); // Simulate slower access time
  }
  return readI2C(diskBus, addr);
}

void MemoryHAL::writeMain(const uint16_t addr, const uint8_t data, const bool simulateDelay) {
  if (simulateDelay) {
    delay(MAIN_MEMORY_DELAY_MS); // Simulate slower access time
  }
  writeI2C(diskBus, addr, data);
}
