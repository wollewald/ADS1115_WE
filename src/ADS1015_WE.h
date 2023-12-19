/******************************************************************************
 *
 * This is a library for the ADS1115 and ADS1015 A/D Converter
 *
 * You'll find several example sketches which should enable you to use the library.
 *
 * You are free to use it, change it or build on it. In case you like it, it would
 * be cool if you give it a star.
 *
 * If you find bugs, please inform me!
 *
 * Written by Wolfgang (Wolle) Ewald
 * https://wolles-elektronikkiste.de/en/ads1115-a-d-converter-with-amplifier (English)
 * https://wolles-elektronikkiste.de/ads1115 (German)
 *
 *
 ******************************************************************************/

#ifndef ADS1015_WE_H
#define ADS1015_WE_H
#include <ADS1115_WE.h>

#if ARDUINO < 100
#include <WProgram.h>
#else
#include <Arduino.h>
#endif

class ADS1015_WE : public ADS1115_WE {
    public:
        using ADS1115_WE::ADS1115_WE;               
};

#endif
