// ****************************************************************************
/// \file      TimeControl.cpp
///
/// \brief     Sets and Returns time and date
///
/// \details   Simple check, if input values are valid
///
/// \author    Christoph Capiaghi
///
/// \version   0.2
///
/// \date      20170510
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
#include "TimeControl.h"


// Private constants **********************************************************
static tmElements_t tm; // Struct with parameters from second to year

static uint8_t timerHour   = 0;
static uint8_t timerMin    = 0;
static uint8_t startHour   = 0;
static uint8_t startMin    = 0;
static uint8_t endHour     = 0;
static uint8_t endMin      = 0;


// ----------------------------------------------------------------------------
/// \brief     Get the current time hour
/// \detail    
/// \warning   
/// \return    current hour
/// \todo
uint8_t getTimeHour()
{
	RTC.read(tm);
	return tm.Hour;
}

// ----------------------------------------------------------------------------
/// \brief     Get the current time minute
/// \detail    
/// \warning   
/// \return    current min
/// \todo
uint8_t getTimeMin()
{
	RTC.read(tm);
	return tm.Minute;
}

// ----------------------------------------------------------------------------
/// \brief     Get the current day
/// \detail    
/// \warning   
/// \return    current day
/// \todo
uint8_t getTimeDay()
{
	RTC.read(tm);
	return tm.Day;
}

// ----------------------------------------------------------------------------
/// \brief     Get the current month
/// \detail    
/// \warning   
/// \return    current month
/// \todo
uint8_t getTimeMonth()
{
	RTC.read(tm);
	return tm.Month;
}

// ----------------------------------------------------------------------------
/// \brief     Get the current year
/// \detail    
/// \warning   
/// \return    current year
/// \todo
uint16_t getTimeYear()
{
	RTC.read(tm);
	return tmYearToCalendar(tm.Year);
}

// ----------------------------------------------------------------------------
/// \brief     Set the current hour
/// \detail    Checks input: 0 to 23 allowed
/// \warning   
/// \return    
/// \todo
void setTimeHour(uint8_t value)
{
   if( value < 0 || value > 23)
   {
      value = 0;
   }
	tm.Hour = value;
   RTC.write(tm);
}

// ----------------------------------------------------------------------------
/// \brief     Set the current min
/// \detail    Checks input: 0 to 59 allowed. Set seconds to 0
/// \warning   
/// \return    
/// \todo
void setTimeMin(uint8_t value)
{
   if( value < 0 || value > 59)
   {
      value = 0;
   }
	tm.Minute = value;
   tm.Second = 0;
   RTC.write(tm);
}

// ----------------------------------------------------------------------------
/// \brief     Set the current day
/// \detail    Checks input: 1 to 31 allowed.
/// \warning   
/// \return    
/// \todo
void setTimeDay(uint8_t value)
{
   if( value < 1 || value > 31)
   {
      value = 0;
   }
	tm.Day = value;
	RTC.write(tm);
}

// ----------------------------------------------------------------------------
/// \brief     Set the current month
/// \detail    Checks input: 1 to 12 allowed.
/// \warning   
/// \return    
/// \todo
void setTimeMonth(uint8_t value)
{
   if( value < 1 || value > 12)
   {
      value = 0;
   }
	tm.Month = value;
	RTC.write(tm);
}

// ----------------------------------------------------------------------------
/// \brief     Set the current year
/// \detail    Checks input: years < 2017 not allowed
/// \warning   
/// \return    
/// \todo
void setTimeYear(uint16_t value)
{   if( value < 2017)
   {
      value = 2017;
   }
	tm.Year = CalendarYrToTm(value); // Definition of time: offset from 1970;
	RTC.write(tm);
}

// ----------------------------------------------------------------------------
/// \brief     
/// \detail    
/// \warning   
/// \return    
/// \todo
uint8_t checkRTC()
{

	if(RTC.read(tm))
	{
		Serial.println("RTC Present");
		Serial.print("Time = ");
		print2digits(tm.Hour);
		Serial.write(':');
		print2digits(tm.Minute);
		Serial.write(':');
		print2digits(tm.Second);
		Serial.print(", Date (D/M/Y) = ");
		Serial.print(tm.Day);
		Serial.write('/');
		Serial.print(tm.Month);
		Serial.write('/');
		Serial.print(tmYearToCalendar(tm.Year));
		Serial.println();
	}
	else
	{
		Serial.println("E11: The RTC is stopped. Please config time");
		return 0;
	}
   
   if(RTC.chipPresent())
	{
		Serial.println("RTC Present");
	}
	else
	{
		Serial.println("E10: RTC not present. Connected?");
		return 0;
	}
   
	return 1;
}

static void print2digits(int number) {
  if (number >= 0 && number < 10) {
    Serial.write('0');
  }
  Serial.print(number);
}


// Timer **********************************************************

// ----------------------------------------------------------------------------
/// \brief     Set the timer hour
/// \detail    Checks input: 0 h to 24 h allowed
/// \warning   
/// \return    
/// \todo
void setTimerHour(uint8_t value)
{
   if( value < 0 || value > 24)
   {
      value = 0;
   }
	timerHour = value;
}


// ----------------------------------------------------------------------------
/// \brief     Set the timer minute
/// \detail    Checks input: 0 to 59 allowed.
/// \warning   
/// \return    
/// \todo
void setTimerMin(uint8_t value)
{
   if( value < 0 || value > 59)
   {
      value = 0;
   }
	timerMin = value;
}



// ----------------------------------------------------------------------------
/// \brief     Returns the timer hour
/// \detail    
/// \warning   
/// \return    uint8_t timer hour
/// \todo
uint8_t getTimerHour()
{
   return timerHour;
}

// ----------------------------------------------------------------------------
/// \brief     Returns the timer minute
/// \detail    
/// \warning   
/// \return    uint8_t timer minute
/// \todo
uint8_t getTimerMin()
{
	return timerMin;
}



// ----------------------------------------------------------------------------
/// \brief     Starts the timer
/// \detail    
/// \warning   Be sure you have set the timer values (setTimerHour and setTimerMin)
/// \return    uint8_t timer minute
/// \todo      Testing
void startTimer()
{
   // Example: Current time 13:55
   // Timer set to 1 h 10 min
   // Current time + timer = End time -> 15:05
   
   Serial.println("Start Timer");
   
   // Read current time (ex. 13:55)
   startHour   = getTimeHour();      // ex. 13 h
   startMin    = getTimeMin();       // ex. 10 min
   
   
   // Calculate End time: Read timer and add
   endHour     = startHour + timerHour; // 13 + 1 = 14
   endMin      = startMin + timerMin;   // 55 + 10 = 65

   // For this example, we get the end time 14:65
   // Correction:
   
   if ( endMin > 59 )
   {
      endHour++;              // Increase Hour -> 14 + 1 = 15
      endMin = endMin % 60;   // Wrap min: 65 mod 60 = 5  
   }
   // Resulting in 15:05 (correct)
   
   // What if the timer goes over midnight?
   if (endHour > 24)
   {
      endHour = endHour % 24;
   }
   
   // Now everything is set.
   // Timer is expired, if:
   //    current hour   >= endHour 
   //    current min    >= endMin   
   
   
}

// ----------------------------------------------------------------------------
/// \brief     Returns the state of the timer
/// \detail    
/// \warning   -
/// \return    bool: true finished, false running
/// \todo
bool timerExpired()
{
   // now >= start
   if( (getTimeHour() >= endHour) && (getTimeMin() >= endMin) )
   {
      return true;
   }
   else
   {
      return false;
   }
}


// ----------------------------------------------------------------------------
/// \brief     Returns the timer end hour (for example 13:14 -> 13)
/// \detail    
/// \warning   
/// \return    uint8_t timer end hour
/// \todo
uint8_t getEndHour()
{
  return endHour;
}


// ----------------------------------------------------------------------------
/// \brief     Returns the timer end min (for example 13:14 -> 14)
/// \detail    
/// \warning   
/// \return    uint8_t timer end min
/// \todo
uint8_t getEndMin()
{
  return endMin;
}

