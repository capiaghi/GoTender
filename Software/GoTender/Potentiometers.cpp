// ****************************************************************************
/// \file      Potentiometers.cpp
///
/// \brief     Read out Potentiometers
///
/// \details   Read out Potentiometers and convert it to a temperature value (set point)
///
/// \author    Christoph Capiaghi
///
/// \version   0.1
///
/// \date      20170511
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
/// \todo      set point -> Soll- Wert, actual value -> Ist-Wert
///
// ****************************************************************************
// Includes *******************************************************************
#include "Potentiometers.h"


// Private constants **********************************************************
#define SCALING_FACTOR     ( 4 )    // Value from 0 ... 1023
                                    // Max. Temperature 256 °C -> /4

// ----------------------------------------------------------------------------
/// \brief     
/// \detail    
/// \warning   
/// \return    
/// \todo
uint16_t getPotentiometerOvenVal()
{
   uint16_t raw = getPotentiometerOvenValRAW();
   // Value from 0 ... 1023
   // Max. Temperature 256 °C -> /4
   
   return (raw / SCALING_FACTOR);
}

// ----------------------------------------------------------------------------
/// \brief     
/// \detail    
/// \warning   
/// \return    
/// \todo
uint16_t getPotentiometerMeatVal()
{
   int raw = getPotentiometerMeatValRAW();

   
   return (raw / SCALING_FACTOR);
}

// ----------------------------------------------------------------------------
/// \brief     
/// \detail    
/// \warning   
/// \return    
/// \todo
uint16_t getPotentiometerOvenValRAW()
{
   return analogRead(POTI_OVEN_PIN);
}

// ----------------------------------------------------------------------------
/// \brief     
/// \detail    
/// \warning   
/// \return    
/// \todo
uint16_t getPotentiometerMeatValRAW()
{
   return analogRead(POTI_MEAT_PIN);
}