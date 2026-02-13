#include <Adafruit_NeoPixel.h>
#include "Adafruit_TCS3430.h"

#define PIXEL_PIN 6
#define PIXEL_COUNT 16

Adafruit_TCS3430 tcs = Adafruit_TCS3430();
Adafruit_NeoPixel pixels(PIXEL_COUNT, PIXEL_PIN, NEO_GRB + NEO_KHZ800);

static void setAll(uint8_t r, uint8_t g, uint8_t b) {
  for (uint16_t i = 0; i < PIXEL_COUNT; i++)
    pixels.setPixelColor(i, pixels.Color(r, g, b));
  pixels.show();
  delay(200);
}

static bool readAverageY(uint16_t *y_out) {
  const uint8_t samples = 4;
  uint32_t sy = 0;
  for (uint8_t i = 0; i < samples; i++) {
    uint16_t x, y, z, ir1;
    if (!tcs.getChannels(&x, &y, &z, &ir1))
      return false;
    sy += y;
    delay(30);
  }
  *y_out = sy / samples;
  return true;
}

void setup() {
  Serial.begin(115200);
  while (!Serial)
    delay(10);
  Serial.println("TEST_START: 14_persistence_test");

  if (!tcs.begin()) {
    Serial.println("TEST_FAIL: 14_persistence_test: begin() failed");
    return;
  }

  pixels.begin();
  pixels.setBrightness(80);
  setAll(0, 0, 0);

  tcs.setIntegrationTime(50.0f); // ~50ms per cycle
  tcs.setALSGain(TCS3430_GAIN_16X);
  tcs.setInterruptClearOnRead(false);

  // Calibrate: read dark and bright
  uint16_t dark = 0;
  if (!readAverageY(&dark)) {
    Serial.println("TEST_FAIL: 14_persistence_test: dark read failed");
    return;
  }
  Serial.print("  dark=");
  Serial.println(dark);

  setAll(255, 255, 255);
  uint16_t bright = 0;
  if (!readAverageY(&bright)) {
    Serial.println("TEST_FAIL: 14_persistence_test: bright read failed");
    setAll(0, 0, 0);
    return;
  }
  Serial.print("  bright=");
  Serial.println(bright);

  // Thresholds: window around dark so bright light is OUTSIDE
  // low=0, high = dark + small margin -> bright readings exceed high -> fires
  uint16_t high = dark + (bright - dark) / 4;
  if (high < dark + 10)
    high = dark + 10;
  tcs.setALSThresholdLow(0);
  tcs.setALSThresholdHigh(high);
  Serial.print("  threshold high=");
  Serial.println(high);

  // Set persistence to 10 cycles
  tcs.setInterruptPersistence(TCS3430_PERS_10);
  tcs.enableALSInt(true);

  // Clean start: disable ALS, clear, re-enable
  setAll(0, 0, 0);
  delay(200);
  tcs.ALSEnable(false);
  tcs.clearALSInterrupt();
  tcs.ALSEnable(true);

  // Turn on light - should need 10 cycles (~500ms at 50ms/cycle) to fire
  setAll(255, 255, 255);

  // Check after 2 cycles (~100ms) - should NOT have fired yet
  delay(150);
  tcs.ALSEnable(false);
  bool early = tcs.isALSInterrupt();
  tcs.ALSEnable(true);
  if (early) {
    Serial.println("TEST_FAIL: 14_persistence_test: interrupt fired too early");
    setAll(0, 0, 0);
    return;
  }
  Serial.println("  no interrupt after 150ms: OK");

  // Wait for remaining cycles - should fire by ~600ms total
  delay(600);
  if (!tcs.isALSInterrupt()) {
    // Give extra time
    delay(500);
    if (!tcs.isALSInterrupt()) {
      Serial.println("TEST_FAIL: 14_persistence_test: interrupt did not fire");
      setAll(0, 0, 0);
      return;
    }
  }
  Serial.println("  interrupt fired after persistence: OK");

  // Now test PERS_EVERY - should fire on first cycle
  tcs.ALSEnable(false);
  tcs.clearALSInterrupt();
  tcs.setInterruptPersistence(TCS3430_PERS_EVERY);
  tcs.ALSEnable(true);

  delay(100);
  if (!tcs.isALSInterrupt()) {
    delay(200);
    if (!tcs.isALSInterrupt()) {
      Serial.println("TEST_FAIL: 14_persistence_test: PERS_EVERY did not fire");
      setAll(0, 0, 0);
      return;
    }
  }
  Serial.println("  PERS_EVERY fires immediately: OK");

  setAll(0, 0, 0);
  tcs.enableALSInt(false);
  Serial.println("TEST_PASS: 14_persistence_test");
}

void loop() { delay(1000); }
