// ****************************************************************************
/// \file      ThermoCouples.c
///
/// \brief     Returns Termperature of Oven and Meat
///
/// \details   Read out thermocouple temperatures
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
// ****************************************************************************
// Includes *******************************************************************
#include "ThermoCouples.h"

// Private constants **********************************************************


// Constructor
// What should happen when someone creates an instance of your class
ThermoCouples::ThermoCouples(const uint8_t spiCsOvenPin, const uint8_t spiCsMeatPin):
      thermocoupleOven(spiCsOvenPin),
      thermocoupleMeat(spiCsMeatPin)
      {}

// Destructor
ThermoCouples::~ThermoCouples()
{
	
}

// ----------------------------------------------------------------------------
/// \brief     Get temperature of the oven
/// \detail    
/// \warning   
/// \return    double temperature in celsius
/// \todo      
///
double ThermoCouples::getTemperatureOven()
{
	return thermocoupleOven.readCelsius();
}

// ----------------------------------------------------------------------------
/// \brief     Get temperature of the meat
/// \detail    
/// \warning   
/// \return    double temperature in celsius
/// \todo      
///
double ThermoCouples::getTemperatureMeat()
{
	return thermocoupleMeat.readCelsius();
}


// ----------------------------------------------------------------------------
/// \brief     Get mean temperature of the oven
/// \detail    Takes NUM_OF_TC_MEASUREMENTS Measurements and removes highest and lowest value.
///            Then the function calculates the mean value over the rest of
///            measurements.  
/// \warning   
/// \return    double temperature in celsius
/// \todo      
///
double ThermoCouples::getMeanTemperatureOven()
{
	double temperatureMean 	= 0;
	double temperature 		= 0;
	double temperatureLow 	= MAX_TEMP_OVEN;
	double temperatureHigh 	= MIN_TEMP_OVEN;
	
	for (int i = 0; i < (NUM_OF_TC_MEASUREMENTS + 2); i++) // +2: To remove max and min
	{
		temperature 		= getTemperatureOven();
		temperatureMean 	+= temperature;
	// Lowest Temperature
    if(temperature < temperatureLow)
    {
      temperatureLow = temperature;
    }
    
    // Highest Temperature
    if(temperature > temperatureHigh)
    {
      temperatureHigh = temperature;
    }
  }
  // Remove highest and lowest temperature and calculate mean value
  temperatureMean = (temperatureMean - temperatureHigh - temperatureLow) / NUM_OF_TC_MEASUREMENTS;
  return temperatureMean;
}

// ----------------------------------------------------------------------------
/// \brief     Get mean temperature of the meat
/// \detail    Takes NUM_OF_TC_MEASUREMENTS Measurements and removes highest and lowest value.
///            Then the function calculates the mean value over the rest of
///            measurements.  
/// \warning   
/// \return    double temperature in celsius
/// \todo      
///
double ThermoCouples::getMeanTemperatureMeat()
{
	double temperatureMean 	= 0;
	double temperature 		= 0;
	double temperatureLow 	= MAX_TEMP_OVEN;
	double temperatureHigh 	= MIN_TEMP_OVEN;
	
	for (int i = 0; i < (NUM_OF_TC_MEASUREMENTS + 2); i++) // +2: To remove max and min
	{
		temperature 		= getTemperatureMeat();
		temperatureMean 	+= temperature;
	// Lowest Temperature
    if(temperature < temperatureLow)
    {
      temperatureLow = temperature;
    }
    
    // Highest Temperature
    if(temperature > temperatureHigh)
    {
      temperatureHigh = temperature;
    }
  }
  // Remove highest and lowest temperature and calculate mean value
  temperatureMean = (temperatureMean - temperatureHigh - temperatureLow) / NUM_OF_TC_MEASUREMENTS;
  return temperatureMean;
}

// ----------------------------------------------------------------------------
/// \brief     Checks function of the thermoelemts
/// \detail    Simple Test of the thermoelemts: Connected? To High or to low
///            temperatures?
/// \warning   
/// \return    -
/// \todo      Add error code, print out at Display, Temperature > 500 degree
uint8_t ThermoCouples::checkThermoCouples()
{
  double temperaturOvenCelsius = getTemperatureOven();
  double temperaturMeatCelsius = getTemperatureMeat();
  
  if (isnan(temperaturOvenCelsius))
  {
    Serial.println("E1: Something wrong with thermocouple OVEN. Connected?");
	return 0;
  }
  
  if (isnan(temperaturMeatCelsius))
  {
    Serial.println("E2: Something wrong with thermocouple MEAT. Connected?");
	return 0;
  }

  if(temperaturOvenCelsius < MIN_TEMP_OVEN)
  {
    Serial.println("E3: Temperatur OVEN to low. Polarity correct?");
	return 0;
  }

  if(temperaturMeatCelsius < MIN_TEMP_MEAT)
  {
    Serial.println("E4: Temperatur MEAT to low. Polarity correct?");
	return 0;
  }

  if(temperaturOvenCelsius > MAX_TEMP_OVEN)
  {
    Serial.println("E5: Temperatur OVEN to high.");
	return 0;
  }

  if(temperaturMeatCelsius > MAX_TEMP_MEAT)
  {
    Serial.println("E6: Temperatur MEAT to high.");
	return 0;
  } 
  
  return 1;
  
}
