#include <LiquidCrystal.h>
#include "CacheController.h"
#include "ButtonManager.h"
#include "LEDController.h"
#include "LCDRenderer.h"

CacheController cacheController;
ButtonManager buttonManager;
LEDController ledController;
LCDRenderer lcdRenderer;

void setup() {
  cacheController.init();
  ButtonManager::init();
  LEDController::init();
  LCDRenderer::init();
}

void loop() {
}
