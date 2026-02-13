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

// Measure cycle-to-cycle period using INT_READ_CLEAR
// Returns average of N consecutive cycle periods in microseconds
static uint32_t measureCyclePeriod(uint8_t cycles, uint32_t timeout_us) {
  tcs.setInterruptClearOnRead(true);

  // Wait for first AINT (and auto-clear it by reading)
  uint32_t t = micros();
  while ((micros() - t) < timeout_us) {
    if (tcs.isALSInterrupt()) break;
    delayMicroseconds(100);
  }

  // Now measure N consecutive cycles
  uint32_t total = 0;
  for (uint8_t i = 0; i < cycles; i++) {
    uint32_t start = micros();
    while ((micros() - start) < timeout_us) {
      if (tcs.isALSInterrupt()) {
        total += (micros() - start);
        break;
      }
      delayMicroseconds(100);
    }
  }
  return total / cycles;
}

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    delay(10);
  }

  Serial.println("TEST_START: test_wait");

  if (!tcs.begin()) {
    Serial.println("TEST_FAIL: test_wait: begin() failed");
    return;
  }

  pixels.begin();
  pixels.setBrightness(80);
  setAll(255, 255, 255);

  tcs.setIntegrationTime(50.0f);
  tcs.setALSGain(TCS3430_GAIN_16X);

  // Measure without wait (average of 4 cycles)
  tcs.waitEnable(false);
  uint32_t no_wait = measureCyclePeriod(4, 1500000);

  // Measure with wait — 100ms wait time (average of 4 cycles)
  tcs.waitEnable(true);
  tcs.setWaitTime(100.0f);
  uint32_t with_wait = measureCyclePeriod(4, 1500000);

  Serial.print("No-wait avg us: ");
  Serial.println(no_wait);
  Serial.print("With-wait avg us: ");
  Serial.println(with_wait);

  // With-wait should be at least 50ms longer
  if (with_wait <= (no_wait + 50000)) {
    Serial.println("TEST_FAIL: test_wait: wait did not extend cycle");
    setAll(0, 0, 0);
    return;
  }

  setAll(0, 0, 0);
  Serial.println("TEST_PASS: test_wait");
}

void loop() {
  delay(1000);
}
