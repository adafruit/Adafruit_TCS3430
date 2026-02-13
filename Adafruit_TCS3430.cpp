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

/*!
 *    @brief  Set integration cycles
 *    @param  cycles Number of integration cycles (1-256)
 *    @return true on success
 */
bool Adafruit_TCS3430::setIntegrationCycles(uint8_t cycles) {
  Adafruit_BusIO_Register atime_reg =
      Adafruit_BusIO_Register(i2c_dev, TCS3430_REG_ATIME);
  return atime_reg.write(cycles);
}

/*!
 *    @brief  Get integration cycles
 *    @return Current integration cycles
 */
uint8_t Adafruit_TCS3430::getIntegrationCycles() {
  Adafruit_BusIO_Register atime_reg =
      Adafruit_BusIO_Register(i2c_dev, TCS3430_REG_ATIME);
  return atime_reg.read();
}

/*!
 *    @brief  Set integration time in milliseconds
 *    @param  ms Integration time in ms
 *    @return true on success
 */
bool Adafruit_TCS3430::setIntegrationTime(float ms) {
  uint8_t cycles = (uint8_t)((ms / 2.78) - 1);
  return setIntegrationCycles(cycles);
}

/*!
 *    @brief  Get integration time in milliseconds
 *    @return Integration time in ms
 */
float Adafruit_TCS3430::getIntegrationTime() {
  uint8_t cycles = getIntegrationCycles();
  return (cycles + 1) * 2.78;
}

/*!
 *    @brief  Enable/disable wait functionality
 *    @param  enable true to enable wait
 *    @return true on success
 */
bool Adafruit_TCS3430::waitEnable(bool enable) {
  Adafruit_BusIO_Register enable_reg =
      Adafruit_BusIO_Register(i2c_dev, TCS3430_REG_ENABLE);
  Adafruit_BusIO_RegisterBits wen_bit =
      Adafruit_BusIO_RegisterBits(&enable_reg, 1, 3);
  return wen_bit.write(enable);
}

/*!
 *    @brief  Check if wait is enabled
 *    @return true if wait is enabled
 */
bool Adafruit_TCS3430::isWaitEnabled() {
  Adafruit_BusIO_Register enable_reg =
      Adafruit_BusIO_Register(i2c_dev, TCS3430_REG_ENABLE);
  Adafruit_BusIO_RegisterBits wen_bit =
      Adafruit_BusIO_RegisterBits(&enable_reg, 1, 3);
  return wen_bit.read();
}

/*!
 *    @brief  Enable/disable ALS functionality
 *    @param  enable true to enable ALS
 *    @return true on success
 */
bool Adafruit_TCS3430::ALSEnable(bool enable) {
  Adafruit_BusIO_Register enable_reg =
      Adafruit_BusIO_Register(i2c_dev, TCS3430_REG_ENABLE);
  Adafruit_BusIO_RegisterBits aen_bit =
      Adafruit_BusIO_RegisterBits(&enable_reg, 1, 1);
  return aen_bit.write(enable);
}

/*!
 *    @brief  Check if ALS is enabled
 *    @return true if ALS is enabled
 */
bool Adafruit_TCS3430::isALSEnabled() {
  Adafruit_BusIO_Register enable_reg =
      Adafruit_BusIO_Register(i2c_dev, TCS3430_REG_ENABLE);
  Adafruit_BusIO_RegisterBits aen_bit =
      Adafruit_BusIO_RegisterBits(&enable_reg, 1, 1);
  return aen_bit.read();
}

/*!
 *    @brief  Power on/off the sensor
 *    @param  enable true to power on
 *    @return true on success
 */
bool Adafruit_TCS3430::powerOn(bool enable) {
  Adafruit_BusIO_Register enable_reg =
      Adafruit_BusIO_Register(i2c_dev, TCS3430_REG_ENABLE);
  Adafruit_BusIO_RegisterBits pon_bit =
      Adafruit_BusIO_RegisterBits(&enable_reg, 1, 0);
  return pon_bit.write(enable);
}

/*!
 *    @brief  Check if sensor is powered on
 *    @return true if powered on
 */
bool Adafruit_TCS3430::isPoweredOn() {
  Adafruit_BusIO_Register enable_reg =
      Adafruit_BusIO_Register(i2c_dev, TCS3430_REG_ENABLE);
  Adafruit_BusIO_RegisterBits pon_bit =
      Adafruit_BusIO_RegisterBits(&enable_reg, 1, 0);
  return pon_bit.read();
}

/*!
 *    @brief  Set wait cycles
 *    @param  cycles Number of wait cycles
 *    @return true on success
 */
bool Adafruit_TCS3430::setWaitCycles(uint8_t cycles) {
  Adafruit_BusIO_Register wtime_reg =
      Adafruit_BusIO_Register(i2c_dev, TCS3430_REG_WTIME);
  return wtime_reg.write(cycles);
}

/*!
 *    @brief  Get wait cycles
 *    @return Current wait cycles
 */
uint8_t Adafruit_TCS3430::getWaitCycles() {
  Adafruit_BusIO_Register wtime_reg =
      Adafruit_BusIO_Register(i2c_dev, TCS3430_REG_WTIME);
  return wtime_reg.read();
}

/*!
 *    @brief  Set wait time in milliseconds
 *    @param  ms Wait time in ms
 *    @return true on success
 */
bool Adafruit_TCS3430::setWaitTime(float ms) {
  uint8_t cycles = (uint8_t)((ms / 2.78) - 1);
  return setWaitCycles(cycles);
}

/*!
 *    @brief  Get wait time in milliseconds
 *    @return Wait time in ms
 */
float Adafruit_TCS3430::getWaitTime() {
  uint8_t cycles = getWaitCycles();
  return (cycles + 1) * 2.78;
}

/*!
 *    @brief  Set ALS low threshold
 *    @param  threshold Low threshold value
 *    @return true on success
 */
bool Adafruit_TCS3430::setALSThresholdLow(uint16_t threshold) {
  Adafruit_BusIO_Register threshold_reg =
      Adafruit_BusIO_Register(i2c_dev, TCS3430_REG_AILTL, 2, LSBFIRST);
  return threshold_reg.write(threshold);
}

/*!
 *    @brief  Get ALS low threshold
 *    @return Current low threshold value
 */
uint16_t Adafruit_TCS3430::getALSThresholdLow() {
  Adafruit_BusIO_Register threshold_reg =
      Adafruit_BusIO_Register(i2c_dev, TCS3430_REG_AILTL, 2, LSBFIRST);
  return threshold_reg.read();
}

/*!
 *    @brief  Set ALS high threshold
 *    @param  threshold High threshold value
 *    @return true on success
 */
bool Adafruit_TCS3430::setALSThresholdHigh(uint16_t threshold) {
  Adafruit_BusIO_Register threshold_reg =
      Adafruit_BusIO_Register(i2c_dev, TCS3430_REG_AIHTL, 2, LSBFIRST);
  return threshold_reg.write(threshold);
}

/*!
 *    @brief  Get ALS high threshold
 *    @return Current high threshold value
 */
uint16_t Adafruit_TCS3430::getALSThresholdHigh() {
  Adafruit_BusIO_Register threshold_reg =
      Adafruit_BusIO_Register(i2c_dev, TCS3430_REG_AIHTL, 2, LSBFIRST);
  return threshold_reg.read();
}

/*!
 *    @brief  Set interrupt persistence
 *    @param  persistence Persistence setting
 *    @return true on success
 */
bool Adafruit_TCS3430::setInterruptPersistence(tcs3430_pers_t persistence) {
  Adafruit_BusIO_Register pers_reg =
      Adafruit_BusIO_Register(i2c_dev, TCS3430_REG_PERS);
  Adafruit_BusIO_RegisterBits apers_bits =
      Adafruit_BusIO_RegisterBits(&pers_reg, 4, 0);
  return apers_bits.write(persistence);
}

/*!
 *    @brief  Get interrupt persistence
 *    @return Current persistence setting
 */
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

/*!
 *    @brief  Set ALS MUX to IR2 or X channel
 *    @param  enable true for IR2, false for X channel
 *    @return true on success
 */
bool Adafruit_TCS3430::setALSMUX_IR2(bool enable) {
  Adafruit_BusIO_Register cfg1_reg =
      Adafruit_BusIO_Register(i2c_dev, TCS3430_REG_CFG1);
  Adafruit_BusIO_RegisterBits amux_bit =
      Adafruit_BusIO_RegisterBits(&cfg1_reg, 1, 3);
  return amux_bit.write(enable);
}

/*!
 *    @brief  Get ALS MUX setting
 *    @return true if IR2 channel, false if X channel
 */
bool Adafruit_TCS3430::getALSMUX_IR2() {
  Adafruit_BusIO_Register cfg1_reg =
      Adafruit_BusIO_Register(i2c_dev, TCS3430_REG_CFG1);
  Adafruit_BusIO_RegisterBits amux_bit =
      Adafruit_BusIO_RegisterBits(&cfg1_reg, 1, 3);
  return amux_bit.read();
}

/*!
 *    @brief  Set ALS gain
 *    @param  gain Gain setting
 *    @return true on success
 */
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

/*!
 *    @brief  Get ALS gain
 *    @return Current gain setting
 */
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

/*!
 *    @brief  Check if ALS is saturated
 *    @return true if saturated
 */
bool Adafruit_TCS3430::isALSSaturated() {
  Adafruit_BusIO_Register status_reg =
      Adafruit_BusIO_Register(i2c_dev, TCS3430_REG_STATUS);
  Adafruit_BusIO_RegisterBits asat_bit =
      Adafruit_BusIO_RegisterBits(&status_reg, 1, 7);
  return asat_bit.read();
}

/*!
 *    @brief  Clear ALS saturation flag
 *    @return true on success
 */
bool Adafruit_TCS3430::clearALSSaturated() {
  // Write 0x80 to STATUS to clear saturation flag
  Adafruit_BusIO_Register status_reg =
      Adafruit_BusIO_Register(i2c_dev, TCS3430_REG_STATUS);
  return status_reg.write(0x80);
}

/*!
 *    @brief  Check if ALS interrupt is active
 *    @return true if interrupt is active
 */
bool Adafruit_TCS3430::isALSInterrupt() {
  Adafruit_BusIO_Register status_reg =
      Adafruit_BusIO_Register(i2c_dev, TCS3430_REG_STATUS);
  Adafruit_BusIO_RegisterBits aint_bit =
      Adafruit_BusIO_RegisterBits(&status_reg, 1, 4);
  return aint_bit.read();
}

/*!
 *    @brief  Clear ALS interrupt flag
 *    @return true on success
 */
bool Adafruit_TCS3430::clearALSInterrupt() {
  // Write 0xFF to STATUS to clear all flags. Note: if the threshold
  // condition still exists and ALS is running, AINT will re-fire
  // on the next integration cycle.
  Adafruit_BusIO_Register status_reg =
      Adafruit_BusIO_Register(i2c_dev, TCS3430_REG_STATUS);
  return status_reg.write(0xFF);
}

/*!
 *    @brief  Read X, Y, Z, and IR1 channels in a single burst
 *    @param  x Pointer to store X channel data
 *    @param  y Pointer to store Y channel data
 *    @param  z Pointer to store Z channel data
 *    @param  ir1 Pointer to store IR1 channel data
 *    @return true on success
 */
bool Adafruit_TCS3430::getChannels(uint16_t* x, uint16_t* y, uint16_t* z,
                                   uint16_t* ir1) {
  bool was_ir2 = getALSMUX_IR2();
  if (was_ir2) {
    if (!setALSMUX_IR2(false)) {
      return false;
    }
  }

  uint8_t buffer[8];
  Adafruit_BusIO_Register data_reg =
      Adafruit_BusIO_Register(i2c_dev, TCS3430_REG_CH0DATAL, 8);
  if (!data_reg.read(buffer, 8)) {
    if (was_ir2) {
      setALSMUX_IR2(true);
    }
    return false;
  }

  *z = buffer[0] | ((uint16_t)buffer[1] << 8);
  *y = buffer[2] | ((uint16_t)buffer[3] << 8);
  *ir1 = buffer[4] | ((uint16_t)buffer[5] << 8);
  *x = buffer[6] | ((uint16_t)buffer[7] << 8);

  if (was_ir2) {
    setALSMUX_IR2(true);
  }

  return true;
}

/*!
 *    @brief  Read IR2 (CH3) by switching AMUX to IR2
 *    @return IR2 channel value
 */
uint16_t Adafruit_TCS3430::getIR2() {
  uint16_t ir2 = 0;
  bool was_ir2 = getALSMUX_IR2();

  if (!setALSMUX_IR2(true)) {
    return 0;
  }

  delay((uint16_t)getIntegrationTime());

  Adafruit_BusIO_Register ch3_reg =
      Adafruit_BusIO_Register(i2c_dev, TCS3430_REG_CH3DATAL, 2, LSBFIRST);
  ir2 = ch3_reg.read();

  if (!was_ir2) {
    setALSMUX_IR2(false);
  }

  return ir2;
}

/*!
 *    @brief  Calculate CIE 1931 chromaticity coordinates
 *    @param  CIEx Pointer to store CIE x
 *    @param  CIEy Pointer to store CIE y
 *    @return true on success
 */
bool Adafruit_TCS3430::getCIE(float* CIEx, float* CIEy) {
  uint16_t x = 0;
  uint16_t y = 0;
  uint16_t z = 0;
  uint16_t ir1 = 0;

  if (!getChannels(&x, &y, &z, &ir1)) {
    return false;
  }

  float sum = (float)x + (float)y + (float)z;
  if (sum <= 0.0f) {
    *CIEx = 0.0f;
    *CIEy = 0.0f;
    return false;
  }

  *CIEx = (float)x / sum;
  *CIEy = (float)y / sum;
  return true;
}

/*!
 *    @brief  Calculate lux from the Y channel
 *    @return Calculated lux
 */
float Adafruit_TCS3430::getLux() {
  uint16_t x = 0;
  uint16_t y = 0;
  uint16_t z = 0;
  uint16_t ir1 = 0;

  if (!getChannels(&x, &y, &z, &ir1)) {
    return 0.0f;
  }

  float gain = 1.0f;
  switch (getALSGain()) {
    case TCS3430_GAIN_1X:
      gain = 1.0f;
      break;
    case TCS3430_GAIN_4X:
      gain = 4.0f;
      break;
    case TCS3430_GAIN_16X:
      gain = 16.0f;
      break;
    case TCS3430_GAIN_64X:
      gain = 66.0f;
      break;
    case TCS3430_GAIN_128X:
      gain = 137.0f;
      break;
    default:
      gain = 1.0f;
      break;
  }

  float integration_ms = getIntegrationTime();
  if (integration_ms <= 0.0f) {
    return 0.0f;
  }

  return (float)y / gain / (integration_ms / 1000.0f);
}

/*!
 *    @brief  Calculate correlated color temperature (CCT)
 *    @return Correlated color temperature in Kelvin
 */
float Adafruit_TCS3430::getCCT() {
  float x = 0.0f;
  float y = 0.0f;

  if (!getCIE(&x, &y)) {
    return 0.0f;
  }

  float n = (x - 0.3320f) / (0.1858f - y);
  return (449.0f * n * n * n) + (3525.0f * n * n) + (6823.3f * n) + 5520.33f;
}

/*!
 *    @brief  Set interrupt clear on read mode
 *    @param  enable true to enable clear on read
 *    @return true on success
 */
bool Adafruit_TCS3430::setInterruptClearOnRead(bool enable) {
  Adafruit_BusIO_Register cfg3_reg =
      Adafruit_BusIO_Register(i2c_dev, TCS3430_REG_CFG3);
  Adafruit_BusIO_RegisterBits int_read_clear_bit =
      Adafruit_BusIO_RegisterBits(&cfg3_reg, 1, 7);
  return int_read_clear_bit.write(enable);
}

/*!
 *    @brief  Get interrupt clear on read mode
 *    @return true if clear on read is enabled
 */
bool Adafruit_TCS3430::getInterruptClearOnRead() {
  Adafruit_BusIO_Register cfg3_reg =
      Adafruit_BusIO_Register(i2c_dev, TCS3430_REG_CFG3);
  Adafruit_BusIO_RegisterBits int_read_clear_bit =
      Adafruit_BusIO_RegisterBits(&cfg3_reg, 1, 7);
  return int_read_clear_bit.read();
}

/*!
 *    @brief  Set sleep after interrupt mode
 *    @param  enable true to enable sleep after interrupt
 *    @return true on success
 */
bool Adafruit_TCS3430::setSleepAfterInterrupt(bool enable) {
  Adafruit_BusIO_Register cfg3_reg =
      Adafruit_BusIO_Register(i2c_dev, TCS3430_REG_CFG3);
  Adafruit_BusIO_RegisterBits sai_bit =
      Adafruit_BusIO_RegisterBits(&cfg3_reg, 1, 4);
  return sai_bit.write(enable);
}

/*!
 *    @brief  Get sleep after interrupt mode
 *    @return true if sleep after interrupt is enabled
 */
bool Adafruit_TCS3430::getSleepAfterInterrupt() {
  Adafruit_BusIO_Register cfg3_reg =
      Adafruit_BusIO_Register(i2c_dev, TCS3430_REG_CFG3);
  Adafruit_BusIO_RegisterBits sai_bit =
      Adafruit_BusIO_RegisterBits(&cfg3_reg, 1, 4);
  return sai_bit.read();
}

/*!
 *    @brief  Set auto-zero mode
 *    @param  enable true to enable auto-zero
 *    @return true on success
 */
bool Adafruit_TCS3430::setAutoZeroMode(bool enable) {
  Adafruit_BusIO_Register az_config_reg =
      Adafruit_BusIO_Register(i2c_dev, TCS3430_REG_AZ_CONFIG);
  Adafruit_BusIO_RegisterBits az_mode_bit =
      Adafruit_BusIO_RegisterBits(&az_config_reg, 1, 7);
  return az_mode_bit.write(enable);
}

/*!
 *    @brief  Get auto-zero mode
 *    @return true if auto-zero is enabled
 */
bool Adafruit_TCS3430::getAutoZeroMode() {
  Adafruit_BusIO_Register az_config_reg =
      Adafruit_BusIO_Register(i2c_dev, TCS3430_REG_AZ_CONFIG);
  Adafruit_BusIO_RegisterBits az_mode_bit =
      Adafruit_BusIO_RegisterBits(&az_config_reg, 1, 7);
  return az_mode_bit.read();
}

/*!
 *    @brief  Set auto-zero interval
 *    @param  n Run auto-zero every N measurements
 *    @return true on success
 */
bool Adafruit_TCS3430::setRunAutoZeroEveryN(uint8_t n) {
  Adafruit_BusIO_Register az_config_reg =
      Adafruit_BusIO_Register(i2c_dev, TCS3430_REG_AZ_CONFIG);
  Adafruit_BusIO_RegisterBits az_nth_bits =
      Adafruit_BusIO_RegisterBits(&az_config_reg, 7, 0);
  return az_nth_bits.write(n);
}

/*!
 *    @brief  Get auto-zero interval
 *    @return Auto-zero interval (every N measurements)
 */
uint8_t Adafruit_TCS3430::getRunAutoZeroEveryN() {
  Adafruit_BusIO_Register az_config_reg =
      Adafruit_BusIO_Register(i2c_dev, TCS3430_REG_AZ_CONFIG);
  Adafruit_BusIO_RegisterBits az_nth_bits =
      Adafruit_BusIO_RegisterBits(&az_config_reg, 7, 0);
  return az_nth_bits.read();
}

/*!
 *    @brief  Enable/disable saturation interrupt
 *    @param  enable true to enable saturation interrupt
 *    @return true on success
 */
bool Adafruit_TCS3430::enableSaturationInt(bool enable) {
  Adafruit_BusIO_Register intenab_reg =
      Adafruit_BusIO_Register(i2c_dev, TCS3430_REG_INTENAB);
  Adafruit_BusIO_RegisterBits asien_bit =
      Adafruit_BusIO_RegisterBits(&intenab_reg, 1, 7);
  return asien_bit.write(enable);
}

/*!
 *    @brief  Enable/disable ALS interrupt
 *    @param  enable true to enable ALS interrupt
 *    @return true on success
 */
bool Adafruit_TCS3430::enableALSInt(bool enable) {
  Adafruit_BusIO_Register intenab_reg =
      Adafruit_BusIO_Register(i2c_dev, TCS3430_REG_INTENAB);
  Adafruit_BusIO_RegisterBits aien_bit =
      Adafruit_BusIO_RegisterBits(&intenab_reg, 1, 4);
  return aien_bit.write(enable);
}