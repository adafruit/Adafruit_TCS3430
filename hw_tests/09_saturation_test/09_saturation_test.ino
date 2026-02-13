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
}

void setup() {
  Serial.begin(115200);
  while (!Serial)
    delay(10);
  Serial.println("TEST_START: 09_saturation_test");

  if (!tcs.begin()) {
    Serial.println("TEST_FAIL: 09_saturation_test: begin() failed");
    return;
  }

  pixels.begin();
  pixels.setBrightness(255);
  setAll(255, 255, 255);

  Serial.println("Settings: gain=128X, integration=711 ms, brightness=255");
  tcs.setALSGain(TCS3430_GAIN_128X);
  // Max integration time: ATIME=255 -> 711ms
  tcs.setIntegrationTime(711.0f);
  tcs.clearALSSaturated();
  delay(800); // wait for full integration cycle

  // Check multiple times - ASAT may take a cycle to latch
  bool saturated = false;
  uint8_t tries = 0;
  for (uint8_t i = 0; i < 5; i++) {
    tries = i + 1;
    if (tcs.isALSSaturated()) {
      saturated = true;
      break;
    }
    delay(300);
  }

  if (!saturated) {
    Serial.println("TEST_FAIL: 09_saturation_test: ASAT not set");
    setAll(0, 0, 0);
    return;
  }

  Serial.print("ASAT detected after ");
  Serial.print(tries);
  Serial.println(" checks");

  setAll(0, 0, 0);
  Serial.println("TEST_PASS: 09_saturation_test");
}

void loop() { delay(1000); }
