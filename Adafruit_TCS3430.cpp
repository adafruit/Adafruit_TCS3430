/*!
 *  @file Adafruit_TCS3430.cpp
 *
 *  @mainpage Adafruit TCS3430 Color and ALS Sensor
 *
 *  @section intro_sec Introduction
 *
 * 	I2C Driver for TCS3430 Color and ALS Sensor
 *
 * 	This is a library for the Adafruit TCS3430 breakout:
 * 	http://www.adafruit.com/
 *
 * 	Adafruit invests time and resources providing this open source code,
 *  please support Adafruit and open-source hardware by purchasing products from
 * 	Adafruit!
 *
 *  @section author Author
 *
 *  Limor 'ladyada' Fried with assistance from Claude Code
 *
 * 	@section license License
 *
 * 	BSD (see license.txt)
 *
 * 	@section  HISTORY
 *
 *     v1.0 - First release
 */

#include "Adafruit_TCS3430.h"

#include <Wire.h>

#include "Arduino.h"

/*!
 *    @brief  Instantiates a new TCS3430 class
 */
Adafruit_TCS3430::Adafruit_TCS3430() {}

/*!
 *    @brief  Cleans up the TCS3430
 */
Adafruit_TCS3430::~Adafruit_TCS3430() {
  if (i2c_dev) {
    delete i2c_dev;
  }
}

/*!
 *    @brief  Sets up the hardware and initializes I2C
 *    @param  addr
 *            The I2C address to be used.
 *    @param  theWire
 *            The Wire object to be used for I2C connections.
 *    @return True if initialization was successful, otherwise false.
 */
bool Adafruit_TCS3430::begin(uint8_t addr, TwoWire* theWire) {
  if (i2c_dev) {
    delete i2c_dev;
  }

  i2c_dev = new Adafruit_I2CDevice(addr, theWire);

  if (!i2c_dev->begin()) {
    return false;
  }

  // Check chip ID
  Adafruit_BusIO_Register id_reg =
      Adafruit_BusIO_Register(i2c_dev, TCS3430_REG_ID);
  uint8_t chip_id = id_reg.read();
  if (chip_id != 0xDC) {
    return false;
  }

  // Power on and enable ALS
  if (!powerOn(true)) {
    return false;
  }
  if (!ALSEnable(true)) {
    return false;
  }

  return true;
}

bool Adafruit_TCS3430::setIntegrationCycles(uint8_t cycles) {
  Adafruit_BusIO_Register atime_reg =
      Adafruit_BusIO_Register(i2c_dev, TCS3430_REG_ATIME);
  return atime_reg.write(cycles);
}

uint8_t Adafruit_TCS3430::getIntegrationCycles() {
  Adafruit_BusIO_Register atime_reg =
      Adafruit_BusIO_Register(i2c_dev, TCS3430_REG_ATIME);
  return atime_reg.read();
}

bool Adafruit_TCS3430::setIntegrationTime(float ms) {
  uint8_t cycles = (uint8_t)((ms / 2.78) - 1);
  return setIntegrationCycles(cycles);
}

float Adafruit_TCS3430::getIntegrationTime() {
  uint8_t cycles = getIntegrationCycles();
  return (cycles + 1) * 2.78;
}

bool Adafruit_TCS3430::waitEnable(bool enable) {
  Adafruit_BusIO_Register enable_reg =
      Adafruit_BusIO_Register(i2c_dev, TCS3430_REG_ENABLE);
  Adafruit_BusIO_RegisterBits wen_bit =
      Adafruit_BusIO_RegisterBits(&enable_reg, 1, 3);
  return wen_bit.write(enable);
}

bool Adafruit_TCS3430::isWaitEnabled() {
  Adafruit_BusIO_Register enable_reg =
      Adafruit_BusIO_Register(i2c_dev, TCS3430_REG_ENABLE);
  Adafruit_BusIO_RegisterBits wen_bit =
      Adafruit_BusIO_RegisterBits(&enable_reg, 1, 3);
  return wen_bit.read();
}

bool Adafruit_TCS3430::ALSEnable(bool enable) {
  Adafruit_BusIO_Register enable_reg =
      Adafruit_BusIO_Register(i2c_dev, TCS3430_REG_ENABLE);
  Adafruit_BusIO_RegisterBits aen_bit =
      Adafruit_BusIO_RegisterBits(&enable_reg, 1, 1);
  return aen_bit.write(enable);
}

bool Adafruit_TCS3430::isALSEnabled() {
  Adafruit_BusIO_Register enable_reg =
      Adafruit_BusIO_Register(i2c_dev, TCS3430_REG_ENABLE);
  Adafruit_BusIO_RegisterBits aen_bit =
      Adafruit_BusIO_RegisterBits(&enable_reg, 1, 1);
  return aen_bit.read();
}

bool Adafruit_TCS3430::powerOn(bool enable) {
  Adafruit_BusIO_Register enable_reg =
      Adafruit_BusIO_Register(i2c_dev, TCS3430_REG_ENABLE);
  Adafruit_BusIO_RegisterBits pon_bit =
      Adafruit_BusIO_RegisterBits(&enable_reg, 1, 0);
  return pon_bit.write(enable);
}

bool Adafruit_TCS3430::isPoweredOn() {
  Adafruit_BusIO_Register enable_reg =
      Adafruit_BusIO_Register(i2c_dev, TCS3430_REG_ENABLE);
  Adafruit_BusIO_RegisterBits pon_bit =
      Adafruit_BusIO_RegisterBits(&enable_reg, 1, 0);
  return pon_bit.read();
}

bool Adafruit_TCS3430::setWaitCycles(uint8_t cycles) {
  Adafruit_BusIO_Register wtime_reg =
      Adafruit_BusIO_Register(i2c_dev, TCS3430_REG_WTIME);
  return wtime_reg.write(cycles);
}

uint8_t Adafruit_TCS3430::getWaitCycles() {
  Adafruit_BusIO_Register wtime_reg =
      Adafruit_BusIO_Register(i2c_dev, TCS3430_REG_WTIME);
  return wtime_reg.read();
}

bool Adafruit_TCS3430::setWaitTime(float ms) {
  uint8_t cycles = (uint8_t)((ms / 2.78) - 1);
  return setWaitCycles(cycles);
}

float Adafruit_TCS3430::getWaitTime() {
  uint8_t cycles = getWaitCycles();
  return (cycles + 1) * 2.78;
}

bool Adafruit_TCS3430::setALSThresholdLow(uint16_t threshold) {
  Adafruit_BusIO_Register threshold_reg =
      Adafruit_BusIO_Register(i2c_dev, TCS3430_REG_AILTL, 2, LSBFIRST);
  return threshold_reg.write(threshold);
}

uint16_t Adafruit_TCS3430::getALSThresholdLow() {
  Adafruit_BusIO_Register threshold_reg =
      Adafruit_BusIO_Register(i2c_dev, TCS3430_REG_AILTL, 2, LSBFIRST);
  return threshold_reg.read();
}

bool Adafruit_TCS3430::setALSThresholdHigh(uint16_t threshold) {
  Adafruit_BusIO_Register threshold_reg =
      Adafruit_BusIO_Register(i2c_dev, TCS3430_REG_AIHTL, 2, LSBFIRST);
  return threshold_reg.write(threshold);
}

uint16_t Adafruit_TCS3430::getALSThresholdHigh() {
  Adafruit_BusIO_Register threshold_reg =
      Adafruit_BusIO_Register(i2c_dev, TCS3430_REG_AIHTL, 2, LSBFIRST);
  return threshold_reg.read();
}

bool Adafruit_TCS3430::setInterruptPersistence(tcs3430_pers_t persistence) {
  Adafruit_BusIO_Register pers_reg =
      Adafruit_BusIO_Register(i2c_dev, TCS3430_REG_PERS);
  Adafruit_BusIO_RegisterBits apers_bits =
      Adafruit_BusIO_RegisterBits(&pers_reg, 4, 0);
  return apers_bits.write(persistence);
}

tcs3430_pers_t Adafruit_TCS3430::getInterruptPersistence() {
  Adafruit_BusIO_Register pers_reg =
      Adafruit_BusIO_Register(i2c_dev, TCS3430_REG_PERS);
  Adafruit_BusIO_RegisterBits apers_bits =
      Adafruit_BusIO_RegisterBits(&pers_reg, 4, 0);
  return (tcs3430_pers_t)apers_bits.read();
}

/*!
 *    @brief  Sets wait long mode
 *    @param  enable true to enable 12x wait time multiplier
 *    @return true on success
 */
bool Adafruit_TCS3430::setWaitLong(bool enable) {
  Adafruit_BusIO_Register cfg0_reg =
      Adafruit_BusIO_Register(i2c_dev, TCS3430_REG_CFG0);
  Adafruit_BusIO_RegisterBits wlong_bit =
      Adafruit_BusIO_RegisterBits(&cfg0_reg, 1, 2);
  return wlong_bit.write(enable);
}

/*!
 *    @brief  Gets wait long mode status
 *    @return true if 12x wait time multiplier is enabled
 */
bool Adafruit_TCS3430::getWaitLong() {
  Adafruit_BusIO_Register cfg0_reg =
      Adafruit_BusIO_Register(i2c_dev, TCS3430_REG_CFG0);
  Adafruit_BusIO_RegisterBits wlong_bit =
      Adafruit_BusIO_RegisterBits(&cfg0_reg, 1, 2);
  return wlong_bit.read();
}

bool Adafruit_TCS3430::setALSMUX_IR2(bool enable) {
  Adafruit_BusIO_Register cfg1_reg =
      Adafruit_BusIO_Register(i2c_dev, TCS3430_REG_CFG1);
  Adafruit_BusIO_RegisterBits amux_bit =
      Adafruit_BusIO_RegisterBits(&cfg1_reg, 1, 3);
  return amux_bit.write(enable);
}

bool Adafruit_TCS3430::getALSMUX_IR2() {
  Adafruit_BusIO_Register cfg1_reg =
      Adafruit_BusIO_Register(i2c_dev, TCS3430_REG_CFG1);
  Adafruit_BusIO_RegisterBits amux_bit =
      Adafruit_BusIO_RegisterBits(&cfg1_reg, 1, 3);
  return amux_bit.read();
}

bool Adafruit_TCS3430::setALSGain(tcs3430_gain_t gain) {
  Adafruit_BusIO_Register cfg1_reg =
      Adafruit_BusIO_Register(i2c_dev, TCS3430_REG_CFG1);
  Adafruit_BusIO_RegisterBits again_bits =
      Adafruit_BusIO_RegisterBits(&cfg1_reg, 2, 0);

  if (gain == TCS3430_GAIN_128X) {
    Adafruit_BusIO_Register cfg2_reg =
        Adafruit_BusIO_Register(i2c_dev, TCS3430_REG_CFG2);
    Adafruit_BusIO_RegisterBits hgain_bit =
        Adafruit_BusIO_RegisterBits(&cfg2_reg, 1, 4);
    if (!again_bits.write(TCS3430_GAIN_64X) || !hgain_bit.write(1)) {
      return false;
    }
  } else {
    Adafruit_BusIO_Register cfg2_reg =
        Adafruit_BusIO_Register(i2c_dev, TCS3430_REG_CFG2);
    Adafruit_BusIO_RegisterBits hgain_bit =
        Adafruit_BusIO_RegisterBits(&cfg2_reg, 1, 4);
    if (!again_bits.write(gain) || !hgain_bit.write(0)) {
      return false;
    }
  }
  return true;
}

tcs3430_gain_t Adafruit_TCS3430::getALSGain() {
  Adafruit_BusIO_Register cfg1_reg =
      Adafruit_BusIO_Register(i2c_dev, TCS3430_REG_CFG1);
  Adafruit_BusIO_RegisterBits again_bits =
      Adafruit_BusIO_RegisterBits(&cfg1_reg, 2, 0);

  Adafruit_BusIO_Register cfg2_reg =
      Adafruit_BusIO_Register(i2c_dev, TCS3430_REG_CFG2);
  Adafruit_BusIO_RegisterBits hgain_bit =
      Adafruit_BusIO_RegisterBits(&cfg2_reg, 1, 4);

  uint8_t again_val = again_bits.read();
  bool hgain_val = hgain_bit.read();

  if (again_val == TCS3430_GAIN_64X && hgain_val) {
    return TCS3430_GAIN_128X;
  }
  return (tcs3430_gain_t)again_val;
}

bool Adafruit_TCS3430::isALSSaturated() {
  Adafruit_BusIO_Register status_reg =
      Adafruit_BusIO_Register(i2c_dev, TCS3430_REG_STATUS);
  Adafruit_BusIO_RegisterBits asat_bit =
      Adafruit_BusIO_RegisterBits(&status_reg, 1, 7);
  return asat_bit.read();
}

bool Adafruit_TCS3430::clearALSSaturated() {
  Adafruit_BusIO_Register status_reg =
      Adafruit_BusIO_Register(i2c_dev, TCS3430_REG_STATUS);
  Adafruit_BusIO_RegisterBits asat_bit =
      Adafruit_BusIO_RegisterBits(&status_reg, 1, 7);
  return asat_bit.write(1);
}

bool Adafruit_TCS3430::isALSInterrupt() {
  Adafruit_BusIO_Register status_reg =
      Adafruit_BusIO_Register(i2c_dev, TCS3430_REG_STATUS);
  Adafruit_BusIO_RegisterBits aint_bit =
      Adafruit_BusIO_RegisterBits(&status_reg, 1, 4);
  return aint_bit.read();
}

bool Adafruit_TCS3430::clearALSInterrupt() {
  Adafruit_BusIO_Register status_reg =
      Adafruit_BusIO_Register(i2c_dev, TCS3430_REG_STATUS);
  Adafruit_BusIO_RegisterBits aint_bit =
      Adafruit_BusIO_RegisterBits(&status_reg, 1, 4);
  return aint_bit.write(1);
}

void Adafruit_TCS3430::getData(uint16_t* x, uint16_t* y, uint16_t* z) {
  uint8_t buffer[6];
  Adafruit_BusIO_Register data_reg =
      Adafruit_BusIO_Register(i2c_dev, TCS3430_REG_CH0DATAL, 6);
  data_reg.read(buffer, 6);

  *z = buffer[0] | ((uint16_t)buffer[1] << 8);
  *y = buffer[2] | ((uint16_t)buffer[3] << 8);
  *x = buffer[4] | ((uint16_t)buffer[5] << 8);
}

bool Adafruit_TCS3430::setInterruptClearOnRead(bool enable) {
  Adafruit_BusIO_Register cfg3_reg =
      Adafruit_BusIO_Register(i2c_dev, TCS3430_REG_CFG3);
  Adafruit_BusIO_RegisterBits int_read_clear_bit =
      Adafruit_BusIO_RegisterBits(&cfg3_reg, 1, 7);
  return int_read_clear_bit.write(enable);
}

bool Adafruit_TCS3430::getInterruptClearOnRead() {
  Adafruit_BusIO_Register cfg3_reg =
      Adafruit_BusIO_Register(i2c_dev, TCS3430_REG_CFG3);
  Adafruit_BusIO_RegisterBits int_read_clear_bit =
      Adafruit_BusIO_RegisterBits(&cfg3_reg, 1, 7);
  return int_read_clear_bit.read();
}

bool Adafruit_TCS3430::setSleepAfterInterrupt(bool enable) {
  Adafruit_BusIO_Register cfg3_reg =
      Adafruit_BusIO_Register(i2c_dev, TCS3430_REG_CFG3);
  Adafruit_BusIO_RegisterBits sai_bit =
      Adafruit_BusIO_RegisterBits(&cfg3_reg, 1, 4);
  return sai_bit.write(enable);
}

bool Adafruit_TCS3430::getSleepAfterInterrupt() {
  Adafruit_BusIO_Register cfg3_reg =
      Adafruit_BusIO_Register(i2c_dev, TCS3430_REG_CFG3);
  Adafruit_BusIO_RegisterBits sai_bit =
      Adafruit_BusIO_RegisterBits(&cfg3_reg, 1, 4);
  return sai_bit.read();
}

bool Adafruit_TCS3430::setAutoZeroMode(bool enable) {
  Adafruit_BusIO_Register az_config_reg =
      Adafruit_BusIO_Register(i2c_dev, TCS3430_REG_AZ_CONFIG);
  Adafruit_BusIO_RegisterBits az_mode_bit =
      Adafruit_BusIO_RegisterBits(&az_config_reg, 1, 7);
  return az_mode_bit.write(enable);
}

bool Adafruit_TCS3430::getAutoZeroMode() {
  Adafruit_BusIO_Register az_config_reg =
      Adafruit_BusIO_Register(i2c_dev, TCS3430_REG_AZ_CONFIG);
  Adafruit_BusIO_RegisterBits az_mode_bit =
      Adafruit_BusIO_RegisterBits(&az_config_reg, 1, 7);
  return az_mode_bit.read();
}

bool Adafruit_TCS3430::setRunAutoZeroEveryN(uint8_t n) {
  Adafruit_BusIO_Register az_config_reg =
      Adafruit_BusIO_Register(i2c_dev, TCS3430_REG_AZ_CONFIG);
  Adafruit_BusIO_RegisterBits az_nth_bits =
      Adafruit_BusIO_RegisterBits(&az_config_reg, 7, 0);
  return az_nth_bits.write(n);
}

uint8_t Adafruit_TCS3430::getRunAutoZeroEveryN() {
  Adafruit_BusIO_Register az_config_reg =
      Adafruit_BusIO_Register(i2c_dev, TCS3430_REG_AZ_CONFIG);
  Adafruit_BusIO_RegisterBits az_nth_bits =
      Adafruit_BusIO_RegisterBits(&az_config_reg, 7, 0);
  return az_nth_bits.read();
}

bool Adafruit_TCS3430::enableSaturationInt(bool enable) {
  Adafruit_BusIO_Register intenab_reg =
      Adafruit_BusIO_Register(i2c_dev, TCS3430_REG_INTENAB);
  Adafruit_BusIO_RegisterBits asien_bit =
      Adafruit_BusIO_RegisterBits(&intenab_reg, 1, 7);
  return asien_bit.write(enable);
}

bool Adafruit_TCS3430::enableALSInt(bool enable) {
  Adafruit_BusIO_Register intenab_reg =
      Adafruit_BusIO_Register(i2c_dev, TCS3430_REG_INTENAB);
  Adafruit_BusIO_RegisterBits aien_bit =
      Adafruit_BusIO_RegisterBits(&intenab_reg, 1, 4);
  return aien_bit.write(enable);
}