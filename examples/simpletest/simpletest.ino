/*!\
 * @file simpletest.ino
 *
 * Simple test sketch for TCS3430 Color and ALS Sensor
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

  Serial.println(F("TCS3430 Simple Test"));

  if (!tcs.begin()) {
    Serial.println(F("Failed to find TCS3430 chip"));
    while (1) {
      delay(10);
    }
  }

  Serial.println(F("TCS3430 found!"));
}

void loop() {
  uint16_t x, y, z, ir1;
  if (tcs.getChannels(&x, &y, &z, &ir1)) {
    Serial.print(F("X: "));
    Serial.print(x);
    Serial.print(F(", Y: "));
    Serial.print(y);
    Serial.print(F(", Z: "));
    Serial.print(z);
    Serial.print(F(", IR1: "));
    Serial.println(ir1);
  } else {
    Serial.println(F("Failed to read channels"));
  }

  delay(1000);
}
