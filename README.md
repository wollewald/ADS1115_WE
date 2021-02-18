# ADS1115_WE

An Arduino library for the 16-bit, 4 channel ADS1115 ADC with gain and alert functions.

I have have tried to optimize the library for convenience to use. If you try the examples I recommend to start with `Single_Shot.ino`.

You can find more details here:

https://wolles-elektronikkiste.de/ads1115 (German)

https://wolles-elektronikkiste.de/en/ads1115-a-d-converter-with-amplifier (English)

All features of the ADS1115 are implemented, including alert functions. 

In version 1.3.0 I have added a feature to the continuous mode, which ensures that you can change channels safely without risking that the
first data read is still from the former channel. If you experienced this issue, you might have solved it with a delay. If this applies to 
you, you can delete the delays. 

It seems there are modules out there which do not have the full 16 bit resolution. It's not an issue of this library:
https://github.com/wollewald/ADS1115_WE/issues/15

If you like the library it would be cool if you can give it a star. If you find bugs, please inform me. 
