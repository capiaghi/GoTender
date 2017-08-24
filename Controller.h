// ****************************************************************************
/// \file      Controller.h
///
/// \brief     Main Controller for the heater
///
/// \details   Controller for the run state
///
/// \author    Christoph Capiaghi
///
/// \version   0.1
///
/// \date      20170522
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
#ifndef __CONTROLLER__H
#define __CONTROLLER__H

// Include ********************************************************************
#include "Arduino.h"
#include "PinMapping.h"
#include "Config.h" // True and false

// Exported types *************************************************************
/// \brief     Optional description
/// \details   Optional description (more detailed)


// Exported constants *********************************************************


//const int8_t    MIN_TEMP_OVEN = -10; // For Checking the TC



// Exported macros ************************************************************

// Exported functions *********************************************************
		
double 		getTemperatureOvenSetPoint();
double 		getTemperatureMeatSetPoint();
void		   setTemperatureOvenSetPoint(double val);
void		   setTemperatureMeatSetPoint(double val);

void        startSmoker(bool start);
bool        getSmokerState(void);
void        startHeater(bool start);

void        emergencyOff();

		
#endif
