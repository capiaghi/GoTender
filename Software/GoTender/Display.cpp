// ****************************************************************************
/// \file      Display.cpp
///
/// \brief     Display driver for  A000096
///
/// \details   160 pixel x 128 pixel 
///
/// \author    Christoph Capiaghi
///
/// \version   0.1
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
#include "Display.h"

static TFT TFTscreen = TFT(SPI_CS_LCD_PIN, DC_LCD_PIN, RESET_LCD_PIN);

#define CHAR_ARRAY_LENGTH_TITLE  ( 10 ) // Title length
#define CHAR_ARRAY_LENGTH        ( 30 )



#define XPOS_OVEN          ( 40 )
#define XPOS_MEAT          ( XPOS_OVEN + 40 )
#define XPOS_SMOKER        ( XPOS_MEAT + 40 )

#define YPOS_LABELS        ( 80 )
#define YPOS_SPACE         ( 10 )


// Private constants **********************************************************
// Create Screen object

static uint8_t first = 1;
static char charArray[CHAR_ARRAY_LENGTH];
static char charArrayTitle[CHAR_ARRAY_LENGTH_TITLE];
static char oldCharArray[CHAR_ARRAY_LENGTH] = ""; 

static uint16_t oldValue = -1;
static uint8_t oldHour = -1;
static uint8_t oldMin = -1;

static int16_t oldTemperatureOven            = -1;
static int16_t oldTemperatureMeat            = -1;
static int16_t oldTemperatureOvenSetPoint    = -1;
static int16_t oldTemperatureMeatSetPoint    = -1;

static String oldCmd = "";


// ----------------------------------------------------------------------------
/// \brief     Initialize Display
/// \detail    Initialize and sets Background Color
/// \warning   
/// \return    -
/// \todo      -
void initDisplay()
{
  // Initialize Display
  TFTscreen.begin();
  TFTscreen.background(LCD_BACKGROUND_COLOR); // Clear the screen with a pretty color  
}
// ----------------------------------------------------------------------------
/// \brief     Displays State
/// \detail    Display State on top left, medium font size
/// \warning   
/// \return    -
/// \todo
void displayTitle(String stateStr)
{
   TFTscreen.stroke(LCD_FONT_COLOR);
   TFTscreen.background(LCD_BACKGROUND_COLOR); // Clear screen
   TFTscreen.setTextSize(MEDIUM_FONT_SIZE);
   
   // Save to charArray
   stateStr.toCharArray(charArrayTitle, CHAR_ARRAY_LENGTH);
   // Set new text
   TFTscreen.text(charArrayTitle, 0, 0);  
   displayRefresh(); // Write all facts again (clear screen delete time / date) TBD: okay?
}

// ----------------------------------------------------------------------------
/// \brief     Displays test
/// \detail    Display test value (uint16_t) in the middle left, small font size
/// \warning   -
/// \return    -
/// \todo      Delete
void testDisplayOutput(uint16_t value)
{
   // Font color and text size
   TFTscreen.stroke(LCD_FONT_COLOR); // White Font
   TFTscreen.setTextSize(SMALL_FONT_SIZE);
   
   writeString(String(value), String(oldValue), 0, 50);
      
   // Save value
   oldValue = value;
}

// ----------------------------------------------------------------------------
/// \brief     Displays time
/// \detail    Display time on top right, small font size. Updates only when
///            old value changes
/// \warning   
/// \return    -
/// \todo      - Seconds? Flashing ":"? Tests pending
void displayTime()
{
   uint8_t hour   =  getTimeHour();
   uint8_t min    =  getTimeMin();
   
   // Update only, if anything changed
   if(hour != oldHour || min != oldMin)
   {
      String hourStr;
      String minStr;
      
      String oldHourStr;
      String oldMinStr;
      
      // time format: hh:mm
      if( hour < 10) // add "0"
      {
         hourStr     = "0" + String(hour);
         oldHourStr  = "0" + String(oldHour);
      }
      else
      {
         hourStr     = String(hour);
         oldHourStr  = String(oldHour);
      }
      
      if( min < 10)
      {
         minStr      = "0" + String(min);
         oldMinStr   = "0" + String(oldMin);
      }
      else
      {
         minStr      = String(min);
         oldMinStr   = String(oldMin);
      }

      String timeStr     = hourStr     + ":" + minStr;
      String oldTimeStr  = oldHourStr  + ":" + oldMinStr;
      
      // Set text size
      TFTscreen.setTextSize(SMALL_FONT_SIZE);
      
      writeString(String(timeStr), String(oldTimeStr), 125, 0);
      
      // Safe old values
      oldHour = hour;
      oldMin = min;
   }
}

// ----------------------------------------------------------------------------
/// \brief     Displays date
/// \detail    Display date on bottom right, small font size in format dd.mm.yyyy
/// \warning   
/// \return    -
/// \todo      - Update, when refreshing screen (now: delete)
void displayDate()
{
   uint8_t day     = getTimeDay();
   uint8_t month   = getTimeMonth();
   uint16_t year   = getTimeYear();
   String dateStr;
   String dayStr;
   String monthStr;
   
   // Day format: dd
   if ( day < 10) // add "0"
   {
      dayStr = "0" + String(day);
   }
   else
   {
      dayStr = String(day);
   }
   
   // Month format: mm
   if ( month < 10) // add "0"
   {
      monthStr = "0" + String(month);
   }
   else
   {
      monthStr = String(month);
   }
   

   // Date format: dd.mm.yyyy
   dateStr = dayStr + "." + monthStr + "." + String(year);
   
   // Font text size
   TFTscreen.setTextSize(SMALL_FONT_SIZE);
   
   writeString(String(dateStr), String(""), 100, 120);
   
}

// ----------------------------------------------------------------------------
/// \brief     Displays command
/// \detail    Display big command under title
/// \warning   
/// \return    -
/// \todo      - 
void displayCommand(String cmd)
{
   TFTscreen.setTextSize(MEDIUM_FONT_SIZE);
   writeString(cmd, oldCmd, 0, 25);
   oldCmd = cmd.substring(0);
}

// ----------------------------------------------------------------------------
/// \brief     Displays commandSmall1
/// \detail    Display small command under command
/// \warning   
/// \return    -
/// \todo      - 
void displayCommandSmall1(String cmd)
{
   TFTscreen.setTextSize(SMALL_FONT_SIZE);
   writeString(cmd, oldCmd, 0, 50);
   oldCmd = cmd.substring(0);
}

// ----------------------------------------------------------------------------
/// \brief     Displays commandSmall2
/// \detail    Display small command under command1
/// \warning   
/// \return    -
/// \todo      - 
void displayCommandSmall2(String cmd)
{
   TFTscreen.setTextSize(SMALL_FONT_SIZE);
   writeString(cmd, oldCmd, 0, 60);
   oldCmd = cmd.substring(0);
}


// ----------------------------------------------------------------------------
/// \brief     Display actual temperature
/// \detail    
/// \warning   
/// \return    -
/// \todo      - 
static void displayActualTemperature()
{
   int16_t temperatureOven = getTemperatureOven();
   int16_t temperatureMeat = getTemperatureMeat();
   
   // Update only, if anything changed
   if((temperatureOven != oldTemperatureOven))
   {    
      TFTscreen.stroke(LCD_FONT_COLOR);
      TFTscreen.setTextSize(SMALL_FONT_SIZE);
      writeString(String(temperatureOven) + " C", String(oldTemperatureOven) + " C", XPOS_OVEN, YPOS_LABELS + YPOS_SPACE);
      // Safe old values
      oldTemperatureOven = temperatureOven;
   }
   
   // Update only, if anything changed
   if((temperatureMeat != oldTemperatureMeat))
   {     
      TFTscreen.stroke(LCD_FONT_COLOR);
      TFTscreen.setTextSize(SMALL_FONT_SIZE);
      writeString(String(temperatureMeat)+ " C", String(oldTemperatureMeat)+ " C", XPOS_MEAT, YPOS_LABELS + YPOS_SPACE);
      // Safe old values
      oldTemperatureMeat = temperatureMeat;
   }

}

// ----------------------------------------------------------------------------
/// \brief     Display setpoint temperature
/// \detail    
/// \warning   Converts double temperature in int16_t temperature!
/// \return    -
/// \todo      get Temperature from setter 
static void displaySetPointTemperature()
{
   int16_t temperatureOvenSetPoint = getTemperatureOvenSetPoint(); // TBD: test
   int16_t temperatureMeatSetPoint = getTemperatureMeatSetPoint(); // TBD: test
   
   // Update only, if anything changed
   if((temperatureOvenSetPoint != oldTemperatureOvenSetPoint))
   {    
      TFTscreen.setTextSize(SMALL_FONT_SIZE);
      writeString(String(temperatureOvenSetPoint)+ " C", String(oldTemperatureOvenSetPoint)+ " C", XPOS_OVEN, YPOS_LABELS + 2*YPOS_SPACE);
      oldTemperatureOvenSetPoint = temperatureOvenSetPoint;
   }
   
   if((temperatureMeatSetPoint != oldTemperatureMeatSetPoint))
   {    
      TFTscreen.setTextSize(SMALL_FONT_SIZE);
      writeString(String(temperatureMeatSetPoint)+ " C", String(oldTemperatureMeatSetPoint)+ " C", XPOS_MEAT, YPOS_LABELS + 2*YPOS_SPACE);
      oldTemperatureMeatSetPoint = temperatureMeatSetPoint;
   }
}

// ----------------------------------------------------------------------------
/// \brief     Force value update
/// \detail    Call one before change states: Updates all values
/// \warning   
/// \return    -
/// \todo      get Temperature from setter 
void displayRefresh()
{
   first = 1;   
   oldValue = -1;
   oldHour = -1;
   oldMin = -1;
   oldTemperatureOven            = -1;
   oldTemperatureMeat            = -1;
   oldTemperatureOvenSetPoint    = -1;
   oldTemperatureMeatSetPoint    = -1;
   oldCmd = "";
   displaySmokerState();
   displayDate();
   displayTemperatures();
}

// ----------------------------------------------------------------------------
/// \brief     Display all temperatures
/// \detail    Display set and actual temperatures
/// \warning   
/// \return    -
/// \todo      - 
void displayTemperatures()
{
   if (first == 1) // Draw table whitout values
   {
      TFTscreen.setTextSize(SMALL_FONT_SIZE);
      TFTscreen.text("Oven",     XPOS_OVEN,     YPOS_LABELS);
      TFTscreen.text("Meat",     XPOS_MEAT,     YPOS_LABELS);
      TFTscreen.text("Smoker",   XPOS_SMOKER,   YPOS_LABELS);
      
      TFTscreen.text("Now:",     0,    YPOS_LABELS + YPOS_SPACE);
      TFTscreen.text("Set:",     0,    YPOS_LABELS + 2*YPOS_SPACE);
      first = 0;
   }
   displayActualTemperature();
   displaySetPointTemperature();
}


// ----------------------------------------------------------------------------
/// \brief     Draws a circle with state of the smoker 
/// \detail    Red filled: Enabled, not filled: disabled
/// \warning
/// \return    -
/// \todo      
void displaySmokerState()
{
   if(getSmokerState())
   {
      TFTscreen.stroke(LCD_FONT_COLOR);
      TFTscreen.fill(LCD_SMOKER_STATE_COLOR);
      TFTscreen.circle(XPOS_SMOKER + 10, YPOS_LABELS + 2*YPOS_SPACE, CIRCLE_RADIUS); // TBD Coordinates
   }
   else
   {
      TFTscreen.stroke(LCD_FONT_COLOR);
      TFTscreen.fill(LCD_BACKGROUND_COLOR);
      TFTscreen.circle(XPOS_SMOKER + 10, YPOS_LABELS + 2*YPOS_SPACE, CIRCLE_RADIUS); // TBD Coordinates
   }
   
}


// ----------------------------------------------------------------------------
/// \brief     Writes a string to the display
/// \detail    Deletes and writes new string to display
/// \warning
/// \return    -
/// \todo      
static void writeString(String text, String oldText, uint8_t xPos, uint8_t yPos)
{
   // Save to charArray
   text.toCharArray(charArray, CHAR_ARRAY_LENGTH);
   oldText.toCharArray(oldCharArray, CHAR_ARRAY_LENGTH);
   // Delete old text
   TFTscreen.stroke(INVERSE_LCD_FONT_COLOR);
   TFTscreen.text(oldCharArray, xPos, yPos);
   // Set new text
   TFTscreen.stroke(LCD_FONT_COLOR);
   TFTscreen.text(charArray, xPos, yPos);
}