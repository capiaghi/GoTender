// ****************************************************************************
/// \file      Potentiometers.h
///
/// \brief     
///
/// \details    
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
/// \todo     
///

// Define to prevent recursive inclusion
#ifndef __POTENTIOMETERS__
#define __POTENTIOMETERS__

// Include ********************************************************************
#include "Arduino.h"
#include "PinMapping.h"

// Exported types *************************************************************
/// \brief     Optional description
/// \details   Optional description (more detailed)


// Exported constants *********************************************************

// Exported macros ************************************************************

// Exported functions *********************************************************


uint16_t getPotentiometerOvenVal();
uint16_t getPotentiometerMeatVal();
uint16_t getPotentiometerOvenValRAW();
uint16_t getPotentiometerMeatValRAW();

#endif
