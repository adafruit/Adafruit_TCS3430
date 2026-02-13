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
  Serial.println("Enabling ALS interrupt (AIEN)");
  tcs.enableALSInt(true);
  tcs.clearALSInterrupt();

  Serial.print("AINT status before light: ");
  Serial.println(tcs.isALSInterrupt() ? "true" : "false");

  setAll(255, 255, 255);

  // Poll for interrupt
  bool intFired = false;
  for (uint8_t i = 0; i < 20; i++) {
    delay(100);
    bool aint = tcs.isALSInterrupt();
    Serial.print("Poll ");
    Serial.print(i);
    Serial.print(" AINT=");
    Serial.println(aint ? "true" : "false");
    if (aint) {
      intFired = true;
      break;
    }
  }

  if (!intFired) {
    Serial.println(
        "TEST_FAIL: test_interrupts: ALS interrupt never fired (2s timeout)");
    setAll(0, 0, 0);
    return;
  }

  Serial.println("Clearing ALS interrupt");
  tcs.clearALSInterrupt();
  Serial.print("AINT status after clear: ");
  Serial.println(tcs.isALSInterrupt() ? "true" : "false");

  Serial.println("Disabling ALS interrupt (AIEN)");
  tcs.enableALSInt(false);

  setAll(0, 0, 0);
  Serial.println("TEST_PASS: test_interrupts");
}

void loop() {
  delay(1000);
}
