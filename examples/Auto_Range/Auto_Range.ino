/***************************************************************************
* Example sketch for the ADS1115_WE library
*
* This sketch shows how to use the Auto Range function of the AD1115_WE library.  
*  
* Further information can be found on:
* https://wolles-elektronikkiste.de/ads1115 (German)
* https://wolles-elektronikkiste.de/en/ads1115-a-d-converter-with-amplifier (English)
* 
***************************************************************************/

#include<ADS1115_WE.h> 
#include<Wire.h>
#define I2C_ADDRESS 0x48

/* There are several ways to create your ADS1115_WE object:
 * ADS1115_WE adc = ADS1115_WE()             -> uses Wire / I2C Address = 0x48
 * ADS1115_WE adc = ADS1115_WE(I2C_ADDRESS)  -> uses Wire / I2C_ADDRESS
 * ADS1115_WE adc = ADS1115_WE(&wire2)       -> uses the TwoWire object wire2 / I2C_ADDRESS
 * ADS1115_WE adc = ADS1115_WE(&wire2, I2C_ADDRESS) -> all together
 * Successfully tested with two I2C busses on an ESP32
 */
ADS1115_WE adc = ADS1115_WE(I2C_ADDRESS);

void setup() {
  Wire.begin();
  Serial.begin(9600);
  if(!adc.init()){
    Serial.println("ADS1115 not connected!");
  }

  /* Set the voltage range of the ADC to adjust the gain
   * Please note that you must not apply more than VDD + 0.3V to the input pins!
   * 
   * ADS1115_RANGE_6144  ->  +/- 6144 mV
   * ADS1115_RANGE_4096  ->  +/- 4096 mV
   * ADS1115_RANGE_2048  ->  +/- 2048 mV (default)
   * ADS1115_RANGE_1024  ->  +/- 1024 mV
   * ADS1115_RANGE_0512  ->  +/- 512 mV
   * ADS1115_RANGE_0256  ->  +/- 256 mV
   */
  adc.setVoltageRange_mV(ADS1115_RANGE_6144); //comment line/change parameter to change range

  /* Set the inputs to be compared
   *  
   *  ADS1115_COMP_0_1    ->  compares 0 with 1 (default)
   *  ADS1115_COMP_0_3    ->  compares 0 with 3
   *  ADS1115_COMP_1_3    ->  compares 1 with 3
   *  ADS1115_COMP_2_3    ->  compares 2 with 3
   *  ADS1115_COMP_0_GND  ->  compares 0 with GND
   *  ADS1115_COMP_1_GND  ->  compares 1 with GND
   *  ADS1115_COMP_2_GND  ->  compares 2 with GND
   *  ADS1115_COMP_3_GND  ->  compares 3 with GND
   */
  adc.setCompareChannels(ADS1115_COMP_0_GND); //comment line/change parameter to change channel

  /* Set number of conversions after which the alert pin asserts
   * - or you can disable the alert 
   *  
   *  ADS1115_ASSERT_AFTER_1  -> after 1 conversion
   *  ADS1115_ASSERT_AFTER_2  -> after 2 conversions
   *  ADS1115_ASSERT_AFTER_4  -> after 4 conversions
   *  ADS1115_DISABLE_ALERT   -> disable comparator / alert pin (default) 
   */
  //adc.setAlertPinMode(ADS1115_ASSERT_AFTER_1); //uncomment if you want to change the default

  /* Set the conversion rate in SPS (samples per second)
   * Options should be self-explaining: 
   * 
   *  ADS1115_8_SPS 
   *  ADS1115_16_SPS  
   *  ADS1115_32_SPS 
   *  ADS1115_64_SPS  
   *  ADS1115_128_SPS (default)
   *  ADS1115_250_SPS 
   *  ADS1115_475_SPS 
   *  ADS1115_860_SPS 
   */
   adc.setConvRate(ADS1115_64_SPS); //uncomment if you want to change the default

  /* Set continuous or single shot mode:
   * 
   *  ADS1115_CONTINUOUS  ->  continuous mode
   *  ADS1115_SINGLE     ->  single shot mode (default)
   */
  adc.setMeasureMode(ADS1115_CONTINUOUS); //comment line/change parameter to change mode

   /* Choose maximum limit or maximum and minimum alert limit (window) in Volt - alert pin will 
   *  assert when measured values are beyond the maximum limit or outside the window 
   *  Upper limit first: setAlertLimit_V(MODE, maximum, minimum)
   *  In max limit mode the minimum value is the limit where the alert pin assertion will be  
   *  cleared (if not latched)  
   * 
   *  ADS1115_MAX_LIMIT
   *  ADS1115_WINDOW
   * 
   */
  //adc.setAlertModeAndLimit_V(ADS1115_MAX_LIMIT, 3.0, 1.5); //uncomment if you want to change the default
  
  /* Enable or disable latch. If latch is enabled the alert pin will assert until the
   * conversion register is read (getResult functions). If disabled the alert pin assertion will be
   * cleared with next value within limits. 
   *  
   *  ADS1115_LATCH_DISABLED (default)
   *  ADS1115_LATCH_ENABLED
   */
  //adc.setAlertLatch(ADS1115_LATCH_ENABLED); //uncomment if you want to change the default

  /* Sets the alert pin polarity if active:
   *  
   * ADS1115_ACT_LOW  ->  active low (default)   
   * ADS1115_ACT_HIGH ->  active high
   */
  //adc.setAlertPol(ADS1115_ACT_LOW); //uncomment if you want to change the default
 
  /* With this function the alert pin will assert, when a conversion is ready.
   * In order to deactivate, use the setAlertLimit_V function  
   */
  //adc.setAlertPinToConversionReady(); //uncomment if you want to change the default

  /* Enable or disable permanent automatic range selection mode. If enabled, the range will
   * change if the measured values are outside of 30-80% of the maximum value of the current 
   * range.  
   * !!! Use EITHER this function once OR setAutoRange() whenever needed (see below) !!!
   */
  adc.setPermanentAutoRangeMode(true);

  Serial.println("ADS1115 Example Sketch - Continuous Mode with Auto Range");
  Serial.println();
}

void loop() {
  Serial.print("Channel 0 - ");
  readChannel(ADS1115_COMP_0_GND);
  
  Serial.print("Channel 1 - ");
  readChannel(ADS1115_COMP_1_GND);
    
  Serial.print("Channel 2 - ");
  readChannel(ADS1115_COMP_2_GND);
  
  Serial.print("Channel 3 - ");
  readChannel(ADS1115_COMP_3_GND);
  
  Serial.println("-------------------------------");
  delay(1000);
}

void readChannel(ADS1115_MUX channel) {
  float voltage = 0.0;
  adc.setCompareChannels(channel);

  /* setAutoRange() switches to the highest range (+/- 6144 mV), measures the current 
   * voltage and then switches to the lowest range where the current value is still 
   * below 80% of the maximum value of the range. The function is only suitable if you 
   * expect stable or slowly changing voltages. setAutoRange needs roughly the time you
   * would need for three conversions. 
   * If the ADS115 is in single shot mode, setAutoRange() will switch into continuous
   * mode to measure a value and switch back again.
   * !!! Use EITHER this function whenever needed OR setPermanentAutoRangeMode(true) once !!!
   */  
  //adc.setAutoRange(); //use either this or setPermanentAutoRangeMode(true)
  
  voltage = adc.getResult_V(); // alternative: getResult_mV for Millivolt
  printVoltageRange(); // this is just to show that the range is changing with changing voltages 
  Serial.println(voltage);
}
  
void printVoltageRange(){
  unsigned int voltageRange = adc.getVoltageRange_mV();
  Serial.print("Range: ");

  switch(voltageRange){
    case 6144:
      Serial.print("+/- 6144 mV, Voltage [V]: ");
      break;
    case 4096:
      Serial.print("+/- 4096 mV, Voltage [V]: ");
      break;
    case 2048:
      Serial.print("+/- 2048 mV, Voltage [V]: ");
      break;
    case 1024:
      Serial.print("+/- 1024 mV, Voltage [V]: ");
      break;
    case 512:
      Serial.print("+/- 512 mV, Voltage [V]: ");
      break;
    case 256:
      Serial.print("+/- 256 mV, Voltage [V]: ");
      break;
    default:
      Serial.println("Something went wrong");
  }
}
