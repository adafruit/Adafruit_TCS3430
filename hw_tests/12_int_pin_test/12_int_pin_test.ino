// 12_int_pin_test: Verify physical INT pin toggles with ALS interrupts
// Breakout has open-drain inverter on INT: active-HIGH at MCU, needs pullup

#include <Adafruit_NeoPixel.h>

#include "Adafruit_TCS3430.h"

#define PIXEL_PIN 6
#define PIXEL_COUNT 16
#define INT_PIN 2
#define INT_ACTIVE HIGH
#define INT_IDLE LOW

Adafruit_TCS3430 tcs = Adafruit_TCS3430();
Adafruit_NeoPixel pixels(PIXEL_COUNT, PIXEL_PIN, NEO_GRB + NEO_KHZ800);

static void setAll(uint8_t r, uint8_t g, uint8_t b) {
  for (uint16_t i = 0; i < PIXEL_COUNT; i++)
    pixels.setPixelColor(i, pixels.Color(r, g, b));
  pixels.show();
  delay(200);
}

static bool waitForPin(int expected, uint32_t timeout_ms) {
  uint32_t start = millis();
  while ((millis() - start) < timeout_ms) {
    if (digitalRead(INT_PIN) == expected)
      return true;
    delay(1);
  }
  return false;
}

void setup() {
  Serial.begin(115200);
  while (!Serial) delay(10);

  Serial.println("TEST_START: 12_int_pin_test");

  if (!tcs.begin()) {
    Serial.println("TEST_FAIL: 12_int_pin_test: begin() failed");
    return;
  }

  pixels.begin();
  pixels.setBrightness(80);
  setAll(0, 0, 0);
  pinMode(INT_PIN, INPUT_PULLUP);

  // Setup: fast integration, guaranteed-to-fire thresholds
  tcs.setIntegrationTime(2.78f);
  tcs.setALSGain(TCS3430_GAIN_4X);
  tcs.setInterruptPersistence(TCS3430_PERS_EVERY);
  tcs.setInterruptClearOnRead(false);

  // --- Step 1: Clear state, verify idle ---
  tcs.ALSEnable(false);
  tcs.enableALSInt(false);
  tcs.clearALSInterrupt();
  delay(50);

  Serial.print("Idle pin: ");
  Serial.println(digitalRead(INT_PIN) == INT_IDLE ? "OK (idle)" : "UNEXPECTED");
  if (digitalRead(INT_PIN) != INT_IDLE) {
    Serial.println("TEST_FAIL: 12_int_pin_test: pin not idle at start");
    return;
  }

  // --- Step 2: Enable ALS + AIEN with thresholds 0/0 (always fires) ---
  tcs.setALSThresholdLow(0);
  tcs.setALSThresholdHigh(0);
  tcs.clearALSInterrupt();
  tcs.enableALSInt(true);
  tcs.ALSEnable(true);

  if (!waitForPin(INT_ACTIVE, 500)) {
    Serial.println("TEST_FAIL: 12_int_pin_test: INT never went active");
    return;
  }
  Serial.println("INT went active: PASS");

  // --- Step 3: Clear interrupt, verify pin returns to idle ---
  tcs.ALSEnable(false);
  tcs.clearALSInterrupt();
  delay(10);

  if (digitalRead(INT_PIN) != INT_IDLE) {
    Serial.println("TEST_FAIL: 12_int_pin_test: INT did not return to idle after clear");
    return;
  }
  Serial.println("INT cleared to idle: PASS");

  // --- Step 4: Verify INT stays idle with AIEN disabled ---
  tcs.enableALSInt(false);
  tcs.clearALSInterrupt();
  tcs.ALSEnable(true);
  delay(100); // let several cycles run

  if (digitalRead(INT_PIN) != INT_IDLE) {
    Serial.println("TEST_FAIL: 12_int_pin_test: INT active with AIEN disabled");
    return;
  }
  Serial.println("INT stays idle with AIEN off: PASS");

  // --- Step 5: Toggle test - verify multiple assert/clear cycles ---
  tcs.ALSEnable(false);
  tcs.clearALSInterrupt();
  tcs.enableALSInt(true);
  tcs.ALSEnable(true);

  uint8_t toggles = 0;
  for (uint8_t i = 0; i < 5; i++) {
    if (!waitForPin(INT_ACTIVE, 200))
      break;
    tcs.ALSEnable(false);
    tcs.clearALSInterrupt();
    if (digitalRead(INT_PIN) != INT_IDLE)
      break;
    tcs.ALSEnable(true);
    toggles++;
  }
  Serial.print("Toggle cycles: ");
  Serial.print(toggles);
  Serial.println("/5");

  if (toggles < 5) {
    Serial.println("TEST_FAIL: 12_int_pin_test: toggle cycles incomplete");
    return;
  }
  Serial.println("Toggle test: PASS");

  // Cleanup
  tcs.enableALSInt(false);
  tcs.ALSEnable(false);
  tcs.clearALSInterrupt();
  setAll(0, 0, 0);

  Serial.println("TEST_PASS: 12_int_pin_test");
}

void loop() { delay(1000); }
