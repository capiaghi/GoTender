// ****************************************************************************
/// \file      Settings.c
///
/// \brief     Sets and returns Button flags
///
/// \details   Button Handler for button up, enter and down
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
#include "Settings.h"



/// \brief Used SubStates
/// \details States for the settings state machine
typedef enum stm_substate_e
{
  STM_SUBSTATE_SET_TIME,                /// RTC Time and Date
  STM_SUBSTATE_SET_TEMPERATURE_OVEN,   /// Set temperature
  STM_SUBSTATE_SET_TEMPERATURE_MEAT,
  STM_SUBSTATE_DONE,                    /// Next main state
} stm_substate_t;

/// \brief Used SubStates time
/// \details States for the settings state machine
typedef enum stm_substate_time_e
{
  STM_SUBSTATE_SET_HOUR,          /// RTC Hour
  STM_SUBSTATE_SET_MIN,           /// RTC Min
  STM_SUBSTATE_SET_DAY,           /// RTC Day
  STM_SUBSTATE_SET_MONTH,         /// RTC Month
  STM_SUBSTATE_SET_YEAR,          /// RTC YEAR
  STM_SUBSTATE_TIME_DONE,
} stm_substate_time_t;

uint8_t value = 0;
uint8_t nextState = 0;
// Private constants **********************************************************

static stm_substate_t       stm_SubState;
static stm_substate_time_t  stm_time_SubState;
// ----------------------------------------------------------------------------
/// \brief     Settings for GoTender
/// \detail    Set time and temperature
/// \warning   
/// \return    if finished: return 1
/// \todo
uint8_t settings(ButtonHandler *buttons, tmElements_t *tm)
{
  // State machine with Time, Temperature, Finished
  switch (stm_SubState)
  {
    //==============================================================================
    case STM_SUBSTATE_SET_TIME:
    
    switch(stm_time_SubState)
    {
      case STM_SUBSTATE_SET_HOUR:
	    #ifdef DEBUG
          Serial.println("STM_SUBSTATE_SET_HOUR");
        #endif
        // Show value on display
        if(nextState == 1)
        {
		// TBD setTimeHour(value);
          tm->Hour = value;
          value = 1;
          nextState = 0;
          stm_time_SubState = STM_SUBSTATE_SET_MIN;
        }
      break;
  
      case STM_SUBSTATE_SET_MIN:
	  	#ifdef DEBUG
          Serial.println("STM_SUBSTATE_SET_MIN");
        #endif
      // Show value on display
        if(nextState == 1)
        {
          tm->Minute = value;
          tm->Second = 0;
          value = 1;
          nextState = 0;
          stm_time_SubState = STM_SUBSTATE_SET_DAY;
        }
      break;

      case STM_SUBSTATE_SET_DAY:
	  	#ifdef DEBUG
          Serial.println("STM_SUBSTATE_SET_MIN");
        #endif
        // Show value on display
        
        if(nextState == 1)
        {
          tm->Day = value;
          value = 1;
          nextState = 0;
          stm_time_SubState = STM_SUBSTATE_SET_MONTH;
        }
      break;

      case STM_SUBSTATE_SET_MONTH:
	  	#ifdef DEBUG
          Serial.println("STM_SUBSTATE_SET_MONTH");
        #endif
        // Show value on display
        
        if(nextState == 1)
        {
          tm->Month = value;
          value = 2017;
          nextState = 0;
          stm_time_SubState = STM_SUBSTATE_SET_YEAR;
        }
      break;

      case STM_SUBSTATE_SET_YEAR:
	  	#ifdef DEBUG
          Serial.println("STM_SUBSTATE_SET_YEAR");
        #endif
        // Show value on display
        if(nextState == 1)
        {
          tm->Year = CalendarYrToTm(value); // Definition of time: offset from 1970;
          nextState = 0;
          stm_SubState = STM_SUBSTATE_SET_TEMPERATURE_OVEN;
        }
    }

    // Set s to 0

      if(buttons->getButtonStateUp())
      {
        value++;
      }

      if(buttons->getButtonStateEnter())
      {
        nextState = 1;
      }
      
      if(buttons->getButtonStateDown())
      {
        value--;
      }

    break;
      
    case STM_SUBSTATE_SET_TEMPERATURE_OVEN:
		#ifdef DEBUG
          Serial.println("STM_SUBSTATE_SET_TEMPERATURE_OVEN");
        #endif
       //Serial.println(readOvenTemperatureC());

      if(buttons->getButtonStateEnter())
      {
        // Set temperature oven TBD
        stm_SubState = STM_SUBSTATE_SET_TEMPERATURE_MEAT;
      }
 
    break;

    case STM_SUBSTATE_SET_TEMPERATURE_MEAT:
		#ifdef DEBUG
          Serial.println("STM_SUBSTATE_SET_TEMPERATURE_MEAT");
        #endif
      //Serial.println(readMeatTemperatureC());

      if(buttons->getButtonStateEnter())
      {
        // Set temperature meat TBD
        stm_SubState = STM_SUBSTATE_DONE;
      }
    break;
    
    case STM_SUBSTATE_DONE:
		#ifdef DEBUG
          Serial.println("STM_SUBSTATE_DONE");
        #endif
		  return 1;
    break;

    default:
    break;
  }
  return 0;
}

