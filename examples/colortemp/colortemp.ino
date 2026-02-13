/*!\
 * @file colortemp.ino
 *
 * Color temperature example for TCS3430 Color and ALS Sensor
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

  Serial.println(F("TCS3430 Color Temperature Example"));

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
    float cie_x = 0.0f;
    float cie_y = 0.0f;
    if (tcs.getCIE(&cie_x, &cie_y)) {
      Serial.print(F("CIE x: "));
      Serial.print(cie_x, 4);
      Serial.print(F(", CIE y: "));
      Serial.print(cie_y, 4);
      Serial.print(F(", Lux: "));
      Serial.print(tcs.getLux(), 2);
      Serial.print(F(", CCT: "));
      Serial.print(tcs.getCCT(), 1);
      Serial.println(F(" K"));
    } else {
      Serial.println(F("Failed to compute CIE"));
    }
  } else {
    Serial.println(F("Failed to read channels"));
  }

  delay(1000);
}
