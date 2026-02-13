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
  Serial.println("TEST_START: 02_gain_test");

  if (!tcs.begin()) {
    Serial.println("TEST_FAIL: 02_gain_test: begin() failed");
    return;
  }

  pixels.begin();
  tcs.setIntegrationTime(50.0f);
  tcs.setALSGain(TCS3430_GAIN_1X);

  // Auto-calibrate: find brightness where 1X reads 50-200
  uint8_t brightness = 5;
  uint16_t y1x = 0;
  for (uint8_t attempt = 0; attempt < 20; attempt++) {
    pixels.setBrightness(brightness);
    setAll(255, 255, 255);
    delay(200);
    if (!readAverageY(&y1x)) {
      Serial.println("TEST_FAIL: 02_gain_test: calibration read failed");
      setAll(0, 0, 0);
      return;
    }
    Serial.print("  cal: bright=");
    Serial.print(brightness);
    Serial.print(" Y@1X=");
    Serial.println(y1x);
    if (y1x >= 50 && y1x <= 200)
      break;
    if (y1x < 50) {
      brightness = (brightness < 200) ? brightness + 10 : 255;
    } else {
      brightness = (brightness > 10) ? brightness - 5 : 1;
    }
    if (attempt == 19) {
      Serial.println("  cal: using best effort");
    }
  }

  // Now step through gains
  tcs3430_gain_t gains[] = {TCS3430_GAIN_1X, TCS3430_GAIN_4X,
                            TCS3430_GAIN_16X, TCS3430_GAIN_64X,
                            TCS3430_GAIN_128X};
  uint16_t prev = 0;
  bool first = true;
  for (uint8_t i = 0; i < 5; i++) {
    tcs.setALSGain(gains[i]);
    delay(150);
    uint16_t y = 0;
    if (!readAverageY(&y)) {
      Serial.println("TEST_FAIL: 02_gain_test: read failed");
      setAll(0, 0, 0);
      return;
    }
    Serial.print("  gain_step=");
    Serial.print(i);
    Serial.print(" Y=");
    Serial.println(y);
    if (!first && prev < 60000 && y <= prev) {
      Serial.print("TEST_FAIL: 02_gain_test: not increasing prev=");
      Serial.print(prev);
      Serial.print(" curr=");
      Serial.println(y);
      setAll(0, 0, 0);
      return;
    }
    prev = y;
    first = false;
  }

  setAll(0, 0, 0);
  Serial.println("TEST_PASS: 02_gain_test");
}

void loop() { delay(1000); }
