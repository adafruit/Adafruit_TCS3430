/*
 * TCS3430 Color Display Demo for ESP32-S2 TFT Feather
 *
 * Displays live XYZ + IR1 channel values and an approximate color swatch
 * on the built-in 1.14" TFT (240x135).
 *
 * Written by Limor 'ladyada' Fried with assistance from Claude Code for
 * Adafruit Industries. MIT license, check license.txt for more information
 */

#include <Adafruit_ST7789.h>
#include <Adafruit_TCS3430.h>
#include <Fonts/FreeSans9pt7b.h>
#include <Fonts/FreeSansBold12pt7b.h>
#include <Fonts/FreeSansBold18pt7b.h>

Adafruit_TCS3430 tcs;
Adafruit_ST7789 display = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);
GFXcanvas16 canvas(240, 135);

const uint16_t SCREEN_W = 240;
const uint16_t SCREEN_H = 135;

// Convert CIE XYZ to approximate RGB565 for the color swatch
static uint16_t xyzToRGB565(float X, float Y, float Z) {
  // sRGB D65 matrix
  float r = 3.2406f * X - 1.5372f * Y - 0.4986f * Z;
  float g = -0.9689f * X + 1.8758f * Y + 0.0415f * Z;
  float b = 0.0557f * X - 0.2040f * Y + 1.0570f * Z;

  if (r < 0)
    r = 0;
  if (g < 0)
    g = 0;
  if (b < 0)
    b = 0;

  // Normalize to brightest channel
  float maxC = r;
  if (g > maxC)
    maxC = g;
  if (b > maxC)
    maxC = b;
  if (maxC > 0) {
    r /= maxC;
    g /= maxC;
    b /= maxC;
  }

  // Gamma approximation
  r = sqrtf(r);
  g = sqrtf(g);
  b = sqrtf(b);

  return ((uint8_t)(r * 255) & 0xF8) << 8 | ((uint8_t)(g * 255) & 0xFC) << 3 |
         (uint8_t)(b * 255) >> 3;
}

void setup() {
  Serial.begin(115200);
  delay(100);

  pinMode(TFT_I2C_POWER, OUTPUT);
  digitalWrite(TFT_I2C_POWER, HIGH);
  delay(100);

  display.init(135, 240);
  display.setRotation(3);

  canvas.setFont(&FreeSans9pt7b);
  canvas.setTextColor(ST77XX_WHITE);

  if (!tcs.begin()) {
    canvas.fillScreen(ST77XX_BLACK);
    canvas.setCursor(0, 40);
    canvas.setTextColor(ST77XX_RED);
    canvas.println(F(" TCS3430 not found!"));
    display.drawRGBBitmap(0, 0, canvas.getBuffer(), SCREEN_W, SCREEN_H);
    while (1)
      delay(10);
  }

  tcs.setALSGain(TCS3430_GAIN_64X);
  tcs.setIntegrationTime(100.0);

  pinMode(TFT_BACKLITE, OUTPUT);
  digitalWrite(TFT_BACKLITE, HIGH);
}

void loop() {
  uint16_t x, y, z, ir1;
  if (!tcs.getChannels(&x, &y, &z, &ir1)) {
    delay(500);
    return;
  }

  // Compute color swatch from XYZ
  float fx = (float)x / 65535.0;
  float fy = (float)y / 65535.0;
  float fz = (float)z / 65535.0;
  uint16_t swatchColor = xyzToRGB565(fx, fy, fz);

  canvas.fillScreen(ST77XX_BLACK);

  // Title bar
  canvas.setFont(&FreeSansBold12pt7b);
  canvas.setTextColor(ST77XX_WHITE);
  int16_t bx, by;
  uint16_t bw, bh;
  canvas.getTextBounds("Adafruit TCS3430", 0, 0, &bx, &by, &bw, &bh);
  canvas.setCursor((SCREEN_W - bw) / 2, 18);
  canvas.print(F("Adafruit TCS3430"));

  // Color swatch — right side, centered vertically below title
  const uint16_t SWATCH_SIZE = 65;
  const uint16_t SWATCH_X = SCREEN_W - SWATCH_SIZE - 26;
  const uint16_t SWATCH_Y = 45;
  canvas.drawRect(SWATCH_X - 1, SWATCH_Y - 1, SWATCH_SIZE + 2, SWATCH_SIZE + 2,
                  ST77XX_WHITE);
  canvas.fillRect(SWATCH_X, SWATCH_Y, SWATCH_SIZE, SWATCH_SIZE, swatchColor);

  // Channel values — left side
  canvas.setFont(&FreeSansBold12pt7b);
  uint16_t rowY = 50;
  uint16_t rowStep = 27;

  // Align under "Adafruit" in the title
  int16_t labelX = (SCREEN_W - bw) / 2;

  canvas.setTextColor(ST77XX_RED);
  canvas.setCursor(labelX, rowY);
  canvas.print(F("X: "));
  canvas.print(x);

  canvas.setTextColor(ST77XX_GREEN);
  canvas.setCursor(labelX, rowY + rowStep);
  canvas.print(F("Y: "));
  canvas.print(y);

  canvas.setTextColor(0x001F); // blue
  canvas.setCursor(labelX, rowY + rowStep * 2);
  canvas.print(F("Z: "));
  canvas.print(z);

  canvas.setTextColor(0xC81F); // purple-ish for IR
  canvas.setCursor(labelX, rowY + rowStep * 3);
  canvas.print(F("IR: "));
  canvas.print(ir1);

  display.drawRGBBitmap(0, 0, canvas.getBuffer(), SCREEN_W, SCREEN_H);
  delay(200);
}
