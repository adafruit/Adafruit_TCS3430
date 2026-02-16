/*!\
 * @file colormatrix.ino
 *
 * Example applying a 3x4 color matrix to TCS3430 raw channels.
 *
 * IMPORTANT:
 * - The color matrix (CM) depends on the optical stack (diffuser, cover glass,
 *   housing, etc.).
 * - These example matrices are from ams AN000571 and may not match your
 *   hardware.
 * - For accurate results, derive your own CM per the app note procedure.
 * - Low IR matrix is intended for LED/CFL sources; High IR is for
 *   incandescent/halogen.
 *
 * Low IR matrix (used below):
 * X' = -0.28837*X + 0.58484*Y + 1.55207*Z + -1.21521*IR
 * Y' = -0.30518*X + 0.60817*Y + 1.62203*Z + -1.25651*IR
 * Z' = -0.23132*X + 0.46517*Y + 1.22896*Z + -0.95905*IR
 *
 * High IR matrix (comment only):
 * X' =  0.582690*X + -0.183675*Y + -1.583206*Z + 0.082557*IR
 * Y' =  0.529610*X + -0.178553*Y + -1.416517*Z + 0.076360*IR
 * Z' =  0.188025*X + -0.057204*Y + -0.506941*Z + 0.025853*IR
 *
 * Prints raw channels and computed CIE x,y, lux, and CCT.
 *
 * Limor 'ladyada' Fried with assistance from Claude Code
 * MIT License
 */

#include "Adafruit_TCS3430.h"

Adafruit_TCS3430 tcs = Adafruit_TCS3430();

static const float kColorMatrix[3][4] = {
    {-0.28837, 0.58484, 1.55207, -1.21521},
    {-0.30518, 0.60817, 1.62203, -1.25651},
    {-0.23132, 0.46517, 1.22896, -0.95905},
};

float gainToValue(tcs3430_gain_t gain) {
  switch (gain) {
    case TCS3430_GAIN_1X:
      return 1.0;
    case TCS3430_GAIN_4X:
      return 4.0;
    case TCS3430_GAIN_16X:
      return 16.0;
    case TCS3430_GAIN_64X:
      return 66.0;
    case TCS3430_GAIN_128X:
      return 137.0;
    default:
      return 1.0;
  }
}

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    delay(10);
  }

  Serial.println(F("TCS3430 Color Matrix Example"));

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
  // tcs.setWaitTime(50.0);        // optional wait between cycles
  // tcs.setWaitLong(true);         // 12x wait multiplier
}

void loop() {
  uint16_t x = 0;
  uint16_t y = 0;
  uint16_t z = 0;
  uint16_t ir1 = 0;

  if (!tcs.getChannels(&x, &y, &z, &ir1)) {
    Serial.println(F("Failed to read channels"));
    delay(1000);
    return;
  }

  float cie_x = 0.0;
  float cie_y = 0.0;
  float cie_X = kColorMatrix[0][0] * x + kColorMatrix[0][1] * y +
                kColorMatrix[0][2] * z + kColorMatrix[0][3] * ir1;
  float cie_Y = kColorMatrix[1][0] * x + kColorMatrix[1][1] * y +
                kColorMatrix[1][2] * z + kColorMatrix[1][3] * ir1;
  float cie_Z = kColorMatrix[2][0] * x + kColorMatrix[2][1] * y +
                kColorMatrix[2][2] * z + kColorMatrix[2][3] * ir1;

  float sum = cie_X + cie_Y + cie_Z;
  if (sum > 0.0) {
    cie_x = cie_X / sum;
    cie_y = cie_Y / sum;
  }

  float cct = 0.0;
  if (sum > 0.0 && (0.1858 - cie_y) != 0.0) {
    float n = (cie_x - 0.3320) / (0.1858 - cie_y);
    cct = (449.0 * n * n * n) + (3525.0 * n * n) + (6823.3 * n) + 5520.33;
  }

  float gain = gainToValue(tcs.getALSGain());
  float integration_ms = tcs.getIntegrationTime();
  float lux = 0.0;
  if (integration_ms > 0.0) {
    lux = cie_Y * (16.0 / gain) * (100.0 / integration_ms);
  }

  Serial.print(F("X: "));
  Serial.print(x);
  Serial.print(F("  Y: "));
  Serial.print(y);
  Serial.print(F("  Z: "));
  Serial.print(z);
  Serial.print(F("  IR1: "));
  Serial.println(ir1);

  Serial.print(F("  CIE x: "));
  Serial.print(cie_x, 4);
  Serial.print(F("  y: "));
  Serial.print(cie_y, 4);
  Serial.print(F("  Lux: "));
  Serial.print(lux, 1);
  Serial.print(F("  CCT: "));
  Serial.print(cct, 0);
  Serial.println(F(" K"));

  delay(1000);
}
