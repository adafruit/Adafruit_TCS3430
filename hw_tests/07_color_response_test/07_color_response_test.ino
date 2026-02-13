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
  delay(300);
}

static bool readAvg(uint16_t *x, uint16_t *y, uint16_t *z) {
  const uint8_t samples = 4;
  uint32_t sx = 0, sy = 0, sz = 0;
  for (uint8_t i = 0; i < samples; i++) {
    uint16_t cx, cy, cz, cir;
    if (!tcs.getChannels(&cx, &cy, &cz, &cir))
      return false;
    sx += cx;
    sy += cy;
    sz += cz;
    delay(30);
  }
  *x = sx / samples;
  *y = sy / samples;
  *z = sz / samples;
  return true;
}

// Auto-gain: try gains from high to low until no channel exceeds 60000
static tcs3430_gain_t gains[] = {TCS3430_GAIN_64X, TCS3430_GAIN_16X,
                                 TCS3430_GAIN_4X, TCS3430_GAIN_1X};

static bool autoRead(uint16_t *x, uint16_t *y, uint16_t *z) {
  for (uint8_t g = 0; g < 4; g++) {
    tcs.setALSGain(gains[g]);
    delay(200);
    if (!readAvg(x, y, z))
      return false;
    if (*x < 60000 && *y < 60000 && *z < 60000)
      return true;
  }
  return true; // best effort at 1X
}

static bool dominant(uint16_t target, uint16_t other1, uint16_t other2) {
  if (target == 0)
    return false;
  if (target * 5 < other1 * 4)
    return false;
  if (target * 5 < other2 * 4)
    return false;
  return true;
}

void setup() {
  Serial.begin(115200);
  while (!Serial)
    delay(10);
  Serial.println("TEST_START: 07_color_response_test");

  if (!tcs.begin()) {
    Serial.println("TEST_FAIL: 07_color_response_test: begin() failed");
    return;
  }

  pixels.begin();
  pixels.setBrightness(80);
  tcs.setIntegrationTime(50.0f);

  uint16_t x, y, z;

  // Red: X should dominate
  setAll(255, 0, 0);
  if (!autoRead(&x, &y, &z)) {
    Serial.println("TEST_FAIL: 07_color_response_test: red read failed");
    goto done;
  }
  Serial.print("  RED  X=");
  Serial.print(x);
  Serial.print(" Y=");
  Serial.print(y);
  Serial.print(" Z=");
  Serial.println(z);
  if (!dominant(x, y, z)) {
    Serial.println("TEST_FAIL: 07_color_response_test: red not dominant");
    goto done;
  }

  // Green: Y should dominate
  setAll(0, 255, 0);
  if (!autoRead(&x, &y, &z)) {
    Serial.println("TEST_FAIL: 07_color_response_test: green read failed");
    goto done;
  }
  Serial.print("  GRN  X=");
  Serial.print(x);
  Serial.print(" Y=");
  Serial.print(y);
  Serial.print(" Z=");
  Serial.println(z);
  if (!dominant(y, x, z)) {
    Serial.println("TEST_FAIL: 07_color_response_test: green not dominant");
    goto done;
  }

  // Blue: Z should dominate
  setAll(0, 0, 255);
  if (!autoRead(&x, &y, &z)) {
    Serial.println("TEST_FAIL: 07_color_response_test: blue read failed");
    goto done;
  }
  Serial.print("  BLU  X=");
  Serial.print(x);
  Serial.print(" Y=");
  Serial.print(y);
  Serial.print(" Z=");
  Serial.println(z);
  if (!dominant(z, x, y)) {
    Serial.println("TEST_FAIL: 07_color_response_test: blue not dominant");
    goto done;
  }

  // White: all non-zero
  setAll(255, 255, 255);
  if (!autoRead(&x, &y, &z)) {
    Serial.println("TEST_FAIL: 07_color_response_test: white read failed");
    goto done;
  }
  Serial.print("  WHT  X=");
  Serial.print(x);
  Serial.print(" Y=");
  Serial.print(y);
  Serial.print(" Z=");
  Serial.println(z);
  if (x == 0 || y == 0 || z == 0) {
    Serial.println("TEST_FAIL: 07_color_response_test: white has zero");
    goto done;
  }

  Serial.println("TEST_PASS: 07_color_response_test");

done:
  setAll(0, 0, 0);
}

void loop() { delay(1000); }
