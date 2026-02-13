/*
 * TCS3430 Color Display Demo for ESP32-S2 TFT Feather
 *
 * Displays real-time XYZ channel bars, a color swatch approximating
 * the detected color, CIE x/y chromaticity, CCT, and lux on the
 * built-in 1.14" TFT (240x135).
 *
 * Written by Limor 'ladyada' Fried with assistance from Claude Code for
 * Adafruit Industries. MIT license, check license.txt for more information
 */

#include <Adafruit_ST7789.h>
#include <Adafruit_TCS3430.h>
#include <Fonts/FreeSans9pt7b.h>

Adafruit_TCS3430 tcs;
Adafruit_ST7789 display = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);
GFXcanvas16 canvas(240, 135);

// Convert CIE XYZ to approximate RGB565 for the color swatch
// Uses a simplified sRGB matrix + gamma
static uint16_t xyzToRGB565(float X, float Y, float Z) {
  // XYZ to linear sRGB (D65)
  float r = 3.2406f * X - 1.5372f * Y - 0.4986f * Z;
  float g = -0.9689f * X + 1.8758f * Y + 0.0415f * Z;
  float b = 0.0557f * X - 0.2040f * Y + 1.0570f * Z;

  // Clamp negatives
  if (r < 0)
    r = 0;
  if (g < 0)
    g = 0;
  if (b < 0)
    b = 0;

  // Normalize to max=1
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

  // Simple gamma (~2.2)
  r = sqrtf(r);
  g = sqrtf(g);
  b = sqrtf(b);

  uint8_t r8 = (uint8_t)(r * 255);
  uint8_t g8 = (uint8_t)(g * 255);
  uint8_t b8 = (uint8_t)(b * 255);

  return ((r8 & 0xF8) << 8) | ((g8 & 0xFC) << 3) | (b8 >> 3);
}

// Bar colors for X, Y, Z, IR1
const uint16_t barColors[4] = {
    0xF800, // X — red
    0x07E0, // Y — green
    0x001F, // Z — blue
    0x8010, // IR1 — purple
};
const char* barLabels[4] = {"X", "Y", "Z", "IR"};

// Layout constants
const uint16_t BAR_LEFT = 4;
const uint16_t BAR_TOP = 18;
const uint16_t BAR_BOTTOM = 100;
const uint16_t BAR_WIDTH = 22;
const uint16_t BAR_GAP = 6;
const uint16_t SWATCH_X = 120;
const uint16_t SWATCH_Y = 18;
const uint16_t SWATCH_W = 50;
const uint16_t SWATCH_H = 50;
const uint16_t TEXT_X = 120;
const uint16_t TEXT_Y = 78;

void setup() {
  Serial.begin(115200);
  delay(100);

  // Turn on TFT / I2C power
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
    canvas.println(" TCS3430 not found!");
    display.drawRGBBitmap(0, 0, canvas.getBuffer(), 240, 135);
    while (1)
      delay(10);
  }

  tcs.setALSGain(TCS3430_GAIN_64X);
  tcs.setIntegrationTime(100.0f);

  pinMode(TFT_BACKLITE, OUTPUT);
  digitalWrite(TFT_BACKLITE, HIGH);
}

void loop() {
  uint16_t x, y, z, ir1;
  if (!tcs.getChannels(&x, &y, &z, &ir1)) {
    delay(500);
    return;
  }

  uint16_t vals[4] = {x, y, z, ir1};

  // Find max for auto-scaling bars
  uint16_t maxVal = 1;
  for (uint8_t i = 0; i < 4; i++) {
    if (vals[i] > maxVal)
      maxVal = vals[i];
  }

  canvas.fillScreen(ST77XX_BLACK);

  // Title
  canvas.setFont(&FreeSans9pt7b);
  canvas.setTextColor(ST77XX_WHITE);
  canvas.setCursor(4, 14);
  canvas.print("TCS3430");

  // Draw 4 channel bars
  uint16_t barHeight = BAR_BOTTOM - BAR_TOP;
  for (uint8_t i = 0; i < 4; i++) {
    uint16_t h = (uint32_t)vals[i] * barHeight / maxVal;
    if (h < 1 && vals[i] > 0)
      h = 1;

    uint16_t bx = BAR_LEFT + i * (BAR_WIDTH + BAR_GAP);
    uint16_t by = BAR_BOTTOM - h;

    canvas.fillRect(bx, by, BAR_WIDTH, h, barColors[i]);

    // Label below bar
    canvas.setFont(NULL);
    canvas.setTextColor(barColors[i]);
    uint16_t lx = bx + (BAR_WIDTH - strlen(barLabels[i]) * 6) / 2;
    canvas.setCursor(lx, BAR_BOTTOM + 4);
    canvas.print(barLabels[i]);

    // Value on top
    canvas.setTextColor(ST77XX_WHITE);
    char vbuf[6];
    itoa(vals[i], vbuf, 10);
    lx = bx + (BAR_WIDTH - strlen(vbuf) * 6) / 2;
    uint16_t vy = by - 2;
    if (vy < BAR_TOP)
      vy = BAR_TOP;
    canvas.setCursor(lx, vy);
    canvas.print(vbuf);
  }

  // Color swatch from XYZ
  float fx = (float)x / 65535.0f;
  float fy = (float)y / 65535.0f;
  float fz = (float)z / 65535.0f;
  uint16_t swatchColor = xyzToRGB565(fx, fy, fz);
  canvas.fillRoundRect(SWATCH_X, SWATCH_Y, SWATCH_W, SWATCH_H, 4, swatchColor);
  canvas.drawRoundRect(SWATCH_X, SWATCH_Y, SWATCH_W, SWATCH_H, 4, ST77XX_WHITE);

  // CIE, CCT, Lux text
  canvas.setFont(NULL);
  canvas.setTextColor(ST77XX_WHITE);

  float cie_x = 0, cie_y = 0;
  if (tcs.getCIE(&cie_x, &cie_y)) {
    canvas.setCursor(TEXT_X, TEXT_Y);
    canvas.print("CIE ");
    canvas.print(cie_x, 3);
    canvas.print(",");
    canvas.print(cie_y, 3);

    canvas.setCursor(TEXT_X, TEXT_Y + 12);
    canvas.print("CCT ");
    canvas.print((int)tcs.getCCT());
    canvas.print(" K");

    canvas.setCursor(TEXT_X, TEXT_Y + 24);
    canvas.print("Lux ");
    canvas.print(tcs.getLux(), 0);
  }

  // Bottom status line
  canvas.setCursor(BAR_LEFT, 124);
  canvas.setTextColor(0x7BEF); // gray
  canvas.print("G:64X  T:100ms");

  display.drawRGBBitmap(0, 0, canvas.getBuffer(), 240, 135);
  delay(200);
}
