// ****************************************************************************
/// \file      Settings.h
///
/// \brief     Settings for GoTender
///
/// \details   Sets Time and Temperature
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
#ifndef __SETTINGS_H__
#define __SETTINGS_H__

// Include ********************************************************************
#include "Arduino.h"
#include "ButtonHandler.h"
#include "Time.h"
#include "Thermocouples.h"

// Exported types *************************************************************
/// \brief     Optional description
/// \details   Optional description (more detailed)


// Exported constants *********************************************************


// Exported macros ************************************************************

// Exported functions *********************************************************
uint8_t		settings(ButtonHandler *buttons, tmElements_t *tm);

#endif
