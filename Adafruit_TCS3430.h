/*!
 *  @file Adafruit_TCS3430.h
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
 *
 *	BSD license (see license.txt)
 */

#ifndef _ADAFRUIT_TCS3430_H
#define _ADAFRUIT_TCS3430_H

#include <Adafruit_BusIO_Register.h>
#include <Adafruit_I2CDevice.h>

#include "Arduino.h"

/*=========================================================================
    I2C ADDRESS/BITS
    -----------------------------------------------------------------------*/
/** Default I2C address */
#define TCS3430_DEFAULT_ADDR 0x39
/*=========================================================================*/

/*=========================================================================
    REGISTER ADDRESSES
    -----------------------------------------------------------------------*/
/** Enable states and interrupts */
#define TCS3430_REG_ENABLE 0x80
/** ADC integration time */
#define TCS3430_REG_ATIME 0x81
/** ALS wait time */
#define TCS3430_REG_WTIME 0x83
/** ALS interrupt low threshold low byte */
#define TCS3430_REG_AILTL 0x84
/** ALS interrupt low threshold high byte */
#define TCS3430_REG_AILTH 0x85
/** ALS interrupt high threshold low byte */
#define TCS3430_REG_AIHTL 0x86
/** ALS interrupt high threshold high byte */
#define TCS3430_REG_AIHTH 0x87
/** ALS interrupt persistence filters */
#define TCS3430_REG_PERS 0x8C
/** Configuration register zero */
#define TCS3430_REG_CFG0 0x8D
/** Configuration register one */
#define TCS3430_REG_CFG1 0x90
/** Revision ID */
#define TCS3430_REG_REVID 0x91
/** Device ID */
#define TCS3430_REG_ID 0x92
/** Device status register */
#define TCS3430_REG_STATUS 0x93
/** Z CH0 ADC low byte register */
#define TCS3430_REG_CH0DATAL 0x94
/** Z CH0 ADC high byte register */
#define TCS3430_REG_CH0DATAH 0x95
/** Y CH1 ADC low byte register */
#define TCS3430_REG_CH1DATAL 0x96
/** Y CH1 ADC high byte register */
#define TCS3430_REG_CH1DATAH 0x97
/** IR1 CH2 ADC low byte register */
#define TCS3430_REG_CH2DATAL 0x98
/** IR1 CH2 ADC high byte register */
#define TCS3430_REG_CH2DATAH 0x99
/** X or IR2 CH3 ADC low byte register */
#define TCS3430_REG_CH3DATAL 0x9A
/** X or IR2 CH3 ADC high byte register */
#define TCS3430_REG_CH3DATAH 0x9B
/** Configuration register two */
#define TCS3430_REG_CFG2 0x9F
/** Configuration register three */
#define TCS3430_REG_CFG3 0xAB
/** Auto zero configuration */
#define TCS3430_REG_AZ_CONFIG 0xD6
/** Interrupt enables */
#define TCS3430_REG_INTENAB 0xDD
/*=========================================================================*/

/** Interrupt persistence values for PERS register */
typedef enum {
  TCS3430_PERS_EVERY = 0x0, ///< Every ALS cycle
  TCS3430_PERS_1 = 0x1,     ///< 1 consecutive ALS value out of range
  TCS3430_PERS_2 = 0x2,     ///< 2 consecutive ALS values out of range
  TCS3430_PERS_3 = 0x3,     ///< 3 consecutive ALS values out of range
  TCS3430_PERS_5 = 0x4,     ///< 5 consecutive ALS values out of range
  TCS3430_PERS_10 = 0x5,    ///< 10 consecutive ALS values out of range
  TCS3430_PERS_15 = 0x6,    ///< 15 consecutive ALS values out of range
  TCS3430_PERS_20 = 0x7,    ///< 20 consecutive ALS values out of range
  TCS3430_PERS_25 = 0x8,    ///< 25 consecutive ALS values out of range
  TCS3430_PERS_30 = 0x9,    ///< 30 consecutive ALS values out of range
  TCS3430_PERS_35 = 0xA,    ///< 35 consecutive ALS values out of range
  TCS3430_PERS_40 = 0xB,    ///< 40 consecutive ALS values out of range
  TCS3430_PERS_45 = 0xC,    ///< 45 consecutive ALS values out of range
  TCS3430_PERS_50 = 0xD,    ///< 50 consecutive ALS values out of range
  TCS3430_PERS_55 = 0xE,    ///< 55 consecutive ALS values out of range
  TCS3430_PERS_60 = 0xF     ///< 60 consecutive ALS values out of range
} tcs3430_pers_t;

/** ALS gain values for CFG1 register */
typedef enum {
  TCS3430_GAIN_1X = 0x0,  ///< 1x gain
  TCS3430_GAIN_4X = 0x1,  ///< 4x gain
  TCS3430_GAIN_16X = 0x2, ///< 16x gain
  TCS3430_GAIN_64X = 0x3, ///< 64x gain
  TCS3430_GAIN_128X = 0x4 ///< 128x gain (requires HGAIN bit set)
} tcs3430_gain_t;

/*!
 *    @brief  Class that stores state and functions for interacting with
 *            TCS3430 Color and ALS Sensor
 */
class Adafruit_TCS3430 {
 public:
  Adafruit_TCS3430();
  ~Adafruit_TCS3430();

  bool begin(uint8_t addr = TCS3430_DEFAULT_ADDR, TwoWire* theWire = &Wire);

  bool setIntegrationCycles(uint8_t cycles);
  uint8_t getIntegrationCycles();
  bool setIntegrationTime(float ms);
  float getIntegrationTime();

  bool setWaitCycles(uint8_t cycles);
  uint8_t getWaitCycles();
  bool setWaitTime(float ms);
  float getWaitTime();

  bool setALSThresholdLow(uint16_t threshold);
  uint16_t getALSThresholdLow();
  bool setALSThresholdHigh(uint16_t threshold);
  uint16_t getALSThresholdHigh();

  bool setInterruptPersistence(tcs3430_pers_t persistence);
  tcs3430_pers_t getInterruptPersistence();

  bool setWaitLong(bool enable);
  bool getWaitLong();

  bool setALSMUX_IR2(bool enable);
  bool getALSMUX_IR2();
  bool setALSGain(tcs3430_gain_t gain);
  tcs3430_gain_t getALSGain();

  bool isALSSaturated();
  bool clearALSSaturated();
  bool isALSInterrupt();
  bool clearALSInterrupt();

  bool getChannels(uint16_t* x, uint16_t* y, uint16_t* z, uint16_t* ir1);
  uint16_t getIR2();
  bool setInterruptClearOnRead(bool enable);
  bool getInterruptClearOnRead();
  bool setSleepAfterInterrupt(bool enable);
  bool getSleepAfterInterrupt();
  bool setAutoZeroMode(bool enable);
  bool getAutoZeroMode();
  bool setRunAutoZeroEveryN(uint8_t n);
  uint8_t getRunAutoZeroEveryN();
  bool enableSaturationInt(bool enable);
  bool enableALSInt(bool enable);

  bool waitEnable(bool enable);
  bool isWaitEnabled();
  bool ALSEnable(bool enable);
  bool isALSEnabled();
  bool powerOn(bool enable);
  bool isPoweredOn();

 private:
  Adafruit_I2CDevice* i2c_dev = NULL; ///< Pointer to I2C bus interface
};

#endif