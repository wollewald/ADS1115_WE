/***************************************************************************
* Example sketch for the ADS1115_WE library
*
* This is an experimental sketch
*  
* Further information can be found on:
* https://wolles-elektronikkiste.de/ads1115
* 
***************************************************************************/

#include<ADS1115_WE.h> 
#include<Wire.h>
#define I2C_ADDRESS 0x48

ADS1115_WE adc(I2C_ADDRESS);
// ADS1115_WE adc = ADS1115_WE(); // Alternative: uses default address 0x48

void setup() {
  Wire.begin();
  Serial.begin(9600);
  if(!adc.init()){
    Serial.println("ADS1115 not connected!");
  }

  adc.setVoltageRange_mV(ADS1115_RANGE_1024); //comment line/change parameter to change range

  adc.setCompareChannels(ADS1115_COMP_0_1); //comment line/change parameter to change channel

  adc.setMeasureMode(ADS1115_CONTINUOUS); //comment line/change parameter to change mode

  Serial.println("ADS1115 Example Sketch - Eperimental Sketch to test getResultWithRange");
  Serial.println();
}

void loop() {
  float voltage = 0.0;
  int mappedResult = 0;
  voltage = adc.getResult_mV(); // alternative: getResult_mV for Millivolt
  mappedResult = adc.getResultWithRange(0, 255);
  Serial.print("Channel 0 vs GND [mV]: ");
  Serial.println(voltage);
  Serial.print("Mapped Result        : ");
  Serial.println(mappedResult);
  Serial.println("-------------------------------");
  delay(2000);
}
