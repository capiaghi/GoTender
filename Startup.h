// ****************************************************************************
/// \file      Startup.h
///
/// \brief     Startup Sequence
///
/// \details   
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
#ifndef __STARTUP_H__
#define __STARTUP_H__

// Include ********************************************************************
#include "Time.h"
#include "Thermocouples.h"
#include <DS1307RTC.h>              // RTC
#include "TimeControl.h"

// Exported types *************************************************************
/// \brief     Optional description
/// \details   Optional description (more detailed)


// Exported constants *********************************************************


// Exported macros ************************************************************

// Exported functions *********************************************************
uint8_t		startup();

#endif
