# ADS1115_WE

An Arduino library for the 16-bit, 4-channel ADS1115 and the 12-Bit, 4-channel ADS1015 ADC with gain and alert functions. 

I have have tried to optimize the library for convenience to use. If you try the examples I recommend to start with `Single_Shot.ino`.

You can find more details here:

https://wolles-elektronikkiste.de/ads1115 (German)

https://wolles-elektronikkiste.de/en/ads1115-a-d-converter-with-amplifier (English)

All features of the ADS1115 and ADS1015 are implemented, including alert functions. 

The examples are written for the ADS1115 with one exception, which is Continuous_ADS1015.ino. This shows how to "translate" the sketches 
for the ADS1015. Most enum values like ADS1115_RANGE_6144 and ADS1015_RANGE_6144 are even identical. The exceptions are the enum values for 
the conversion rate.

In version 1.4.1 I have implemented the option to use TinyWireM instead of Wire. Therefore the library can be used, for example, on 
an ATtiny85.

Since version 1.3.0 I have added a feature to the continuous mode, which ensures that you can change channels safely without risking that the
first data read is still from the former channel. If you experienced this issue, you might have solved it with a delay. If this applies to 
you, you can delete the delays. 

If you like the library it would be cool if you can give it a star. If you find bugs, please inform me. 

<h2>Beware of fake modules</h2>

There are ADS1115 modules which use ADS1015 ICs and also there are ADS1015 modules which are based on ADS1115 ICs. In theory you should 
recognize the IC by its label which is "BRPI" for the ADS1015 and "BOGI" for the ADS1115. But I have even found ADS1115 ICs labeled with 
"BRPI" which is definitely a fake. The difference between the ADS1115 and the ADS1015 is a) the 16-bit vs. 12-bit resolution an b) the speed. 

If you want to find out what you really have on on your module, then try the example sketch "Who_Am_I.ino". Do not change anything apart from
the I2C address if necessary.


