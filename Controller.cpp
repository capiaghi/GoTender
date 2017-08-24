// ****************************************************************************
/// \file      Controller.cpp
///
/// \brief     Main Controller for the heater
///
/// \details   Controller for the run state
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
#include "Controller.h"

// Private constants **********************************************************

static double  temperatureOvenSetPoint = -1;
static double  temperatureMeatSetPoint = -1;


static bool    allOff = false;         // Emergency Off
static bool    smokerStateOn = false;  // Smoker off



// ----------------------------------------------------------------------------
/// \brief     Get temperature set point of the oven
/// \detail    
/// \warning   
/// \return    double temperature set point in celsius
/// \todo      
///
double getTemperatureOvenSetPoint()
{
	return temperatureOvenSetPoint;
}

// ----------------------------------------------------------------------------
/// \brief     Get temperature set point of the meat
/// \detail    
/// \warning   
/// \return    double temperature in celsius
/// \todo      
///
double getTemperatureMeatSetPoint()
{
	return temperatureMeatSetPoint;
}

// ----------------------------------------------------------------------------
/// \brief     Set temperature set point of the oven
/// \detail    
/// \warning   
/// \return    
/// \todo      
///
void setTemperatureOvenSetPoint(double val)
{
	temperatureOvenSetPoint = val;
}

// ----------------------------------------------------------------------------
/// \brief     Set temperature set point of the meat
/// \detail    
/// \warning   
/// \return    
/// \todo      
///
void setTemperatureMeatSetPoint(double val)
{
	temperatureMeatSetPoint = val;
}

// ----------------------------------------------------------------------------
/// \brief     Enable smoker
/// \detail    Enables smoker, if system is okay
/// \warning   After this command, the smoker is on!
/// \return    
/// \todo      
///
void startSmoker(bool start)
{
   if ( !allOff )
   {
      if (start)
      {
         digitalWrite(RELAIS_SMOKER, HIGH);
         smokerStateOn = true;
      }
      else
      {
         digitalWrite(RELAIS_SMOKER, LOW);
         smokerStateOn = false;
      }
   }
   else
   {
      digitalWrite(RELAIS_SMOKER, LOW);
      smokerStateOn = false;
   }
   
}

// ----------------------------------------------------------------------------
/// \brief     Returns smoker state
/// \detail    
/// \warning  
/// \return    bool: true on, false off
/// \todo      
///
bool getSmokerState( void )
{
   return smokerStateOn;
}


// ----------------------------------------------------------------------------
/// \brief     Enable heater
/// \detail    
/// \warning   After this command, the heater is on!
/// \return    
/// \todo      
///
void startHeater(bool start)
{
   if ( !allOff )
   {
      if(start)
      {
         digitalWrite(RELAIS_HEATER, HIGH);
      }
      else
      {
         digitalWrite(RELAIS_HEATER, LOW);
      }
   }
   else
   {
      digitalWrite(RELAIS_HEATER, LOW);
   }
         
}



// ----------------------------------------------------------------------------
/// \brief     Emergency off
/// \detail    
/// \warning   Disables heater and smoker
/// \return    
/// \todo      Testing
///

void emergencyOff( void )
{
   allOff = true;
}


