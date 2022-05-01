/***************************************************************************
* Example sketch for the ADS1115_WE library
*
* This sketch shows how to use the ADS1115 module with the TinyWireM library:
* https://github.com/adafruit/TinyWireM
* This allows you to run the ADS1115_WE library on an ATtiny85, for example.
* 
* For this specific sketch you also need library Tiny4KOLED to display the 
* measured values on a SSD1306 OLED display:
* https://github.com/datacute/Tiny4kOLED
*  
* Further information can be found on:
* https://wolles-elektronikkiste.de/ads1115 (German)
* https://wolles-elektronikkiste.de/en/ads1115-a-d-converter-with-amplifier (English)
* 
***************************************************************************/

/* !!!! IN ORDER TO USE TinyWireM YOU NEED TO UNCOMMENT #define USE_TINY_WIRE_M_   !!!!
 * !!!! IN ADS1115_config.h WHICH YOU FIND IN THE libraries/ADS1115_WE/src FOLDER  !!!!                                                       !!!!
 */
#include <TinyWireM.h>
#include <Tiny4kOLED.h>
#include<ADS1115_WE.h> 
#define ADS1115_I2C_ADDR 0x48
uint8_t width = 128;
uint8_t height = 64;

/* There are two ways to create your ADS1115_WE object:
 * ADS1115_WE adc = ADS1115_WE()             -> uses Wire / I2C Address = 0x48
 * ADS1115_WE adc = ADS1115_WE(I2C_ADDRESS)  -> uses Wire / I2C_ADDRESS
 */
ADS1115_WE adc = ADS1115_WE(ADS1115_I2C_ADDR);

void setup() {
  TinyWireM.begin();
  oled.begin(width, height, sizeof(tiny4koled_init_128x64br), tiny4koled_init_128x64br);
  oled.clear();
  oled.setFont(FONT6X8);
  oled.on();
  
  if(!adc.init()){
     oled.print("ADS1115 not connected");
     while(1){}
  }
  else{
    oled.print("ADS1115 connected");
    delay(1000);
    oled.clear();
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
  // adc.setConvRate(ADS1115_8_SPS); //uncomment if you want to change the default

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

  adc.setCompareChannels(ADS1115_COMP_0_GND);
  voltage = adc.getResult_V();
  oled.setCursor(0,0);
  oled.print("Channel 0 [V]: ");
  oled.print(voltage);
  oled.clearToEOL();

  adc.setCompareChannels(ADS1115_COMP_1_GND);
  voltage = adc.getResult_V();
  oled.setCursor(0,2);
  oled.print("Channel 1 [V]: ");
  oled.print(voltage);
  oled.clearToEOL();

  adc.setCompareChannels(ADS1115_COMP_2_GND);
  voltage = adc.getResult_V();
  oled.setCursor(0,4);
  oled.print("Channel 2 [V]: ");
  oled.print(voltage);
  oled.clearToEOL();

  adc.setCompareChannels(ADS1115_COMP_3_GND);
  voltage = adc.getResult_V();
  oled.setCursor(0,6);
  oled.print("Channel 3 [V]: ");
  oled.print(voltage);
  oled.clearToEOL();
  
  delay(2000);
}
