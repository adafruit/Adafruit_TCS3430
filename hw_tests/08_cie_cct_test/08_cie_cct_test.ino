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
  delay(500);
}

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    delay(10);
  }

  Serial.println("TEST_START: test_cie_cct");

  if (!tcs.begin()) {
    Serial.println("TEST_FAIL: test_cie_cct: begin() failed");
    return;
  }

  tcs.setIntegrationTime(100.0f);
  tcs.setALSGain(TCS3430_GAIN_16X);

  pixels.begin();
  pixels.setBrightness(80);
  setAll(255, 255, 255);

  uint16_t x = 0, y = 0, z = 0, ir1 = 0;
  if (!tcs.getChannels(&x, &y, &z, &ir1)) {
    Serial.println("TEST_FAIL: test_cie_cct: getChannels failed");
    setAll(0, 0, 0);
    return;
  }
  Serial.print("Raw channels: X=");
  Serial.print(x);
  Serial.print(" Y=");
  Serial.print(y);
  Serial.print(" Z=");
  Serial.print(z);
  Serial.print(" IR1=");
  Serial.println(ir1);

  float cie_x = 0.0f, cie_y = 0.0f;
  if (!tcs.getCIE(&cie_x, &cie_y)) {
    Serial.println("TEST_FAIL: test_cie_cct: getCIE failed");
    setAll(0, 0, 0);
    return;
  }

  float cct = tcs.getCCT();

  Serial.print("CIE x,y: ");
  Serial.print(cie_x, 4);
  Serial.print(", ");
  Serial.println(cie_y, 4);
  Serial.print("CCT: ");
  Serial.println(cct, 1);

  setAll(0, 0, 0);

  if (cie_x <= 0.0f || cie_x >= 1.0f || cie_y <= 0.0f || cie_y >= 1.0f) {
    Serial.print("TEST_FAIL: test_cie_cct: CIE out of range x=");
    Serial.print(cie_x, 4);
    Serial.print(" y=");
    Serial.println(cie_y, 4);
    return;
  }

  if (cct < 2000.0f || cct > 10000.0f) {
    Serial.print("TEST_FAIL: test_cie_cct: CCT out of range ");
    Serial.println(cct, 1);
    return;
  }

  Serial.println("TEST_PASS: test_cie_cct");
}

void loop() {
  delay(1000);
}
