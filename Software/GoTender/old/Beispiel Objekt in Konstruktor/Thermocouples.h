// ****************************************************************************
/// \file      ThermoCouples.h
///
/// \brief     Returns Termperature of Oven and Meat
///
/// \details    Read out thermocouple temperatures
///
/// \author    Christoph Capiaghi
///
/// \version   0.1
///
/// \date      20170509
/// 
/// \copyright Licensed under the Apache License, Version 2.0 (the "License");
/// you may not use this file except in compliance with the License.
/// You may obtain a copy of the License at
/// http://www.apache.org/licenses/LICENSE-2.0
///
/// \pre       
///
/// \bug       
///
/// \warning   
///
/// \todo     
///

// Define to prevent recursive inclusion
#ifndef __THERMOCOUPLES_H__
#define __THERMOCOUPLES_H__

// Include ********************************************************************
#include "Arduino.h"
#include "Adafruit_MAX31855.h"      // Thermocouple

// Exported types *************************************************************
/// \brief     Optional description
/// \details   Optional description (more detailed)


// Exported constants *********************************************************


const int8_t    MIN_TEMP_OVEN = -10; // For Checking the TC
const int8_t    MIN_TEMP_MEAT = -10;
const uint8_t   MAX_TEMP_OVEN = 300;
const uint8_t   MAX_TEMP_MEAT = 300;
const uint8_t   NUM_OF_TC_MEASUREMENTS = 5; // Number of measurements of the temperature (mean value)

// Exported macros ************************************************************

// Exported functions *********************************************************
class ThermoCouples
{
	public:
		ThermoCouples::ThermoCouples(const uint8_t spiCsOvenPin, const uint8_t spiCsMeatPin);
		~ThermoCouples();
			
		double 		getTemperatureOven();
		double 		getTemperatureMeat();
		double 		getMeanTemperatureOven();
		double 		getMeanTemperatureMeat();
		uint8_t		checkThermoCouples();
			
	private:
		Adafruit_MAX31855 thermocoupleOven;
		Adafruit_MAX31855 thermocoupleMeat;
		uint8_t 	_m_spiCsOvenPin;
		uint8_t 	_m_spiCsMeatPin;

		
};
#endif
