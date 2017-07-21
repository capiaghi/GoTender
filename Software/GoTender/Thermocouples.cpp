// ****************************************************************************
/// \file      ThermoCouples.cpp
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

static Adafruit_MAX31855 thermocoupleOven(SPI_CS_TC_OVEN_PIN);
static Adafruit_MAX31855 thermocoupleMeat(SPI_CS_TC_MEAT_PIN);


// ----------------------------------------------------------------------------
/// \brief     Get temperature of the oven
/// \detail    
/// \warning   
/// \return    double temperature in celsius
/// \todo      
///
double getTemperatureOven()
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
double getTemperatureMeat()
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
double getMeanTemperatureOven()
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
double getMeanTemperatureMeat()
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
uint8_t checkThermoCouples()
{
   double temperaturOvenCelsius = getTemperatureOven();
   double temperaturMeatCelsius = getTemperatureMeat();
   char errorOutput[50];
  
   if (isnan(temperaturOvenCelsius))
   {
      sprintf(errorOutput,"E%d: Something wrong with thermocouple OVEN. Connected?",ERROR_CODE_TC_OVEN_NAN);
      Serial.println(errorOutput);
      return ERROR_CODE_TC_OVEN_NAN;
   }
  
  if (isnan(temperaturMeatCelsius))
  {
    Serial.println("E2: Something wrong with thermocouple MEAT. Connected?");
	return ERROR_CODE_TC_MEAT_NAN;
  }

  if(temperaturOvenCelsius < MIN_TEMP_OVEN)
  {
    Serial.println("E3: Temperatur OVEN to low. Polarity correct?");
	return ERROR_CODE_TC_OVEN_MIN;
  }

  if(temperaturMeatCelsius < MIN_TEMP_MEAT)
  {
    Serial.println("E4: Temperatur MEAT to low. Polarity correct?");
	return ERROR_CODE_TC_MEAT_MIN;
  }

  if(temperaturOvenCelsius > MAX_TEMP_OVEN)
  {
    Serial.println("E5: Temperatur OVEN to high.");
	return ERROR_CODE_TC_OVEN_MAX;
  }

  if(temperaturMeatCelsius > MAX_TEMP_MEAT)
  {
    Serial.println("E6: Temperatur MEAT to high.");
	return ERROR_CODE_TC_MEAT_MAX;
  } 
  
  return 1;
}
