#include "Adafruit_TCS3430.h"

Adafruit_TCS3430 tcs = Adafruit_TCS3430();

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    delay(10);
  }

  Serial.println("TEST_START: test_amux_ir2");

  if (!tcs.begin()) {
    Serial.println("TEST_FAIL: test_amux_ir2: begin() failed");
    return;
  }

  if (!tcs.setALSMUX_IR2(false)) {
    Serial.println("TEST_FAIL: test_amux_ir2: set AMUX X failed");
    return;
  }

  uint16_t x, y, z, ir1;
  if (!tcs.getChannels(&x, &y, &z, &ir1)) {
    Serial.println("TEST_FAIL: test_amux_ir2: read channels failed");
    return;
  }

  uint16_t ir2 = tcs.getIR2();

  if (x == 0 || ir2 == 0) {
    Serial.print("TEST_FAIL: test_amux_ir2: zero X/IR2 X=");
    Serial.print(x);
    Serial.print(" IR2=");
    Serial.println(ir2);
    return;
  }

  Serial.println("TEST_PASS: test_amux_ir2");
}

void loop() {
  delay(1000);
}
