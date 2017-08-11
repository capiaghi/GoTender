// ****************************************************************************
/// \file      Controller.h
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

//smoker
void        enableSmoker();         // This command enables the smoker. Call setSmoker first
void        disableSmoker();
uint8_t     getSmokerState();

void        setSmoker();
void        resetSmoker();

//heater
void        enableHeater();         // This command enables the smoker. Call setHeater first
void        disableHeater();
uint8_t     getHeaterState();

void        setHeater();
void        resetHeater();



    
#endif
