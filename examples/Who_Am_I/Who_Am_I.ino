/***************************************************************************
* Example sketch for the ADS1115_WE library
*
* This sketch checks whether you have an ADS1115 or ADS1015 module. The last 
* four bits of raw values obtained from an ADS1015 should be zero. Connect A0
* to a voltage different from GND. The sketch also checks how much time is 
* needed to perform ten measurements at lowest data rate, which is 128 SPS for
* the ADS1015 and 8 SPS for the ADS1115. 
*  
* Further information can be found on:
* https://wolles-elektronikkiste.de/ads1115 (German)
* https://wolles-elektronikkiste.de/en/ads1115-a-d-converter-with-amplifier (English)
* 
***************************************************************************/

#include<ADS1115_WE.h> 
#include<Wire.h>
#define I2C_ADDRESS 0x48

ADS1115_WE adc = ADS1115_WE(I2C_ADDRESS);

void setup() {
  Wire.begin();
  Serial.begin(9600);
  if(!adc.init()){
    Serial.println("ADS1115 not connected!");
  }
  adc.setVoltageRange_mV(ADS1115_RANGE_6144); 
  adc.setCompareChannels(ADS1115_COMP_0_GND); 
  Serial.println("ADS1115/ADS1015 Example Sketch - Who am I");
  Serial.println("Performing 10 single ended conversions A0 vs. GND:");
  uint16_t checkSum = 0;
  for(int i=0; i<10; i++){
    adc.startSingleMeasurement();
    while(adc.isBusy()){}
    int16_t raw = adc.getRawResult();
    Serial.println(raw, BIN);
    checkSum += raw & 0xF;
  }
  Serial.println();
  Serial.print("Check Sum (Sum of the last 4 bits): ");
  Serial.println(checkSum);

  adc.setConvRate(ADS1115_8_SPS); // = ADS1015_128_SPS = 0x0000
  unsigned long startingTime = millis();
  for(int i=0; i<10; i++){
    adc.startSingleMeasurement();
    while(adc.isBusy()){}
  }
  unsigned long duration = millis() - startingTime;
  Serial.print("Time needed for 10 conversions at slowest sample rate [ms]: ");
  Serial.println(duration);
  Serial.println();

  if(checkSum && duration > 1000){
    Serial.println("I am an ADS1115!");
  }
  else if (!checkSum && duration < 1000){
    Serial.println("I am an ADS1015!");
  }
  else {
    Serial.println("Sorry, don't know who I am!");
  }
}

void loop() {}
