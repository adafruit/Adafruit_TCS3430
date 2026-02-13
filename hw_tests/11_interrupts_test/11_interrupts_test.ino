#include <Adafruit_NeoPixel.h>
#include <Wire.h>

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
  delay(500);
}

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    delay(10);
  }

  Serial.println("TEST_START: test_interrupts");

  if (!tcs.begin()) {
    Serial.println("TEST_FAIL: test_interrupts: begin() failed");
    return;
  }

  pixels.begin();
  pixels.setBrightness(80);
  setAll(0, 0, 0);

  tcs.setIntegrationTime(100.0f);
  tcs.setALSGain(TCS3430_GAIN_16X);
  // Set high threshold low so bright light exceeds it and triggers AINT
  tcs.setALSThresholdLow(0);
  tcs.setALSThresholdHigh(100);
  tcs.setInterruptClearOnRead(false);
  tcs.enableALSInt(true);
  tcs.clearALSInterrupt();

  setAll(255, 255, 255);

  // Poll for interrupt
  bool intFired = false;
  for (uint8_t i = 0; i < 20; i++) {
    delay(100);
    if (tcs.isALSInterrupt()) {
      intFired = true;
      break;
    }
  }

  if (!intFired) {
    Serial.println("TEST_FAIL: test_interrupts: ALS interrupt never fired (2s timeout)");
    setAll(0, 0, 0);
    return;
  }

  tcs.clearALSInterrupt();
  setAll(0, 0, 0);
  Serial.println("TEST_PASS: test_interrupts");
}

void loop() {
  delay(1000);
}
