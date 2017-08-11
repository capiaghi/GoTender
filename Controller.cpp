// ****************************************************************************
/// \file      Controller.cpp
///
/// \brief     Main Controller for the heater
///
/// \details   Controller for the run state
///
/// \author    Christoph Capiaghi / Anne Liebold
///
/// \version   0.2
///
/// \date      20170808
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
static uint8_t smokerEnable = 0;
static uint8_t setSmokerState = 0;

static uint8_t heaterEnable = 0;
static uint8_t setHeaterState = 0;


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
/// \detail    Enables the smoker if the smoker was set
/// \warning   After this command, the smoker is on!
/// \return    
/// \todo      
///
void enableSmoker()
{
   if(setSmokerState)
   {
      smokerEnable = TRUE;
   }
   
}

// ----------------------------------------------------------------------------
/// \brief     Disable smoker
/// \detail    
/// \warning   
/// \return    
/// \todo      
///
void disableSmoker()
{
	smokerEnable = FALSE;
}

// ----------------------------------------------------------------------------
/// \brief     Get smoker state
/// \detail    
/// \warning   
/// \return    uint8_t smoker state: 0: Disabled, 1: Enabled
/// \todo      
///
uint8_t getSmokerState()
{
   return setSmokerState;

}


// ----------------------------------------------------------------------------
/// \brief     Set smoker
/// \detail    Sets the smoker flag.
/// \warning   This command "arms" the smoker. Call enableSmoker to start the smoker
/// \return    
/// \todo      
///
void setSmoker()
{
	setSmokerState = TRUE;
   
}

// ----------------------------------------------------------------------------
/// \brief     Reset smoker
/// \detail    Restes the smoker flag.
/// \warning   This command "disarms" the smoker.
/// \return    
/// \todo      
///
void resetSmoker()
{
	setSmokerState = FALSE;
   
}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%



// ----------------------------------------------------------------------------
/// \brief     Enable heater
/// \detail    Enables the heater if the heater was set
/// \warning   After this command, the heater is on!
/// \return    
/// \todo      
///
void enableHeater()
{
   if(setHeaterState)
   {
      heaterEnable = TRUE;
   }
   
}

// ----------------------------------------------------------------------------
/// \brief     Disable heater
/// \detail    
/// \warning   
/// \return    
/// \todo      
///
void disableHeater()
{
  heaterEnable = FALSE;
}

// ----------------------------------------------------------------------------
/// \brief     Get heater state
/// \detail    
/// \warning   
/// \return    uint8_t heater state: 0: Disabled, 1: Enabled
/// \todo      
///
uint8_t getHeaterState()
{
   return setHeaterState;

}


// ----------------------------------------------------------------------------
/// \brief     Set Heater
/// \detail    Sets the Heater flag.
/// \warning   This command "arms" the heater. Call enableHeater to start the heater
/// \return    
/// \todo      
///
void setHeater()
{
  setHeaterState = TRUE;
   
}

// ----------------------------------------------------------------------------
/// \brief     Reset heater
/// \detail    Restes the heater flag.
/// \warning   This command "disarms" the heater.
/// \return    
/// \todo      
///
void resetHeater()
{
  setHeaterState = FALSE;
   
}


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%



//// ----------------------------------------------------------------------------
///// \brief     Enable header
///// \detail    Enables the heater
///// \warning   After this command, the heater is on!
///// \return    
///// \todo      
/////
//void enableHeater()
//{
//   if(setSmokerState)
//   {
//      heaterEnable = TRUE;
//   }
//   
//}
//
//// ----------------------------------------------------------------------------
///// \brief     Disable heater
///// \detail    
///// \warning   
///// \return    
///// \todo      
/////
//void disableHeater()
//{
//	heaterEnable = FALSE;
//}

