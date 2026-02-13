#include <Adafruit_NeoPixel.h>

#include "Adafruit_TCS3430.h"

#define PIXEL_PIN 6
#define PIXEL_COUNT 16

Adafruit_TCS3430 tcs = Adafruit_TCS3430();
Adafruit_NeoPixel pixels(PIXEL_COUNT, PIXEL_PIN, NEO_GRB + NEO_KHZ800);

static void setAll(uint8_t r, uint8_t g, uint8_t b) {
  for (uint16_t i = 0; i < PIXEL_COUNT; i++) {
    pixels.setPixelColor(i, pixels.Color(r, g, b));
  }
  pixels.show();
  delay(200);
}

static bool readAverage(uint16_t* z) {
  const uint8_t samples = 4;
  uint32_t sum = 0;
  for (uint8_t i = 0; i < samples; i++) {
    uint16_t x, y, zval, ir1;
    if (!tcs.getChannels(&x, &y, &zval, &ir1)) {
      return false;
    }
    sum += zval;
    delay(20);
  }
  *z = sum / samples;
  return true;
}

static bool waitForInterrupt(uint32_t timeout_ms) {
  uint32_t start = millis();
  while ((millis() - start) < timeout_ms) {
    if (tcs.isALSInterrupt()) {
      return true;
    }
    delay(1);
  }
  return false;
}

static bool readZ(uint16_t* z) {
  uint16_t x, y, zval, ir1;
  if (!tcs.getChannels(&x, &y, &zval, &ir1)) {
    return false;
  }
  *z = zval;
  return true;
}

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    delay(10);
  }

  Serial.println("TEST_START: test_sai");

  if (!tcs.begin()) {
    Serial.println("TEST_FAIL: test_sai: begin() failed");
    return;
  }

  pixels.begin();
  pixels.setBrightness(80);
  setAll(0, 0, 0);

  tcs.setIntegrationTime(50.0f);
  tcs.setALSGain(TCS3430_GAIN_16X);
  tcs.setInterruptPersistence(TCS3430_PERS_EVERY);
  tcs.setInterruptClearOnRead(false);

  setAll(255, 255, 255);
  uint16_t bright = 0;
  if (!readAverage(&bright)) {
    Serial.println("TEST_FAIL: test_sai: bright read failed");
    return;
  }

  uint16_t high = (bright > 10) ? (bright / 2) : (bright + 1);
  if (!tcs.setALSThresholdLow(0) || !tcs.setALSThresholdHigh(high)) {
    Serial.println("TEST_FAIL: test_sai: set thresholds failed");
    return;
  }

  tcs.enableALSInt(true);
  tcs.setSleepAfterInterrupt(true);
  tcs.clearALSInterrupt();

  setAll(255, 255, 255);
  if (!waitForInterrupt(2000)) {
    Serial.println("TEST_FAIL: test_sai: interrupt timeout");
    return;
  }

  uint16_t z1 = 0;
  uint16_t z2 = 0;
  if (!readZ(&z1)) {
    Serial.println("TEST_FAIL: test_sai: read after interrupt failed");
    return;
  }
  delay(150);
  if (!readZ(&z2)) {
    Serial.println("TEST_FAIL: test_sai: second read failed");
    return;
  }

  Serial.print("z1 (frozen): ");
  Serial.println(z1);
  Serial.print("z2 (frozen): ");
  Serial.println(z2);
  if (z1 != z2) {
    Serial.println("TEST_FAIL: test_sai: readings not frozen after interrupt");
    return;
  }

  setAll(0, 0, 0);
  delay(50);
  tcs.setALSThresholdLow(0);
  tcs.setALSThresholdHigh(0xFFFF);
  tcs.setSleepAfterInterrupt(false);
  tcs.clearALSInterrupt();
  tcs.ALSEnable(false);
  delay(10);
  tcs.ALSEnable(true);
  delay(400);

  uint16_t z3 = 0;
  uint16_t z4 = 0;
  if (!readZ(&z3)) {
    Serial.println("TEST_FAIL: test_sai: read after clear failed");
    return;
  }
  setAll(255, 255, 255);
  delay(400);
  if (!readZ(&z4)) {
    Serial.println("TEST_FAIL: test_sai: second read after clear failed");
    return;
  }

  Serial.print("z3 (dark): ");
  Serial.println(z3);
  Serial.print("z4 (bright): ");
  Serial.println(z4);

  if (z4 <= z3) {
    Serial.println("TEST_FAIL: test_sai: readings did not update after clear");
    return;
  }

  Serial.println("TEST_PASS: test_sai");
}

void loop() {
  delay(1000);
}
