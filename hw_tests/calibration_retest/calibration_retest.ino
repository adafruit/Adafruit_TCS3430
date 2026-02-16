/*!
 * @file calibration_retest.ino
 * @brief Side-by-side TCS3430 vs OPT4048 comparison across NeoPixel conditions.
 * Prints raw TCS channels, TCS CIE/Lux/CCT, and OPT4048 CIE/Lux for comparison.
 */

#include <Adafruit_NeoPixel.h>
#include "Adafruit_OPT4048.h"
#include "Adafruit_TCS3430.h"

#define PIXEL_PIN 6
#define PIXEL_COUNT 16

Adafruit_TCS3430 tcs;
Adafruit_OPT4048 opt;
Adafruit_NeoPixel pixels(PIXEL_COUNT, PIXEL_PIN, NEO_GRB + NEO_KHZ800);

static void setAll(uint8_t r, uint8_t g, uint8_t b, uint8_t bright) {
  pixels.setBrightness(bright);
  for (uint16_t i = 0; i < PIXEL_COUNT; i++)
    pixels.setPixelColor(i, pixels.Color(r, g, b));
  pixels.show();
  delay(600); // settle time
}

struct Condition {
  const char *name;
  uint8_t r, g, b, bright;
};

static const Condition conditions[] = {
    {"dark", 0, 0, 0, 0},
    {"white_lo", 255, 255, 255, 20},
    {"white_med", 255, 255, 255, 80},
    {"white_hi", 255, 255, 255, 200},
    {"red_med", 255, 0, 0, 80},
    {"red_hi", 255, 0, 0, 200},
    {"green_med", 0, 255, 0, 80},
    {"green_hi", 0, 255, 0, 200},
    {"blue_med", 0, 0, 255, 80},
    {"blue_hi", 0, 0, 255, 200},
    {"warm", 255, 180, 100, 80},
    {"cool", 200, 220, 255, 80},
    {"purple", 180, 0, 255, 80},
    {"cyan", 0, 255, 200, 80},
    {"yellow", 255, 255, 0, 80},
    {"orange", 255, 120, 0, 80},
};

#define NUM_CONDITIONS (sizeof(conditions) / sizeof(conditions[0]))

void setup() {
  Serial.begin(115200);
  while (!Serial)
    delay(10);

  Serial.println("=== TCS3430 vs OPT4048 Calibration Retest ===");

  pixels.begin();

  if (!tcs.begin()) {
    Serial.println("ERROR: TCS3430 not found!");
    while (1) delay(100);
  }
  tcs.setIntegrationTime(100.0f);
  tcs.setALSGain(TCS3430_GAIN_16X);

  if (!opt.begin()) {
    Serial.println("ERROR: OPT4048 not found!");
    while (1) delay(100);
  }
  opt.setRange(OPT4048_RANGE_AUTO);
  opt.setConversionTime(OPT4048_CONVERSION_TIME_100MS);
  opt.setMode(OPT4048_MODE_CONTINUOUS);

  delay(500);

  // Header
  Serial.println(
      "condition,tcs_X,tcs_Y,tcs_Z,tcs_IR1,tcs_CIEx,tcs_CIEy,tcs_Lux,tcs_"
      "CCT,opt_CIEx,opt_CIEy,opt_Lux");

  for (uint8_t i = 0; i < NUM_CONDITIONS; i++) {
    const Condition &c = conditions[i];
    setAll(c.r, c.g, c.b, c.bright);

    // Read TCS3430
    uint16_t tx, ty, tz, tir1;
    tcs.getChannels(&tx, &ty, &tz, &tir1);
    float tCIEx = 0, tCIEy = 0;
    tcs.getCIE(&tCIEx, &tCIEy);
    float tLux = tcs.getLux();
    float tCCT = tcs.getCCT();

    // Read OPT4048
    double oCIEx = 0, oCIEy = 0, oLux = 0;
    opt.getCIE(&oCIEx, &oCIEy, &oLux);

    // Print CSV
    Serial.print(c.name);
    Serial.print(",");
    Serial.print(tx);
    Serial.print(",");
    Serial.print(ty);
    Serial.print(",");
    Serial.print(tz);
    Serial.print(",");
    Serial.print(tir1);
    Serial.print(",");
    Serial.print(tCIEx, 4);
    Serial.print(",");
    Serial.print(tCIEy, 4);
    Serial.print(",");
    Serial.print(tLux, 1);
    Serial.print(",");
    Serial.print(tCCT, 0);
    Serial.print(",");
    Serial.print(oCIEx, 4);
    Serial.print(",");
    Serial.print(oCIEy, 4);
    Serial.print(",");
    Serial.println(oLux, 1);
  }

  // Turn off LEDs
  setAll(0, 0, 0, 0);
  Serial.println("=== DONE ===");
}

void loop() {
  delay(1000);
}
