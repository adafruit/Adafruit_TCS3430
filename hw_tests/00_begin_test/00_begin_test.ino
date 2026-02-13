#include <Adafruit_NeoPixel.h>
#include <Wire.h>

#include "Adafruit_TCS3430.h"

#define PIXEL_PIN 6
#define PIXEL_COUNT 16

Adafruit_TCS3430 tcs = Adafruit_TCS3430();
Adafruit_TCS3430 tcs_bad = Adafruit_TCS3430();
Adafruit_NeoPixel pixels(PIXEL_COUNT, PIXEL_PIN, NEO_GRB + NEO_KHZ800);

static uint8_t read8(uint8_t reg) {
  Wire.beginTransmission(TCS3430_DEFAULT_ADDR);
  Wire.write(reg);
  if (Wire.endTransmission(false) != 0) {
    return 0;
  }
  Wire.requestFrom(TCS3430_DEFAULT_ADDR, (uint8_t)1);
  if (!Wire.available()) {
    return 0;
  }
  return Wire.read();
}

static void setAll(uint8_t r, uint8_t g, uint8_t b) {
  for (uint16_t i = 0; i < PIXEL_COUNT; i++) {
    pixels.setPixelColor(i, pixels.Color(r, g, b));
  }
  pixels.show();
  delay(200);
}

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    delay(10);
  }
  Wire.begin();

  Serial.println("TEST_START: test_begin");

  bool begin_ok = tcs.begin();
  Serial.print("begin() result: ");
  Serial.println(begin_ok ? "true" : "false");
  if (!begin_ok) {
    Serial.println("TEST_FAIL: test_begin: begin() failed");
    return;
  }

  uint8_t chip_id = read8(TCS3430_REG_ID);
  Serial.print("Chip ID: 0x");
  Serial.print(chip_id, HEX);
  Serial.println(" (expected 0xDC)");
  if (chip_id != 0xDC) {
    Serial.print("TEST_FAIL: test_begin: bad chip id 0x");
    Serial.println(chip_id, HEX);
    return;
  }

  bool pon = tcs.isPoweredOn();
  Serial.print("PON status: ");
  Serial.println(pon ? "true" : "false");
  if (!pon) {
    Serial.println("TEST_FAIL: test_begin: PON not set");
    return;
  }

  bool aen = tcs.isALSEnabled();
  Serial.print("AEN status: ");
  Serial.println(aen ? "true" : "false");
  if (!aen) {
    Serial.println("TEST_FAIL: test_begin: AEN not set");
    return;
  }

  pixels.begin();
  pixels.setBrightness(80);
  setAll(255, 255, 255);

  tcs.setALSGain(TCS3430_GAIN_64X);
  delay(200);

  uint16_t x, y, z, ir1;
  if (!tcs.getChannels(&x, &y, &z, &ir1)) {
    Serial.println("TEST_FAIL: test_begin: channel read failed");
    return;
  }

  Serial.print("Channels: X=");
  Serial.print(x);
  Serial.print(" Y=");
  Serial.print(y);
  Serial.print(" Z=");
  Serial.print(z);
  Serial.print(" IR1=");
  Serial.println(ir1);

  if (x == 0 || y == 0 || z == 0 || ir1 == 0) {
    Serial.print("TEST_FAIL: test_begin: zero reading X=");
    Serial.print(x);
    Serial.print(" Y=");
    Serial.print(y);
    Serial.print(" Z=");
    Serial.print(z);
    Serial.print(" IR1=");
    Serial.println(ir1);
    return;
  }

  bool second_begin = tcs.begin();
  Serial.print("second begin() result: ");
  Serial.println(second_begin ? "true" : "false");
  if (!second_begin) {
    Serial.println("TEST_FAIL: test_begin: second begin failed");
    return;
  }

  bool bad_begin = tcs_bad.begin(0x3A);
  Serial.print("bad address begin() result: ");
  Serial.println(bad_begin ? "true" : "false");
  if (bad_begin) {
    Serial.println("TEST_FAIL: test_begin: bad address should fail");
    return;
  }

  setAll(0, 0, 0);
  Serial.println("TEST_PASS: test_begin");
}

void loop() {
  delay(1000);
}
