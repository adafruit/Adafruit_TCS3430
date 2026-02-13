#include <Adafruit_NeoPixel.h>

#include "Adafruit_TCS3430.h"

#define PIXEL_PIN 6
#define PIXEL_COUNT 16

Adafruit_TCS3430 tcs = Adafruit_TCS3430();
Adafruit_NeoPixel pixels(PIXEL_COUNT, PIXEL_PIN, NEO_GRB + NEO_KHZ800);

static bool readAverageY(uint16_t* y_out) {
  const uint8_t samples = 5;
  uint32_t sy = 0;
  for (uint8_t i = 0; i < samples; i++) {
    uint16_t x, y, z, ir1;
    if (!tcs.getChannels(&x, &y, &z, &ir1)) {
      return false;
    }
    sy += y;
    delay(50);
  }
  *y_out = sy / samples;
  return true;
}

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    delay(10);
  }

  Serial.println("TEST_START: test_integration_time");

  if (!tcs.begin()) {
    Serial.println("TEST_FAIL: test_integration_time: begin() failed");
    return;
  }

  tcs.setALSGain(TCS3430_GAIN_16X);

  pixels.begin();
  pixels.setBrightness(80);
  for (uint16_t i = 0; i < PIXEL_COUNT; i++) {
    pixels.setPixelColor(i, pixels.Color(255, 255, 255));
  }
  pixels.show();
  delay(500);

  tcs.setIntegrationTime(20.0f);
  delay(30);
  uint16_t y_short = 0;
  if (!readAverageY(&y_short)) {
    Serial.println("TEST_FAIL: test_integration_time: short read failed");
    pixels.clear();
    pixels.show();
    return;
  }

  tcs.setIntegrationTime(200.0f);
  delay(220);
  uint16_t y_long = 0;
  if (!readAverageY(&y_long)) {
    Serial.println("TEST_FAIL: test_integration_time: long read failed");
    pixels.clear();
    pixels.show();
    return;
  }

  pixels.clear();
  pixels.show();

  if (y_long <= y_short) {
    Serial.print("TEST_FAIL: test_integration_time: long<=short ");
    Serial.print(y_long);
    Serial.print(" <= ");
    Serial.println(y_short);
    return;
  }

  Serial.println("TEST_PASS: test_integration_time");
}

void loop() {
  delay(1000);
}
