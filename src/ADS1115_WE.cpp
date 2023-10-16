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
* https://wolles-elektronikkiste.de/en/ads1115-a-d-converter-with-amplifier (English)
* https://wolles-elektronikkiste.de/ads1115 (German)
*
*******************************************/

#include "ADS1115_WE.h"

void ADS1115_WE::reset(){
#ifndef USE_TINY_WIRE_M_  
    _wire->beginTransmission(0);
    _wire->write(0x06);
    _wire->endTransmission();
#else
    TinyWireM.beginTransmission(0);
    TinyWireM.send(0x06);
    TinyWireM.endTransmission();
#endif
}

bool ADS1115_WE::init(){    
#ifndef USE_TINY_WIRE_M_
    _wire->beginTransmission(i2cAddress);
    uint8_t success = _wire->endTransmission();
#else
    TinyWireM.beginTransmission(i2cAddress);
    uint8_t success = TinyWireM.endTransmission();
#endif
    if(success){
        return 0;
    }
    writeRegister(ADS1115_CONFIG_REG, ADS1115_REG_RESET_VAL);
    setVoltageRange_mV(ADS1115_RANGE_2048);
    writeRegister(ADS1115_LO_THRESH_REG, 0x8000);
    writeRegister(ADS1115_HI_THRESH_REG, 0x7FFF);
    deviceMeasureMode = ADS1115_SINGLE;
    autoRangeMode = false;
    return 1;
}

void ADS1115_WE::setAlertPinMode(ADS1115_COMP_QUE mode){
    uint16_t currentConfReg = readRegister(ADS1115_CONFIG_REG);
    currentConfReg &= ~(0x8003);    
    currentConfReg |= mode;
    writeRegister(ADS1115_CONFIG_REG, currentConfReg);
}

void ADS1115_WE::setAlertLatch(ADS1115_LATCH latch){
    uint16_t currentConfReg = readRegister(ADS1115_CONFIG_REG);
    currentConfReg &= ~(0x8004);    
    currentConfReg |= latch;
    writeRegister(ADS1115_CONFIG_REG, currentConfReg);
}

void ADS1115_WE::setAlertPol(ADS1115_ALERT_POL polarity){
    uint16_t currentConfReg = readRegister(ADS1115_CONFIG_REG);
    currentConfReg &= ~(0x8008);    
    currentConfReg |= polarity;
    writeRegister(ADS1115_CONFIG_REG, currentConfReg);
}

void ADS1115_WE::setAlertModeAndLimit_V(ADS1115_COMP_MODE mode, float hiThres, float loThres){
    uint16_t currentConfReg = readRegister(ADS1115_CONFIG_REG);
    currentConfReg &= ~(0x8010);    
    currentConfReg |= mode;
    writeRegister(ADS1115_CONFIG_REG, currentConfReg);
    int16_t alertLimit = calcLimit(hiThres);
    writeRegister(ADS1115_HI_THRESH_REG, alertLimit);
    alertLimit = calcLimit(loThres);
    writeRegister(ADS1115_LO_THRESH_REG, alertLimit);
    
}

void ADS1115_WE::setConvRate(ADS1115_CONV_RATE rate){
    uint16_t currentConfReg = readRegister(ADS1115_CONFIG_REG);
    currentConfReg &= ~(0x80E0);    
    currentConfReg |= rate;
    writeRegister(ADS1115_CONFIG_REG, currentConfReg);
}

convRate ADS1115_WE::getConvRate(){
    uint16_t currentConfReg = readRegister(ADS1115_CONFIG_REG);
    return (convRate)(currentConfReg & 0xE0);
}
    
void ADS1115_WE::setMeasureMode(ADS1115_MEASURE_MODE mode){
    uint16_t currentConfReg = readRegister(ADS1115_CONFIG_REG);
    deviceMeasureMode = mode;
    currentConfReg &= ~(0x8100);    
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
    
    currentConfReg &= ~(0x8E00);    
    currentConfReg |= range;
    writeRegister(ADS1115_CONFIG_REG, currentConfReg);
    convRate rate = getConvRate();
    delayAccToRate(rate);
}

void ADS1115_WE::setAutoRange(){
    uint16_t currentConfReg = readRegister(ADS1115_CONFIG_REG);
    setVoltageRange_mV(ADS1115_RANGE_6144);
    
    if(deviceMeasureMode == ADS1115_SINGLE){
        setMeasureMode(ADS1115_CONTINUOUS);
        convRate rate = getConvRate();
        delayAccToRate(rate);
    }
    
    int16_t rawResult = abs(readRegister(ADS1115_CONV_REG));
    int16_t rawResultCopy = rawResult;
        if(rawResultCopy == -32768){
            rawResultCopy++; 
        }
        rawResultCopy = abs(rawResultCopy);
    
    range optRange = ADS1115_RANGE_6144;
    
    if(rawResultCopy < 1093){
        optRange = ADS1115_RANGE_0256;
    }
    else if(rawResultCopy < 2185){
        optRange = ADS1115_RANGE_0512;
    }
    else if(rawResultCopy < 4370){
        optRange = ADS1115_RANGE_1024;
    }
    else if(rawResultCopy < 8738){
        optRange = ADS1115_RANGE_2048;
    }
    else if(rawResultCopy < 17476){
        optRange = ADS1115_RANGE_4096;
    }
    
    writeRegister(ADS1115_CONFIG_REG, currentConfReg);
    setVoltageRange_mV(optRange); 
}

void ADS1115_WE::setPermanentAutoRangeMode(bool autoMode){
    if(autoMode){
        autoRangeMode = true;
    }
    else{
        autoRangeMode = false;
    }
}
        
void ADS1115_WE::delayAccToRate(convRate cr){
    switch(cr){
        case ADS1115_8_SPS:
            delay(130);
            break;
        case ADS1115_16_SPS:
            delay(65);
            break;
        case ADS1115_32_SPS:
            delay(32);
            break;
        case ADS1115_64_SPS:
            delay(16);
            break;
        case ADS1115_128_SPS:
            delay(8);
            break;
        case ADS1115_250_SPS:
            delay(4);
            break;
        case ADS1115_475_SPS:
            delay(3);
            break;
        case ADS1115_860_SPS:
            delay(2);
            break;
    }
}
    
void ADS1115_WE::setCompareChannels(ADS1115_MUX mux){
    uint16_t currentConfReg = readRegister(ADS1115_CONFIG_REG);
    currentConfReg &= ~(0xF000);    
    currentConfReg |= (mux);
    writeRegister(ADS1115_CONFIG_REG, currentConfReg);
    
    if(!(currentConfReg & 0x0100)){  // => if not single shot mode
        convRate rate = getConvRate();      
        delayAccToRate(rate);
        delayAccToRate(rate);               
    }       
}

void ADS1115_WE::setCompareChannels_nonblock(ADS1115_MUX mux){
    uint16_t currentConfReg = readRegister(ADS1115_CONFIG_REG);
    currentConfReg &= ~(0xF000);    
    currentConfReg |= (mux);
    writeRegister(ADS1115_CONFIG_REG, currentConfReg);
}

void ADS1115_WE::setSingleChannel(size_t channel) {
    if (channel >=  4)
        return;
    setCompareChannels((ADS1115_MUX)(ADS1115_COMP_0_GND + ADS1115_COMP_INC*channel));
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
    float result = getResult_mV();
    result /= 1000;
    return result;  
}

float ADS1115_WE::getResult_mV(){
    int16_t rawResult = getRawResult();
    float result = (rawResult * 1.0 / ADS1115_REG_FACTOR) * voltageRange;
    return result;
}

int16_t ADS1115_WE::getRawResult(){
    int16_t rawResult = readRegister(ADS1115_CONV_REG);
    if(autoRangeMode){
        int16_t rawResultCopy = rawResult;
        if(rawResultCopy == -32768){
            rawResultCopy++; 
        }
        rawResultCopy = abs(rawResultCopy);
        if((rawResultCopy > 26214) && (voltageRange != 6144)){ // 80%
            setAutoRange();
            rawResult = readRegister(ADS1115_CONV_REG);
        }
        else if((rawResultCopy < 9800) && (voltageRange != 256)){ //30%
            setAutoRange();
            rawResult = readRegister(ADS1115_CONV_REG);
        }
    }
    return rawResult;
}

int16_t ADS1115_WE::getResultWithRange(int16_t min, int16_t max){
    int16_t rawResult = getRawResult();
    int16_t result = map(rawResult, -32768, 32767, min, max);
    return result;
}

int16_t ADS1115_WE::getResultWithRange(int16_t min, int16_t max, int16_t maxMillivolt){
    int16_t result = getResultWithRange(min, max);
    result = static_cast<int16_t>((1.0 * result * voltageRange / maxMillivolt) + 0.5);
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
    int16_t limit = static_cast<int16_t>((rawLimit * ADS1115_REG_FACTOR / voltageRange)*1000);
    return limit;
}

uint8_t ADS1115_WE::writeRegister(uint8_t reg, uint16_t val){
    uint8_t lVal = val & 255;
    uint8_t hVal = val >> 8;
#ifndef USE_TINY_WIRE_M_  
    _wire->beginTransmission(i2cAddress);
    _wire->write(reg);
    _wire->write(hVal);
    _wire->write(lVal);
    return _wire->endTransmission();
#else
    TinyWireM.beginTransmission(i2cAddress);
    TinyWireM.send(reg);
    TinyWireM.send(hVal);
    TinyWireM.send(lVal);
    return TinyWireM.endTransmission();
#endif
  
}
  
uint16_t ADS1115_WE::readRegister(uint8_t reg){
    uint8_t MSByte = 0, LSByte = 0;
    uint16_t regValue = 0;
#ifndef USE_TINY_WIRE_M_    
    _wire->beginTransmission(i2cAddress);
    _wire->write(reg);
    _wire->endTransmission(false);
    _wire->requestFrom(i2cAddress,static_cast<uint8_t>(2));
    if(_wire->available()){
        MSByte = _wire->read();
        LSByte = _wire->read();
    }
#else
    TinyWireM.beginTransmission(i2cAddress);
    TinyWireM.send(reg);
    TinyWireM.endTransmission();
    TinyWireM.requestFrom(i2cAddress,static_cast<uint8_t>(2));
    MSByte = TinyWireM.receive();
    LSByte = TinyWireM.receive();
#endif
    regValue = (MSByte<<8) + LSByte;
    return regValue;
}
