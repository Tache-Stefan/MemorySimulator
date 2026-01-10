# Hardware Architecture for Memory Management and Cache Simulation

## General Description
The project aims to physically and logically simulate the memory subsystem of a modern processor. The objective is to build a system that demonstrates the performance difference between memory hierarchies (L1, L2, Main Memory) and implements data management algorithms.

## Bill of Materials
- 1 Arduino Uno
- 2 External EEPROM modules
- 1 LCD Display
- 3 Buttons
- Resistors
- 1 Potentiometer
- Wires
- Breadboard

## System Architecture

### Memory Hierarchy:
- <b>L1 Cache</b>: Implemented in Arduino's internal SRAM.
    - <b>Speed</b>: Instant.
    - <b>Capacity</b>: Very small.
- <b>L2 Cache</b>: External EEPROM module.
    - <b>Speed</b>: Fast.
    - <b>Capacity</b>: Medium.
- <b>Main Memory</b>: External EEPROM module.
    - <b>Speed</b>: Artificially slowed to simulate a Hard Disk / SSD.

## Software/Algorithms

### Memory Management
- <b>Virtual Addressing</b>: The user interacts with virtual addresses.
- <b>Page Table Simulation</b>: The system translates requests into physical addresses.
- <b>Protection</b>: Simulating "Segmentation Fault" if a virtual address is not mapped.

### Cache
- <b>LRU (Least Recently Used)</b>: When L1 is full, the system automatically evicts the oldest unused data.
- <b>Write-Back</b>: Data modifications are done only in Cache (L1/L2). Writing to Main Memory is done only when modified data is evicted from memory.

## Questions
<b>Q1 - What is the system boundary?</b>
- <b>Inside</b>: Arduino Uno, EEPROM modules, LCD, buttons.
- <b>Outside</b>: The human user.

<b>Q2 - Where does intelligence live?</b>
- It acts as the controller simulating the CPU, Memory Management Unit, and Cache logic.

<b>Q3 - What is the hardest technical problem?</b>
- Keeping data synchronized across three memory layers using Write-Back.

<b>Q4 - What is the minimum demo?</b>
- Demonstrating a slow cache miss on a specific address, followed immediately by a cache hit on that same address.

<b>Q5 - Why is this not just a tutorial?</b>
- This project builds a custom OS simulation that manages bytes, which requires custom algorithm design.
