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

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    delay(10);
  }

  Serial.println("TEST_START: test_thresholds");

  if (!tcs.begin()) {
    Serial.println("TEST_FAIL: test_thresholds: begin() failed");
    return;
  }

  pixels.begin();
  pixels.setBrightness(80);
  setAll(0, 0, 0);

  tcs.setIntegrationTime(100.0f);
  tcs.setALSGain(TCS3430_GAIN_16X);
  tcs.setInterruptClearOnRead(false);
  tcs.setInterruptPersistence(TCS3430_PERS_1);

  uint16_t dark = 0;
  if (!readAverage(&dark)) {
    Serial.println("TEST_FAIL: test_thresholds: dark read failed");
    return;
  }
  Serial.print("Dark Z average: ");
  Serial.println(dark);

  setAll(255, 255, 255);
  uint16_t bright = 0;
  if (!readAverage(&bright)) {
    Serial.println("TEST_FAIL: test_thresholds: bright read failed");
    setAll(0, 0, 0);
    return;
  }
  Serial.print("Bright Z average: ");
  Serial.println(bright);
  setAll(0, 0, 0);

  uint16_t low = dark + 20;
  uint16_t high = (bright > 20) ? (bright - 20) : (bright / 2);
  if (high <= low) {
    high = low + 10;
  }

  Serial.print("Setting thresholds low=");
  Serial.print(low);
  Serial.print(" high=");
  Serial.println(high);

  if (!tcs.setALSThresholdLow(low) || !tcs.setALSThresholdHigh(high)) {
    Serial.println("TEST_FAIL: test_thresholds: set failed");
    return;
  }

  tcs.enableALSInt(true);
  Serial.println("ALS interrupt enabled");
  tcs.clearALSInterrupt();

  setAll(255, 255, 255);
  uint32_t start = millis();
  while (!tcs.isALSInterrupt() && (millis() - start) < 2000) {
    delay(10);
  }
  bool bright_int = tcs.isALSInterrupt();
  Serial.print("AINT after bright: ");
  Serial.println(bright_int ? "true" : "false");
  if (!bright_int) {
    Serial.print("TEST_FAIL: test_thresholds: AINT not set on bright, low=");
    Serial.print(low);
    Serial.print(" high=");
    Serial.println(high);
    setAll(0, 0, 0);
    return;
  }

  tcs.clearALSInterrupt();
  setAll(0, 0, 0);
  start = millis();
  while (!tcs.isALSInterrupt() && (millis() - start) < 2000) {
    delay(10);
  }
  bool dark_int = tcs.isALSInterrupt();
  Serial.print("AINT after dark: ");
  Serial.println(dark_int ? "true" : "false");
  if (!dark_int) {
    Serial.println("TEST_FAIL: test_thresholds: AINT not set on dark");
    return;
  }

  Serial.println("TEST_PASS: test_thresholds");
}

void loop() {
  delay(1000);
}
