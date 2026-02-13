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

static uint32_t measureInterruptDelay(uint32_t timeout_us) {
  uint32_t start = micros();
  while ((micros() - start) < timeout_us) {
    if (tcs.isALSInterrupt()) {
      return micros() - start;
    }
    delay(1);
  }
  return 0;
}

static uint32_t measureAverageDelay(uint8_t samples, uint32_t timeout_us) {
  uint32_t sum = 0;
  for (uint8_t i = 0; i < samples; i++) {
    tcs.clearALSInterrupt();
    uint32_t delay_us = measureInterruptDelay(timeout_us);
    if (delay_us == 0) {
      return 0;
    }
    sum += delay_us;
  }
  return sum / samples;
}

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    delay(10);
  }

  Serial.println("TEST_START: test_wlong");

  if (!tcs.begin()) {
    Serial.println("TEST_FAIL: test_wlong: begin() failed");
    return;
  }

  pixels.begin();
  pixels.setBrightness(80);
  setAll(255, 255, 255);

  tcs.setIntegrationTime(2.78f);
  tcs.setALSGain(TCS3430_GAIN_16X);
  tcs.setInterruptClearOnRead(false);
  tcs.setInterruptPersistence(TCS3430_PERS_EVERY);

  uint16_t bright = 0;
  if (!readAverage(&bright)) {
    Serial.println("TEST_FAIL: test_wlong: bright read failed");
    return;
  }

  uint16_t high = (bright > 10) ? (bright / 2) : (bright + 1);
  if (!tcs.setALSThresholdLow(0) || !tcs.setALSThresholdHigh(high)) {
    Serial.println("TEST_FAIL: test_wlong: set thresholds failed");
    return;
  }

  tcs.enableALSInt(true);
  if (!tcs.waitEnable(true) || !tcs.setWaitTime(2.78f)) {
    Serial.println("TEST_FAIL: test_wlong: enable wait failed");
    return;
  }

  if (!tcs.setWaitLong(false)) {
    Serial.println("TEST_FAIL: test_wlong: disable WLONG failed");
    return;
  }

  uint32_t no_wlong = measureAverageDelay(5, 1000000);
  if (no_wlong == 0) {
    Serial.println("TEST_FAIL: test_wlong: no-wlong timeout");
    return;
  }

  if (!tcs.setWaitLong(true)) {
    Serial.println("TEST_FAIL: test_wlong: enable WLONG failed");
    return;
  }

  uint32_t with_wlong = measureAverageDelay(5, 2000000);
  if (with_wlong == 0) {
    Serial.println("TEST_FAIL: test_wlong: wlong timeout");
    return;
  }

  Serial.print("No-WLONG delay us: ");
  Serial.println(no_wlong);
  Serial.print("WLONG delay us: ");
  Serial.println(with_wlong);

  if (with_wlong < (no_wlong * 5)) {
    Serial.println("TEST_FAIL: test_wlong: WLONG did not extend delay");
    return;
  }

  Serial.println("TEST_PASS: test_wlong");
}

void loop() {
  delay(1000);
}
