/***************************************************************************
* Example sketch for the ADS1115_WE library
*
* This sketch shows how you can use the alert pin as conversion ready alert pin.
* It works in continuous mode as well as in single shot mode. 
* Please note that you need to enable the alert pin with setAlertPinMode. Choose any 
* parameter except ADS1115_DISABLE_ALERT.
*  
* Further information can be found on:
* https://wolles-elektronikkiste.de/ads1115 (German)
* https://wolles-elektronikkiste.de/en/ads1115-a-d-converter-with-amplifier (English)
* 
***************************************************************************/

#include<ADS1115_WE.h> 
#include<Wire.h>
#define I2C_ADDRESS 0x48
int interruptPin = 2;
volatile bool convReady = false;

ADS1115_WE adc(I2C_ADDRESS);
// ADS1115_WE adc = ADS1115_WE(); // Alternative: uses default address 0x48

void setup() {
  Wire.begin();
  Serial.begin(9600);
  pinMode(interruptPin, INPUT_PULLUP);
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
  adc.setCompareChannels(ADS1115_COMP_0_GND); //comment line/change parameter to change channels

  /* Set number of conversions out of limit after which the alert pin will assert
   * - or you can disable the alert (including conversion ready alert)
   *  
   *  ADS1115_ASSERT_AFTER_1  -> after 1 conversion
   *  ADS1115_ASSERT_AFTER_2  -> after 2 conversions
   *  ADS1115_ASSERT_AFTER_4  -> after 4 conversions
   *  ADS1115_DISABLE_ALERT   -> disable comparator / alert pin (default) 
   */
  adc.setAlertPinMode(ADS1115_ASSERT_AFTER_1); //needed in this sketch to enable alert pin (doesn't matter if you choose after 1,2 or 4)

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
  adc.setConvRate(ADS1115_8_SPS); //comment line/change parameter to change SPS

  /* Set continuous or single shot mode:
   * 
   *  ADS1115_CONTINUOUS  ->  continuous mode
   *  ADS1115_SINGLE     ->  single shot mode (default)
   */
  //adc.setMeasureMode(ADS1115_CONTINUOUS); // the conversion ready alert pin also works in continuous mode

   /* Choose maximum limit or maximum and minimum alert limit (window) in volts - alert pin will 
   *  assert when measured values are beyond the maximum limit or outside the window 
   *  Upper limit first: setAlertLimit_V(MODE, maximum, minimum)
   *  In max limit mode the minimum value is the limit where the alert pin assertion will be cleared
   *  (if not latched)  
   * 
   *  ADS1115_MAX_LIMIT
   *  ADS1115_WINDOW
   * 
   */
  //adc.setAlertModeAndLimit_V(ADS1115_MAX_LIMIT, 3.0, 1.5); //uncomment if you want to change the default
  
  /* Enable or disable latch. If latch is enabled the alert pin will assert until the
   * conversion register is read (getResult functions). If disabled the alert pin assertion
   * will be cleared with next value within limits. 
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
 
  /* With this function the alert pin assert, when a conversion is ready.
   * In order to deactivate, use the setAlertLimit_V function  
   */
  adc.setAlertPinToConversionReady(); //needed for this sketch

  Serial.println("ADS1115 Example Sketch - Single Shot, Conversion Ready Alert Pin controlled");
  Serial.println();
  attachInterrupt(digitalPinToInterrupt(interruptPin), convReadyAlert, FALLING);
  adc.startSingleMeasurement();
}

  /* In this example I measure 32 times before the result is output. This is only to slow down 
   * the output rate. I want to show that the output rate is controlled by the conversion ready  
   * signals and not by a delay.
   */
void loop() {
  float voltage = 0.0;
  static int counter = 0;
  if(convReady){
    counter++;
    convReady = false;
    if(counter==32){  // counter is 32, conversion rate is 8 SPS --> 4s
      voltage = adc.getResult_V(); // alternative: getResult_mV for Millivolt
      Serial.print("Channel 0 vs GND [V]: ");
      Serial.println(voltage);
      Serial.println("-------------------------------");
      counter = 0;
    }
    adc.startSingleMeasurement();   
  }
}

void convReadyAlert(){
   convReady = true;
}
