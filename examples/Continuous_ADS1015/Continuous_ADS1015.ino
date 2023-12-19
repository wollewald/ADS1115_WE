/***************************************************************************
* Example sketch for the ADS1015_WE library
*
* This sketch shows how to use the ADS1015 in continuous mode. 
*  
* Further information can be found on:
* https://wolles-elektronikkiste.de/ads1115 (German)
* https://wolles-elektronikkiste.de/en/ads1115-a-d-converter-with-amplifier (English)
* 
***************************************************************************/

#include<ADS1015_WE.h> 
#include<Wire.h>
#define I2C_ADDRESS 0x48

/* There are several ways to create your ADS1015_WE object:
 * ADS1015_WE adc = ADS1015_WE(); -> uses Wire / I2C Address = 0x48
 * ADS1015_WE adc = ADS1015_WE(I2C_ADDRESS); -> uses Wire / I2C_ADDRESS
 * ADS1015_WE adc = ADS1015_WE(&Wire); -> you can pass any TwoWire object / I2C Address = 0x48
 * ADS1015_WE adc = ADS1015_WE(&Wire, I2C_ADDRESS); -> all together
 */
ADS1015_WE adc = ADS1015_WE(I2C_ADDRESS);

void setup() {
  bool useADS1015 = true;
  Wire.begin();
  Serial.begin(9600);
  if(!adc.init(useADS1015)){ // passing true will tell the lib that an ADS1015 is used
    Serial.println("ADS1015 not connected!");
  }

  /* Set the voltage range of the ADC to adjust the gain
   * Please note that you must not apply more than VDD + 0.3V to the input pins!
   * 
   * ADS1015_RANGE_6144  ->  +/- 6144 mV
   * ADS1015_RANGE_4096  ->  +/- 4096 mV
   * ADS1015_RANGE_2048  ->  +/- 2048 mV (default)
   * ADS1015_RANGE_1024  ->  +/- 1024 mV
   * ADS1015_RANGE_0512  ->  +/- 512 mV
   * ADS1015_RANGE_0256  ->  +/- 256 mV
   */
  adc.setVoltageRange_mV(ADS1015_RANGE_6144); //comment line/change parameter to change range

  /* Set the inputs to be compared
   *  
   *  ADS1015_COMP_0_1    ->  compares 0 with 1 (default)
   *  ADS1015_COMP_0_3    ->  compares 0 with 3
   *  ADS1015_COMP_1_3    ->  compares 1 with 3
   *  ADS1015_COMP_2_3    ->  compares 2 with 3
   *  ADS1015_COMP_0_GND  ->  compares 0 with GND
   *  ADS1015_COMP_1_GND  ->  compares 1 with GND
   *  ADS1015_COMP_2_GND  ->  compares 2 with GND
   *  ADS1015_COMP_3_GND  ->  compares 3 with GND
   */
  adc.setCompareChannels(ADS1015_COMP_0_GND); //comment line/change parameter to change channel

  /* Set number of conversions after which the alert pin asserts
   * - or you can disable the alert 
   *  
   *  ADS1015_ASSERT_AFTER_1  -> after 1 conversion
   *  ADS1015_ASSERT_AFTER_2  -> after 2 conversions
   *  ADS1015_ASSERT_AFTER_4  -> after 4 conversions
   *  ADS1015_DISABLE_ALERT   -> disable comparator / alert pin (default) 
   */
  //adc.setAlertPinMode(ADS1015_ASSERT_AFTER_1); //uncomment if you want to change the default

  /* Set the conversion rate in SPS (samples per second)
   * Options should be self-explaining: 
   * 
   *  ADS1015_128_SPS 
   *  ADS1015_250_SPS  
   *  ADS1015_490_SPS 
   *  ADS1015_920_SPS  
   *  ADS1015_1600_SPS (default)
   *  ADS1015_2400_SPS 
   *  ADS1015_3300_SPS 
   */
  //adc.setConvRate(ADS1015_3300_SPS); //uncomment if you want to change the default

  /* Set continuous or single shot mode:
   * 
   *  ADS1015_CONTINUOUS  ->  continuous mode
   *  ADS1015_SINGLE     ->  single shot mode (default)
   */
  adc.setMeasureMode(ADS1015_CONTINUOUS); //comment line/change parameter to change mode

   /* Choose maximum limit or maximum and minimum alert limit (window) in Volt - alert pin will 
   *  assert when measured values are beyond the maximum limit or outside the window 
   *  Upper limit first: setAlertLimit_V(MODE, maximum, minimum)
   *  In max limit mode the minimum value is the limit where the alert pin assertion will be  
   *  cleared (if not latched)  
   * 
   *  ADS1015_MAX_LIMIT
   *  ADS1015_WINDOW
   * 
   */
  //adc.setAlertModeAndLimit_V(ADS1015_MAX_LIMIT, 3.0, 1.5); //uncomment if you want to change the default
  
  /* Enable or disable latch. If latch is enabled the alert pin will assert until the
   * conversion register is read (getResult functions). If disabled the alert pin assertion will be
   * cleared with next value within limits. 
   *  
   *  ADS1015_LATCH_DISABLED (default)
   *  ADS1015_LATCH_ENABLED
   */
  //adc.setAlertLatch(ADS1015_LATCH_ENABLED); //uncomment if you want to change the default

  /* Sets the alert pin polarity if active:
   *  
   * ADS1015_ACT_LOW  ->  active low (default)   
   * ADS1015_ACT_HIGH ->  active high
   */
  //adc.setAlertPol(ADS1015_ACT_LOW); //uncomment if you want to change the default
 
  /* With this function the alert pin will assert, when a conversion is ready.
   * In order to deactivate, use the setAlertLimit_V function  
   */
  //adc.setAlertPinToConversionReady(); //uncomment if you want to change the default

  Serial.println("ADS1015 Example Sketch - Continuous Mode");
  Serial.println("All values in volts");
  Serial.println();
}

  /* If you change the compare channels you can immediately read values from the conversion 
   * register, although they might belong to the former channel if no precautions are taken. 
   * It takes about the time needed for two conversions to get the correct data. In single 
   * shot mode you can use the isBusy() function to wait for data from the new channel. This 
   * does not work in continuous mode. 
   * To solve this issue the library adds a delay after change of channels if you are in contunuous
   * mode. The length of the delay is adjusted to the conversion rate. But be aware that the output 
   * rate will be much lower that the conversion rate if you change channels frequently. 
   */

void loop() {
  float voltage = 0.0;

  Serial.print("0: ");
  voltage = readChannel(ADS1015_COMP_0_GND);
  Serial.print(voltage);

  Serial.print(",   1: ");
  voltage = readChannel(ADS1015_COMP_1_GND);
  Serial.print(voltage);
  
  Serial.print(",   2: ");
  voltage = readChannel(ADS1015_COMP_2_GND);
  Serial.print(voltage);

  Serial.print(",   3: ");
  voltage = readChannel(ADS1015_COMP_3_GND);
  Serial.println(voltage);

  delay(1000);
}

float readChannel(ADS1015_MUX channel) {
  float voltage = 0.0;
  adc.setCompareChannels(channel);
  voltage = adc.getResult_V(); // alternative: getResult_mV for Millivolt
  return voltage;
}
