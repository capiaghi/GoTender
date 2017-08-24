// ****************************************************************************
/// \file      TimeControl.h
///
/// \brief     
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
#ifndef __TIMECONTROL_H__
#define __TIMECONTROL_H__

// Include ********************************************************************
#include "Arduino.h"
#include "Time.h"
#include <TimeLib.h>                // Very important: Use actual library (webside https://www.pjrc.com/teensy/td_libs_Time.html)
#include <DS1307RTC.h>              // RTC
#include <Wire.h>

// Exported types *************************************************************
/// \brief     Optional description
/// \details   Optional description (more detailed)


// Exported constants *********************************************************

// Exported macros ************************************************************

// Exported functions *********************************************************


uint8_t getTimeHour();
uint8_t getTimeMin();
uint8_t getTimeDay();
uint8_t getTimeMonth();
uint16_t getTimeYear();

void setTimeHour(uint8_t value);
void setTimeMin(uint8_t value);
void setTimeDay(uint8_t value);
void setTimeMonth(uint8_t value);
void setTimeYear(uint16_t value);
uint8_t checkRTC();
static void print2digits(int number);

// Timer
void setTimerHour(uint8_t value);
void setTimerMin(uint8_t value);
uint8_t getTimerHour();
uint8_t getTimerMin();
void startTimer();
bool timerExpired();






#endif
