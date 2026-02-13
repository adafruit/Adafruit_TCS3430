#include "Adafruit_TCS3430.h"

Adafruit_TCS3430 tcs = Adafruit_TCS3430();

static bool readAverage(uint16_t* x, uint16_t* y, uint16_t* z, uint16_t* ir1) {
  const uint8_t samples = 5;
  uint32_t sx = 0, sy = 0, sz = 0, sir = 0;
  for (uint8_t i = 0; i < samples; i++) {
    uint16_t cx, cy, cz, cir;
    if (!tcs.getChannels(&cx, &cy, &cz, &cir)) {
      return false;
    }
    sx += cx;
    sy += cy;
    sz += cz;
    sir += cir;
    delay(50);
  }
  *x = sx / samples;
  *y = sy / samples;
  *z = sz / samples;
  *ir1 = sir / samples;
  return true;
}

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    delay(10);
  }

  Serial.println("TEST_START: test_channels");

  if (!tcs.begin()) {
    Serial.println("TEST_FAIL: test_channels: begin() failed");
    return;
  }

  uint16_t x = 0, y = 0, z = 0, ir1 = 0;
  if (!readAverage(&x, &y, &z, &ir1)) {
    Serial.println("TEST_FAIL: test_channels: read failed");
    return;
  }

  Serial.print("Channel averages: X=");
  Serial.print(x);
  Serial.print(" Y=");
  Serial.print(y);
  Serial.print(" Z=");
  Serial.print(z);
  Serial.print(" IR1=");
  Serial.println(ir1);

  if (x == 0 || y == 0 || z == 0 || ir1 == 0) {
    Serial.print("TEST_FAIL: test_channels: zero reading X=");
    Serial.print(x);
    Serial.print(" Y=");
    Serial.print(y);
    Serial.print(" Z=");
    Serial.print(z);
    Serial.print(" IR1=");
    Serial.println(ir1);
    return;
  }

  Serial.println("All channels non-zero.");
  Serial.println("TEST_PASS: test_channels");
}

void loop() {
  delay(1000);
}
