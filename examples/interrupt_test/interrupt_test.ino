/*!\
 * @file interrupt_test.ino
 *
 * Interrupt example for TCS3430 XYZ Tristimulus Color Sensor
 * Sets a high threshold — shine a flashlight on the sensor to trigger!
 * Connect INT to pin 2. The Adafruit breakout has an open-drain inverter
 * on INT, so active-HIGH at MCU. Needs INPUT_PULLUP.
 *
 * Limor 'ladyada' Fried with assistance from Claude Code
 * MIT License
 */

#include "Adafruit_TCS3430.h"

#define INT_PIN 2

Adafruit_TCS3430 tcs = Adafruit_TCS3430();

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    delay(10);
  }

  Serial.println(F("TCS3430 Interrupt Test"));
  Serial.println(F("Connect sensor INT to pin 2"));
  Serial.println();

  if (!tcs.begin()) {
    Serial.println(F("Failed to find TCS3430 chip"));
    while (1) {
      delay(10);
    }
  }

  Serial.println(F("TCS3430 found!"));
  pinMode(INT_PIN, INPUT_PULLUP);

  // Setup sensor
  tcs.setALSGain(TCS3430_GAIN_16X);
  tcs.setIntegrationTime(100.0f);
  tcs.setInterruptPersistence(TCS3430_PERS_3); // require 3 consecutive

  // Read current ambient to set threshold above it
  delay(200);
  uint16_t x, y, z, ir1;
  tcs.getChannels(&x, &y, &z, &ir1);

  // Thresholds compare against CH0 (Z channel)
  uint16_t threshold = z * 2; // 2x above current ambient
  if (threshold < z + 200) {
    threshold = z + 200;
  }
  if (threshold > 65000) {
    threshold = 65000;
  }

  Serial.print(F("Current ambient Z: "));
  Serial.println(z);
  Serial.print(F("Interrupt threshold (high): "));
  Serial.println(threshold);
  Serial.println();
  Serial.println(F("Shine a flashlight on the sensor to trigger!"));
  Serial.println(F("INT pin idle = LOW, active = HIGH (breakout inverter)"));
  Serial.println();

  // Window: low=0, high=threshold
  // Interrupt fires when Z exceeds threshold (outside window)
  tcs.setALSThresholdLow(0);
  tcs.setALSThresholdHigh(threshold);
  tcs.setInterruptClearOnRead(false);

  // Clean start
  tcs.enableALSInt(true);
  tcs.ALSEnable(false);
  tcs.clearALSInterrupt();
  tcs.ALSEnable(true);

  Serial.print(F("INT pin: "));
  Serial.println(digitalRead(INT_PIN) ? "HIGH" : "LOW");
}

void loop() {
  // Check INT pin state
  if (digitalRead(INT_PIN) == HIGH) {
    // Read channels to see what triggered it
    uint16_t x, y, z, ir1;
    tcs.getChannels(&x, &y, &z, &ir1);

    Serial.print(F("*** INTERRUPT! *** Z="));
    Serial.print(z);
    Serial.print(F("  Y="));
    Serial.print(y);
    Serial.print(F("  X="));
    Serial.print(x);
    Serial.print(F("  IR1="));
    Serial.println(ir1);

    // Clear and wait for pin to go idle before checking again
    tcs.ALSEnable(false);
    tcs.clearALSInterrupt();
    tcs.ALSEnable(true);

    // Wait for INT to settle back to idle
    delay(500);
    return;
  }

  // Periodic ambient reading every 2 seconds
  static uint32_t lastPrint = 0;
  if (millis() - lastPrint > 2000) {
    lastPrint = millis();

    uint16_t x, y, z, ir1;
    tcs.getChannels(&x, &y, &z, &ir1);
    Serial.print(F("Z="));
    Serial.print(z);
    Serial.print(F("  Y="));
    Serial.print(y);
    Serial.print(F("  (waiting...)  INT="));
    Serial.println(digitalRead(INT_PIN) ? "HIGH" : "LOW");
  }
}
