#include <Adafruit_NeoPixel.h>

#include "Adafruit_TCS3430.h"

#define PIXEL_PIN 6
#define PIXEL_COUNT 16

Adafruit_TCS3430 tcs = Adafruit_TCS3430();
Adafruit_NeoPixel pixels(PIXEL_COUNT, PIXEL_PIN, NEO_GRB + NEO_KHZ800);

static bool readAverage(uint16_t* x, uint16_t* y, uint16_t* z, uint16_t* ir1) {
  const uint8_t samples = 5;
  uint32_t sx = 0, sy = 0, sz = 0, sir = 0;
  for (uint8_t i = 0; i < samples; i++) {
    uint16_t cx, cy, cz, cir;
    if (!tcs.getChannels(&cx, &cy, &cz, &cir)) {
      return false;
    }
    sx += cx;
    sy += cy;
    sz += cz;
    sir += cir;
    delay(50);
  }
  *x = sx / samples;
  *y = sy / samples;
  *z = sz / samples;
  *ir1 = sir / samples;
  return true;
}

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

  Serial.println("TEST_START: test_power");

  if (!tcs.begin()) {
    Serial.println("TEST_FAIL: test_power: begin() failed");
    return;
  }

  pixels.begin();
  pixels.setBrightness(80);

  if (!tcs.powerOn(false)) {
    Serial.println("TEST_FAIL: test_power: power off failed");
    return;
  }
  delay(50);

  uint16_t x = 0, y = 0, z = 0, ir1 = 0;
  bool read_ok = readAverage(&x, &y, &z, &ir1);
  if (read_ok && (x != 0 || y != 0 || z != 0 || ir1 != 0)) {
    Serial.print("TEST_FAIL: test_power: non-zero while off X=");
    Serial.print(x);
    Serial.print(" Y=");
    Serial.print(y);
    Serial.print(" Z=");
    Serial.print(z);
    Serial.print(" IR1=");
    Serial.println(ir1);
    return;
  }

  if (!tcs.powerOn(true) || !tcs.ALSEnable(true)) {
    Serial.println("TEST_FAIL: test_power: power on failed");
    return;
  }

  tcs.setIntegrationTime(100.0f);
  tcs.setALSGain(TCS3430_GAIN_16X);

  setAll(255, 255, 255);

  if (!readAverage(&x, &y, &z, &ir1)) {
    Serial.println("TEST_FAIL: test_power: read after power on failed");
    setAll(0, 0, 0);
    return;
  }

  setAll(0, 0, 0);

  if (x == 0 || y == 0 || z == 0 || ir1 == 0) {
    Serial.print("TEST_FAIL: test_power: zero after power on X=");
    Serial.print(x);
    Serial.print(" Y=");
    Serial.print(y);
    Serial.print(" Z=");
    Serial.print(z);
    Serial.print(" IR1=");
    Serial.println(ir1);
    return;
  }

  Serial.println("TEST_PASS: test_power");
}

void loop() {
  delay(1000);
}
