/*!
 * @file fulltest.ino
 *
 * Full test sketch for TCS3430 Color and ALS Sensor
 *
 * Limor 'ladyada' Fried with assistance from Claude Code
 * MIT License
 */

#include "Adafruit_TCS3430.h"

Adafruit_TCS3430 tcs = Adafruit_TCS3430();

void setup() {
  Serial.begin(115200);

  while (!Serial)
    delay(10);

  Serial.println(F("TCS3430 Color and ALS Sensor Test"));

  if (!tcs.begin()) {
    Serial.println(F("Failed to find TCS3430 chip"));
    while (1)
      delay(10);
  }

  Serial.println(F("TCS3430 found!"));

  Serial.print(F("Power on: "));
  Serial.println(tcs.isPoweredOn());
  Serial.print(F("ALS enabled: "));
  Serial.println(tcs.isALSEnabled());

  tcs.waitEnable(false);
  Serial.print(F("Wait enabled: "));
  Serial.println(tcs.isWaitEnabled());

  tcs.setIntegrationCycles(64);

  Serial.print(F("Integration cycles: "));
  Serial.println(tcs.getIntegrationCycles());
  Serial.print(F("Integration time: "));
  Serial.print(tcs.getIntegrationTime());
  Serial.println(F(" ms"));

  tcs.setWaitTime(50.0);
  Serial.print(F("Wait cycles: "));
  Serial.println(tcs.getWaitCycles());
  Serial.print(F("Wait time: "));
  Serial.print(tcs.getWaitTime());
  Serial.println(F(" ms"));

  tcs.setALSThresholdLow(100);
  tcs.setALSThresholdHigh(5000);
  Serial.print(F("ALS threshold low: "));
  Serial.println(tcs.getALSThresholdLow());
  Serial.print(F("ALS threshold high: "));
  Serial.println(tcs.getALSThresholdHigh());

  tcs.setInterruptPersistence(TCS3430_PERS_5);
  Serial.print(F("Interrupt persistence: "));
  tcs3430_pers_t pers = tcs.getInterruptPersistence();
  switch (pers) {
    case TCS3430_PERS_EVERY:
      Serial.println(F("Every ALS cycle"));
      break;
    case TCS3430_PERS_1:
      Serial.println(F("1 consecutive"));
      break;
    case TCS3430_PERS_2:
      Serial.println(F("2 consecutive"));
      break;
    case TCS3430_PERS_3:
      Serial.println(F("3 consecutive"));
      break;
    case TCS3430_PERS_5:
      Serial.println(F("5 consecutive"));
      break;
    case TCS3430_PERS_10:
      Serial.println(F("10 consecutive"));
      break;
    case TCS3430_PERS_15:
      Serial.println(F("15 consecutive"));
      break;
    case TCS3430_PERS_20:
      Serial.println(F("20 consecutive"));
      break;
    case TCS3430_PERS_25:
      Serial.println(F("25 consecutive"));
      break;
    case TCS3430_PERS_30:
      Serial.println(F("30 consecutive"));
      break;
    case TCS3430_PERS_35:
      Serial.println(F("35 consecutive"));
      break;
    case TCS3430_PERS_40:
      Serial.println(F("40 consecutive"));
      break;
    case TCS3430_PERS_45:
      Serial.println(F("45 consecutive"));
      break;
    case TCS3430_PERS_50:
      Serial.println(F("50 consecutive"));
      break;
    case TCS3430_PERS_55:
      Serial.println(F("55 consecutive"));
      break;
    case TCS3430_PERS_60:
      Serial.println(F("60 consecutive"));
      break;
    default:
      Serial.println(F("Unknown"));
      break;
  }

  tcs.setWaitLong(true);
  Serial.print(F("Wait long: "));
  Serial.println(tcs.getWaitLong() ? F("enabled (12x multiplier)")
                                   : F("disabled"));

  tcs.setALSMUX_IR2(false);
  Serial.print(F("ALS MUX: "));
  Serial.println(tcs.getALSMUX_IR2() ? F("IR2 channel") : F("X channel"));

  tcs.setALSGain(TCS3430_GAIN_16X);
  Serial.print(F("ALS gain: "));
  tcs3430_gain_t gain = tcs.getALSGain();
  switch (gain) {
    case TCS3430_GAIN_1X:
      Serial.println(F("1x"));
      break;
    case TCS3430_GAIN_4X:
      Serial.println(F("4x"));
      break;
    case TCS3430_GAIN_16X:
      Serial.println(F("16x"));
      break;
    case TCS3430_GAIN_64X:
      Serial.println(F("64x"));
      break;
    case TCS3430_GAIN_128X:
      Serial.println(F("128x"));
      break;
    default:
      Serial.println(F("Unknown"));
      break;
  }

  tcs.setInterruptClearOnRead(false);
  Serial.print(F("Interrupt clear on read: "));
  Serial.println(tcs.getInterruptClearOnRead() ? F("enabled") : F("disabled"));

  tcs.setSleepAfterInterrupt(false);
  Serial.print(F("Sleep after interrupt: "));
  Serial.println(tcs.getSleepAfterInterrupt() ? F("enabled") : F("disabled"));

  tcs.setAutoZeroMode(false);
  Serial.print(F("Auto-zero mode: "));
  Serial.println(tcs.getAutoZeroMode() ? F("enabled") : F("disabled"));

  tcs.setRunAutoZeroEveryN(7);
  Serial.print(F("Run auto-zero every N: "));
  Serial.println(tcs.getRunAutoZeroEveryN());

  tcs.enableALSInt(true);
  tcs.enableSaturationInt(true);
  Serial.println(F("ALS and saturation interrupts enabled"));
}

void loop() {
  if (tcs.isALSSaturated()) {
    Serial.println(F("ALS saturated - clearing"));
    tcs.clearALSSaturated();
  }

  if (tcs.isALSInterrupt()) {
    Serial.println(F("ALS interrupt - clearing"));
    tcs.clearALSInterrupt();
  }

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