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

static bool waitForReading(uint32_t* elapsed_us, uint32_t timeout_us) {
  uint32_t start = micros();
  while ((micros() - start) < timeout_us) {
    uint16_t x, y, z, ir1;
    if (!tcs.getChannels(&x, &y, &z, &ir1)) {
      return false;
    }
    if (x > 0 || y > 0 || z > 0 || ir1 > 0) {
      *elapsed_us = micros() - start;
      return true;
    }
    delay(5);
  }
  return false;
}

static bool powerCycle() {
  if (!tcs.powerOn(false)) {
    return false;
  }
  delay(50);
  if (!tcs.powerOn(true)) {
    return false;
  }
  if (!tcs.ALSEnable(true)) {
    return false;
  }
  delay(50);
  return true;
}

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    delay(10);
  }

  Serial.println("TEST_START: test_autozero");

  if (!tcs.begin()) {
    Serial.println("TEST_FAIL: test_autozero: begin() failed");
    return;
  }

  pixels.begin();
  pixels.setBrightness(40);
  setAll(20, 20, 20);

  tcs.setIntegrationTime(100.0f);
  tcs.setALSGain(TCS3430_GAIN_16X);

  if (!tcs.setAutoZeroMode(true) || !tcs.setRunAutoZeroEveryN(0x7F)) {
    Serial.println("TEST_FAIL: test_autozero: enable failed");
    return;
  }

  if (!powerCycle()) {
    Serial.println("TEST_FAIL: test_autozero: power cycle failed");
    return;
  }

  uint32_t az_time = 0;
  if (!waitForReading(&az_time, 2000000)) {
    Serial.println("TEST_FAIL: test_autozero: autozero read failed");
    return;
  }

  if (!tcs.setAutoZeroMode(false) || !tcs.setRunAutoZeroEveryN(0)) {
    Serial.println("TEST_FAIL: test_autozero: disable failed");
    return;
  }

  if (!powerCycle()) {
    Serial.println("TEST_FAIL: test_autozero: power cycle failed (no az)");
    return;
  }

  uint32_t no_az_time = 0;
  if (!waitForReading(&no_az_time, 2000000)) {
    Serial.println("TEST_FAIL: test_autozero: no-az read failed");
    return;
  }

  Serial.print("Autozero first-read us: ");
  Serial.println(az_time);
  Serial.print("No-auto-zero first-read us: ");
  Serial.println(no_az_time);

  if (abs((int32_t)az_time - (int32_t)no_az_time) < 1000) {
    Serial.println("NOTE: test_autozero timing delta subtle; readings valid");
  }

  if (tcs.getAutoZeroMode() || tcs.getRunAutoZeroEveryN() != 0) {
    Serial.println("TEST_FAIL: test_autozero: config readback mismatch");
    return;
  }

  Serial.println("TEST_PASS: test_autozero");
}

void loop() {
  delay(1000);
}
