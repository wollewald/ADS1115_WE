/******************************************************************************
 *
 * This is a library for the ADS1115 A/D Converter
 *
 * You'll find several example sketches which should enable you to use the library. 
 *
 * You are free to use it, change it or build on it. In case you like it, it would
 * be cool if you give it a star.
 *
 * If you find bugs, please inform me!
 * 
 * Written by Wolfgang (Wolle) Ewald
 * https://wolles-elektronikkiste.de
 *
 * 
 ******************************************************************************/

#ifndef ADS1115_WE_H_
#define ADS1115_WE_H_

#if (ARDUINO >= 100)
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif

#include <Wire.h>

/* registers */
#define ADS1115_CONV_REG 		0x00 //Conversion Register
#define ADS1115_CONFIG_REG    	0x01 //Configuration Register
#define ADS1115_LO_THRESH_REG   0x02 //Low Threshold Register
#define ADS1115_HI_THRESH_REG   0x03 //High Threshold Register 

/* other */
#define ADS1115_REG_FACTOR 32768
#define ADS1115_REG_RESET_VAL 0x8583

typedef enum ADS1115_COMP_QUE {
	ADS1115_ASSERT_AFTER_1 = 0x0000,
	ADS1115_ASSERT_AFTER_2 = 0x0001,
	ADS1115_ASSERT_AFTER_4 = 0x0002,
	ADS1115_DISABLE_ALERT  = 0x0003
} compQue;

typedef enum ADS1115_LATCH {
	ADS1115_LATCH_DISABLED = 0x0000,
	ADS1115_LATCH_ENABLED  = 0x0004,
} latch;

typedef enum ADS1115_ALERT_POL {
	ADS1115_ACT_LOW  = 0x0000,   
	ADS1115_ACT_HIGH = 0x0008
} alertPol;

typedef enum ADS1115_COMP_MODE{
	ADS1115_MAX_LIMIT = 0x0000,
	ADS1115_WINDOW    = 0x0010
} compMode;

typedef enum ADS1115_CONV_RATE{
	ADS1115_8_SPS	= 0x0000,
	ADS1115_16_SPS	= 0x0020,
	ADS1115_32_SPS	= 0x0040,
	ADS1115_64_SPS	= 0x0050,
	ADS1115_128_SPS	= 0x0080,
	ADS1115_250_SPS	= 0x00A0,
	ADS1115_475_SPS	= 0x00C0,
	ADS1115_860_SPS	= 0x00E0
} convRate;

typedef enum ADS1115_MEASURE_MODE{
	ADS1115_CONTINOUS = 0x0000,
	ADS1115_SINGLE    = 0x0100
} measureMode;

typedef enum ADS1115_RANGE{
	ADS1115_RANGE_6144	= 0x0000,
	ADS1115_RANGE_4096	= 0x0200,
	ADS1115_RANGE_2048  = 0x0400,
	ADS1115_RANGE_1024	= 0x0600,
	ADS1115_RANGE_0512	= 0x0800,
	ADS1115_RANGE_0256	= 0x0A00,
} range;

typedef enum ADS1115_MUX{
	ADS1115_COMP_0_1   = 0x0000,
	ADS1115_COMP_0_3   = 0x1000,
	ADS1115_COMP_1_3   = 0x2000,
	ADS1115_COMP_2_3   = 0x3000,
	ADS1115_COMP_0_GND = 0x4000,
	ADS1115_COMP_1_GND = 0x5000,
	ADS1115_COMP_2_GND = 0x6000,
	ADS1115_COMP_3_GND = 0x7000
} mux;

typedef enum ADS1115_STATUS_OR_START{
	ADS1115_BUSY 		  = 0x0000,
	ADS1115_START_ISREADY = 0x8000
} statusOrStart;


class ADS1115_WE
{
public:	
	ADS1115_WE(int addr);
	ADS1115_WE();			//sets default I2C Address 0x48
  
	void reset();
	bool init();
	void setAlertPinMode(ADS1115_COMP_QUE mode);
	void setAlertLatch(ADS1115_LATCH latch);
	void setAlertPol(ADS1115_ALERT_POL polarity);
	void setAlertModeAndLimit_V(ADS1115_COMP_MODE mode, float hithres, float lothres);
	void setConvRate(ADS1115_CONV_RATE rate);
	void setMeasureMode(ADS1115_MEASURE_MODE mode);
	void setVoltageRange_mV(ADS1115_RANGE range);
	void setCompareChannels(ADS1115_MUX mux);
	bool isBusy();
	void startSingleMeasurement();
	float getResult_V();
	float getResult_mV();
	void setAlertPinToConversionReady();
	void clearAlert();
	
		
private:
	uint16_t voltageRange;
	ADS1115_MEASURE_MODE deviceMeasureMode;
	int i2cAddress;
	int16_t calcLimit(float rawLimit);
	uint8_t writeRegister(uint8_t reg, uint16_t val);
	uint16_t readRegister(uint8_t reg);

};

#endif

