/***************************************************************************
* Example sketch for the ADS1115_WE library
*
* This sketch shows how to obtain results using different scales / formats.
*  
* Further information can be found on:
* https://wolles-elektronikkiste.de/ads1115
* 
***************************************************************************/

#include <ADS1115_WE.h> 
#include <Wire.h>
#define I2C_ADDRESS 0x48

ADS1115_WE adc(I2C_ADDRESS);

void setup() {
  Wire.begin();
  Serial.begin(9600);
  if(!adc.init()){
    Serial.println("ADS1115 not connected!");
  }

  adc.setVoltageRange_mV(ADS1115_RANGE_6144); //comment line/change parameter to change range
  adc.setCompareChannels(ADS1115_COMP_0_GND); //comment line/change parameter to change channel
  adc.setMeasureMode(ADS1115_CONTINUOUS); //comment line/change parameter to change mode

  Serial.println("ADS1115 Example Sketch - Results in different scales / formats");
  Serial.println("All results are for channel 0 vs. GND");
  Serial.println();
}

void loop() {
  
  /* Output in Volt or in Millivolt */
  
  float voltageInMillivolt = adc.getResult_mV(); 
  Serial.print("Result in Millivolt          [mV]: ");
  Serial.println(voltageInMillivolt);

  float voltageInVolt = adc.getResult_V(); 
  Serial.print("Result in Volt                [V]: ");
  Serial.println(voltageInVolt);

  /* Get the raw result from the conversion register. The conversion register 
   * contains the conversion result of the amplified (!) voltage. This means the
   * value depends on the voltage as well as on the voltage range. E.g. if the 
   * voltage range is 6.144 mV (ADS1115_RANGE_6144), +32767 is 6.144 mV; if the 
   * range is 4.096 mV, +32767 is 4.096 mV, and so on.  
  */
  int rawResult = adc.getRawResult();
  Serial.print("Raw Result                       : ");
  Serial.println(rawResult);

  /* Scaling of the result to a different range: 
   * The results in the conversion register are in a range of -32767 to +32767
   * You might want to receive the result in a different scale, e.g. -1023 to 1023.
   * For -1023 to 1023, and if you have chosen e.g. ADS1115_RANGE_4096, 0 Volt would 
   * give 0 as result and 4.096 mV would give 1023. -4.096 mV would give -1023.
  */
  int scaledResult = adc.getResultWithRange(-1023, 1023);
  Serial.print("Scaled result                    : ");
  Serial.println(scaledResult);

  /* Scaling of the result to a different range plus scaling to a voltage range: 
   * You can use this variant if you also want to scale to a voltage range. E.g. in
   * in order to get results equivalent to an Arduino UNO (10 bit, 5000 mV range), you 
   * would choose getResultWithRange(-1023, 1023, 5000). A difference to the Arduino 
   * UNO is that you can measure negative voltages. 
   * You have to ensure that the voltage range you scale to is smaller than the 
   * measuring voltage range. For this example only ADS1115_RANGE_6144 would cover the 
   * scale up to 5000 mV. 
  */
  int scaledResultWithMaxVoltage = adc.getResultWithRange(-1023, 1023, 5000); 
  Serial.print("Scaled result with voltage scale : ");
  Serial.println(scaledResultWithMaxVoltage);

  /* This function returns the voltage range ADS1115_RANGE_XXXX in Millivolt */
  unsigned int voltRange = adc.getVoltageRange_mV();
  Serial.print("Voltage Range of ADS1115     [mV]: ");
  Serial.println(voltRange);
  
  Serial.println("-------------------------------");
  delay(2000);
}
