/*!\
 * @file basictest.ino
 *
 * Basic test sketch for TCS3430 XYZ Tristimulus Color Sensor
 * Reads raw channels
 *
 * Limor 'ladyada' Fried with assistance from Claude Code
 * MIT License
 */

#include "Adafruit_TCS3430.h"

Adafruit_TCS3430 tcs = Adafruit_TCS3430();

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    delay(10);
  }

  Serial.println(F("TCS3430 Basic Test"));

  if (!tcs.begin()) {
    Serial.println(F("Failed to find TCS3430 chip"));
    while (1) {
      delay(10);
    }
  }

  Serial.println(F("TCS3430 found!"));

  // --- Tweak these settings for your environment ---
  tcs.setALSGain(TCS3430_GAIN_64X); // 1X, 4X, 16X, 64X, or 128X
  tcs.setIntegrationTime(100.0f);   // 2.78ms to 711ms
  // tcs.setWaitTime(50.0f);        // optional wait between cycles
  // tcs.setWaitLong(true);         // 12x wait multiplier

  Serial.println(F("Settings:"));
  Serial.print(F("  Gain: 64X"));
  Serial.print(F("  Integration: 100ms"));
  Serial.println();
}

void loop() {
  uint16_t x, y, z, ir1;
  if (!tcs.getChannels(&x, &y, &z, &ir1)) {
    Serial.println(F("Failed to read channels"));
    delay(1000);
    return;
  }

  Serial.print(F("X: "));
  Serial.print(x);
  Serial.print(F("  Y: "));
  Serial.print(y);
  Serial.print(F("  Z: "));
  Serial.print(z);
  Serial.print(F("  IR1: "));
  Serial.println(ir1);

  delay(1000);
}
