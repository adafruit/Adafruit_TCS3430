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

  Serial.println("Setting AMUX to X (IR2 disabled)");
  if (!tcs.setALSMUX_IR2(false)) {
    Serial.println("TEST_FAIL: test_amux_ir2: set AMUX X failed");
    return;
  }

  uint16_t x, y, z, ir1;
  if (!tcs.getChannels(&x, &y, &z, &ir1)) {
    Serial.println("TEST_FAIL: test_amux_ir2: read channels failed");
    return;
  }

  Serial.print("AMUX off (X channel): X=");
  Serial.print(x);
  Serial.print(" Y=");
  Serial.print(y);
  Serial.print(" Z=");
  Serial.print(z);
  Serial.print(" IR1=");
  Serial.println(ir1);

  Serial.println("Setting AMUX to IR2");
  if (!tcs.setALSMUX_IR2(true)) {
    Serial.println("TEST_FAIL: test_amux_ir2: set AMUX IR2 failed");
    return;
  }

  delay((uint16_t)tcs.getIntegrationTime());
  uint16_t ir2 = tcs.getIR2();
  Serial.print("AMUX on (IR2): IR2=");
  Serial.println(ir2);

  tcs.setALSMUX_IR2(false);

  uint16_t diff = (ir2 > x) ? (ir2 - x) : (x - ir2);
  Serial.print("Difference between X and IR2: ");
  Serial.println(diff);

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
