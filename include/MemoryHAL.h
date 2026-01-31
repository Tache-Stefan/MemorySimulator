#pragma once

#include <Arduino.h>
#include <Wire.h>
#include <SoftwareWire.h>
#include "MemoryTypes.h"
#include "Constants.h"

class MemoryHAL {
public:
  static void init();

  static uint8_t readL2(const uint16_t addr);
  static void writeL2(const uint16_t addr, const uint8_t data);
  static uint8_t readMain(const uint16_t addr, const bool simulateDelay = true);
  static void writeMain(const uint16_t addr, const uint8_t data, const bool simulateDelay = true);

public:
  static SoftwareWire diskBus;

  static constexpr uint8_t SOFT_SDA = 10;
  static constexpr uint8_t SOFT_SCL = 11;
  static constexpr uint16_t MAIN_MEMORY_DELAY_MS = 1500;
  static constexpr uint8_t EEPROM_WRITE_DELAY_MS = 5;

  template<typename WireType>
  static uint8_t readI2C(WireType& bus, const uint16_t addr);
  template<typename WireType>
  static void writeI2C(WireType& bus, const uint16_t addr, const uint8_t data);
};

template<typename WireType>
uint8_t MemoryHAL::readI2C(WireType& bus, const uint16_t addr) {
  bus.beginTransmission(EEPROM_ADDR);
  bus.write(static_cast<uint8_t>(addr >> 8));
  bus.write(static_cast<uint8_t>(addr & 0xFF));
  bus.endTransmission();
  
  bus.requestFrom(EEPROM_ADDR, static_cast<uint8_t>(1));
  if (bus.available()) return bus.read();
  return 0xFF;
}

template<typename WireType>
void MemoryHAL::writeI2C(WireType& bus, const uint16_t addr, const uint8_t data) {
  uint8_t currentValue = readI2C(bus, addr);
  if (currentValue == data) return;
  
  bus.beginTransmission(EEPROM_ADDR);
  bus.write(static_cast<uint8_t>(addr >> 8));
  bus.write(static_cast<uint8_t>(addr & 0xFF));
  bus.write(data);
  bus.endTransmission();
  delay(EEPROM_WRITE_DELAY_MS);
}
