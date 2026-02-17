/*!
 * @file basictest.ino
 *
 * Basic test sketch for TCS3430 XYZ Tristimulus Color Sensor.
 * Polls the ALS interrupt flag to know when new data is ready
 * instead of using a fixed delay.
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
  tcs.setIntegrationTime(100.0);    // 2.78ms to 711ms

  // Enable ALS interrupt so we can poll AINT for data ready
  tcs.enableALSInt(true);
  tcs.setInterruptPersistence(TCS3430_PERS_EVERY);
  tcs.clearALSInterrupt();
}

void loop() {
  // Wait for new data
  if (tcs.isALSInterrupt()) {
	  uint16_t x, y, z, ir1;
	  if (!tcs.getChannels(&x, &y, &z, &ir1)) {
		Serial.println(F("Failed to read channels"));
	  } else {
		Serial.print(F("X: "));
		Serial.print(x);
		Serial.print(F("  Y: "));
		Serial.print(y);
		Serial.print(F("  Z: "));
		Serial.print(z);
		Serial.print(F("  IR1: "));
		Serial.println(ir1);
	  }

	  tcs.clearALSInterrupt();
  }
  
  // Do something else!
  delay(10);
}
