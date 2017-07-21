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
