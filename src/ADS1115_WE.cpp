/*****************************************
* This is a library for the ADS1115 A/D Converter
*
* You'll find an example which should enable you to use the library. 
*
* You are free to use it, change it or build on it. In case you like 
* it, it would be cool if you give it a star.
* 
* If you find bugs, please inform me!
* 
* Written by Wolfgang (Wolle) Ewald
* https://wolles-elektronikkiste.de
*
*******************************************/

#include "ADS1115_WE.h"

ADS1115_WE::ADS1115_WE(int addr){
	i2cAddress = addr;
}

ADS1115_WE::ADS1115_WE(){
	i2cAddress = 0x48;
}

void ADS1115_WE::reset(){
	Wire.beginTransmission(0);
	Wire.write(0x06);
	Wire.endTransmission();
}

bool ADS1115_WE::init(){	
	Wire.beginTransmission(i2cAddress);
	uint8_t success = Wire.endTransmission();
	if(success){
		return 0;
	}
	writeRegister(ADS1115_CONFIG_REG, ADS1115_REG_RESET_VAL);
	setVoltageRange_mV(ADS1115_RANGE_2048);
	writeRegister(ADS1115_LO_THRESH_REG, 0x8000);
	writeRegister(ADS1115_HI_THRESH_REG, 0x7FFF);
	return 1;
}

void ADS1115_WE::setAlertPinMode(ADS1115_COMP_QUE mode){
	uint16_t currentConfReg = readRegister(ADS1115_CONFIG_REG);
	currentConfReg &= ~(0x0003);	
	currentConfReg |= mode;
	writeRegister(ADS1115_CONFIG_REG, currentConfReg);
}

void ADS1115_WE::setAlertLatch(ADS1115_LATCH latch){
	uint16_t currentConfReg = readRegister(ADS1115_CONFIG_REG);
	currentConfReg &= ~(0x0004);	
	currentConfReg |= latch;
	writeRegister(ADS1115_CONFIG_REG, currentConfReg);
}

void ADS1115_WE::setAlertPol(ADS1115_ALERT_POL polarity){
	uint16_t currentConfReg = readRegister(ADS1115_CONFIG_REG);
	currentConfReg &= ~(0x0008);	
	currentConfReg |= polarity;
	writeRegister(ADS1115_CONFIG_REG, currentConfReg);
}

void ADS1115_WE::setAlertModeAndLimit_V(ADS1115_COMP_MODE mode, float hiThres, float loThres){
	uint16_t currentConfReg = readRegister(ADS1115_CONFIG_REG);
	currentConfReg &= ~(0x0010);	
	currentConfReg |= mode;
	writeRegister(ADS1115_CONFIG_REG, currentConfReg);
	int16_t alertLimit = calcLimit(hiThres);
	writeRegister(ADS1115_HI_THRESH_REG, alertLimit);
	alertLimit = calcLimit(loThres);
	writeRegister(ADS1115_LO_THRESH_REG, alertLimit);
	
}

void ADS1115_WE::setConvRate(ADS1115_CONV_RATE rate){
	uint16_t currentConfReg = readRegister(ADS1115_CONFIG_REG);
	currentConfReg &= ~(0x00E0);	
	currentConfReg |= rate;
	writeRegister(ADS1115_CONFIG_REG, currentConfReg);
}
		
void ADS1115_WE::setMeasureMode(ADS1115_MEASURE_MODE mode){
	uint16_t currentConfReg = readRegister(ADS1115_CONFIG_REG);
	currentConfReg &= ~(0x0100);	
	currentConfReg |= mode;
	writeRegister(ADS1115_CONFIG_REG, currentConfReg);
}

void ADS1115_WE::setVoltageRange_mV(ADS1115_RANGE range){
	uint16_t currentVoltageRange = voltageRange;
	uint16_t currentConfReg = readRegister(ADS1115_CONFIG_REG);
	uint16_t currentRange = (currentConfReg >> 9) & 7;
	uint16_t currentAlertPinMode = currentConfReg & 3;
	
	setMeasureMode(ADS1115_SINGLE);
	
	switch(range){
		case ADS1115_RANGE_6144:
			voltageRange = 6144;
			break;
		case ADS1115_RANGE_4096:
			voltageRange = 4096;
			break;
		case ADS1115_RANGE_2048:
			voltageRange = 2048;
			break;
		case ADS1115_RANGE_1024:
			voltageRange = 1024;
			break;
		case ADS1115_RANGE_0512:
			voltageRange = 512;
			break;
		case ADS1115_RANGE_0256:
			voltageRange = 256;
			break;
	}
	
	if ((currentRange != range) && (currentAlertPinMode != ADS1115_DISABLE_ALERT)){
		int16_t alertLimit = readRegister(ADS1115_HI_THRESH_REG);
		alertLimit = alertLimit * (currentVoltageRange * 1.0 / voltageRange);
		writeRegister(ADS1115_HI_THRESH_REG, alertLimit);
		
		alertLimit = readRegister(ADS1115_LO_THRESH_REG);
		alertLimit = alertLimit * (currentVoltageRange * 1.0 / voltageRange);
		writeRegister(ADS1115_LO_THRESH_REG, alertLimit);
	}
	
	currentConfReg &= ~(0x0E00);	
	currentConfReg |= range;
	writeRegister(ADS1115_CONFIG_REG, currentConfReg);
}

void ADS1115_WE::setCompareChannels(ADS1115_MUX mux){
	uint16_t currentConfReg = readRegister(ADS1115_CONFIG_REG);
	currentConfReg &= ~(0x7000);	
	currentConfReg |= (mux);
	writeRegister(ADS1115_CONFIG_REG, currentConfReg);	
}
	
bool ADS1115_WE::isBusy(){
	uint16_t currentConfReg = readRegister(ADS1115_CONFIG_REG);
	return (!(currentConfReg>>15) & 1);
}
	
void ADS1115_WE::startSingleMeasurement(){
	uint16_t currentConfReg = readRegister(ADS1115_CONFIG_REG);
	currentConfReg |= (1 << 15);
	writeRegister(ADS1115_CONFIG_REG, currentConfReg);
}
	
float ADS1115_WE::getResult_V(){
	int16_t rawResult = readRegister(ADS1115_CONV_REG);
	float result = (rawResult * 1.0 / ADS1115_REG_FACTOR) * voltageRange/1000;
	return result;	
}

float ADS1115_WE::getResult_mV(){
	int16_t rawResult = readRegister(ADS1115_CONV_REG);
	float result = (rawResult * 1.0 / ADS1115_REG_FACTOR) * voltageRange;
	return result;
}

int16_t ADS1115_WE::getRawResult(){
	int16_t rawResult = readRegister(ADS1115_CONV_REG);
	return rawResult;
}

int16_t ADS1115_WE::getResultWithRange(int16_t min, int16_t max){
	int16_t rawResult = readRegister(ADS1115_CONV_REG);
	int16_t result = 0;
	result = map(rawResult, -32767, 32767, min, max);
	return result;
}

int16_t ADS1115_WE::getResultWithRange(int16_t min, int16_t max, int16_t maxMillivolt){
	int16_t rawResult = readRegister(ADS1115_CONV_REG);
	int16_t result = 0;
	result = map(rawResult, -32767, 32767, min, max);
	result = (int16_t) ((1.0 * result * voltageRange / maxMillivolt) + 0.5);
	return result;
}

uint16_t ADS1115_WE::getVoltageRange_mV(){
	return voltageRange;
}

void ADS1115_WE::setAlertPinToConversionReady(){
	writeRegister(ADS1115_LO_THRESH_REG, (0<<15));
	writeRegister(ADS1115_HI_THRESH_REG, (1<<15));
}

void ADS1115_WE::clearAlert(){
	readRegister(ADS1115_CONV_REG);
}

/************************************************ 
	private functions
*************************************************/

int16_t ADS1115_WE::calcLimit(float rawLimit){
	int16_t limit = (int16_t)((rawLimit * ADS1115_REG_FACTOR / voltageRange)*1000);
	return limit;
}

uint8_t ADS1115_WE::writeRegister(uint8_t reg, uint16_t val){
  Wire.beginTransmission(i2cAddress);
  uint8_t lVal = val & 255;
  uint8_t hVal = val >> 8;
  Wire.write(reg);
  Wire.write(hVal);
  Wire.write(lVal);
  return Wire.endTransmission();
}
  
uint16_t ADS1115_WE::readRegister(uint8_t reg){
  uint8_t MSByte = 0, LSByte = 0;
  uint16_t regValue = 0;
  Wire.beginTransmission(i2cAddress);
  Wire.write(reg);
  Wire.endTransmission();
  Wire.requestFrom(i2cAddress,2);
  if(Wire.available()){
    MSByte = Wire.read();
    LSByte = Wire.read();
  }
  regValue = (MSByte<<8) + LSByte;
  return regValue;
}
	


