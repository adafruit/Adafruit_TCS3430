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

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    delay(10);
  }

  Serial.println("TEST_START: test_int_read_clear");

  if (!tcs.begin()) {
    Serial.println("TEST_FAIL: test_int_read_clear: begin() failed");
    return;
  }

  pixels.begin();
  pixels.setBrightness(80);
  setAll(255, 255, 255);

  tcs.setIntegrationTime(50.0f);
  tcs.setALSGain(TCS3430_GAIN_16X);

  // Wait for a cycle to complete so AINT sets
  delay(200);

  // Phase 1: INT_READ_CLEAR off — AINT should persist across reads
  tcs.setInterruptClearOnRead(false);
  
  // Disable ALS so no new cycles interfere, then clear
  tcs.ALSEnable(false);
  tcs.clearALSInterrupt();
  // Re-enable and wait for one cycle
  tcs.ALSEnable(true);
  delay(200);

  bool read1 = tcs.isALSInterrupt();
  bool read2 = tcs.isALSInterrupt();
  bool read3 = tcs.isALSInterrupt();

  if (!read1 || !read2 || !read3) {
    Serial.print("TEST_FAIL: test_int_read_clear: AINT not persistent r1=");
    Serial.print(read1);
    Serial.print(" r2=");
    Serial.print(read2);
    Serial.print(" r3=");
    Serial.println(read3);
    setAll(0, 0, 0);
    return;
  }
  Serial.println("  Phase 1 OK: AINT persists across 3 reads");

  // Phase 2: INT_READ_CLEAR on — first read sees AINT, second should not
  tcs.setInterruptClearOnRead(true);

  // Stop ALS, clear, restart, wait for one cycle
  tcs.ALSEnable(false);
  tcs.clearALSInterrupt();
  tcs.ALSEnable(true);
  delay(200);

  bool first_read = tcs.isALSInterrupt();
  // Immediately read again — should be cleared by first read
  bool second_read = tcs.isALSInterrupt();

  if (!first_read) {
    Serial.println(
        "TEST_FAIL: test_int_read_clear: AINT not set on first read");
    setAll(0, 0, 0);
    return;
  }
  if (second_read) {
    Serial.println(
        "TEST_FAIL: test_int_read_clear: AINT not auto-cleared on read");
    setAll(0, 0, 0);
    return;
  }
  Serial.println("  Phase 2 OK: AINT auto-cleared on read");

  setAll(0, 0, 0);
  Serial.println("TEST_PASS: test_int_read_clear");
}

void loop() {
  delay(1000);
}
