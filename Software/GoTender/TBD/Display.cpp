// ****************************************************************************
/// \file      Display.cpp
///
/// \brief     Display driver for  A000096
///
/// \details   160 pixel x 128 pixel 
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
#include "Display.h"

// Create Screen object
static TFT TFTscreen = TFT(SPI_CS_LCD_PIN, DC_LCD_PIN, RESET_LCD_PIN);

#define CHAR_ARRAY_LENGTH_TITLE  ( 10 ) // Title length
#define CHAR_ARRAY_LENGTH        ( 20 ) // Command length
// Private constants **********************************************************
static uint8_t first = 1;
static char charArrayValue[CHAR_ARRAY_LENGTH];
static char oldCharArrayValue[CHAR_ARRAY_LENGTH] = ""; 

static char oldCmd1[CHAR_ARRAY_LENGTH] = "";
static char oldCmd2[CHAR_ARRAY_LENGTH] = "";
static char oldCmd3[CHAR_ARRAY_LENGTH] = "";

static float oldValue = -1;

static uint8_t xPos = 0; // For oscilloscope function -> Position



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
/// \warning   Deletes text on display
/// \return    -
/// \todo
void displayTitle(char *state)
{
   TFTscreen.stroke(LCD_FONT_COLOR);
   TFTscreen.background(LCD_BACKGROUND_COLOR); // Clear screen
   TFTscreen.setTextSize(MEDIUM_FONT_SIZE);
   
   // Set new text
   TFTscreen.text(state, 0, 0);  
   displayRefresh(); // Write all facts again (clear screen delete time / date) TBD: okay?
}


// ----------------------------------------------------------------------------
/// \brief     Displays command
/// \detail    Display  command under title
/// \warning   
/// \return    -
/// \todo      - 
void displayCommand1(char *cmd)
{
   TFTscreen.setTextSize(SMALL_FONT_SIZE);
   writeString(cmd, oldCmd1, 0, 25);
   strncpy(oldCmd1, cmd, CHAR_ARRAY_LENGTH);
   
}

// ----------------------------------------------------------------------------
/// \brief     Displays displayCommand1
/// \detail    Display small command under command
/// \warning   
/// \return    -
/// \todo      - 
void displayCommand2(char *cmd)
{
   TFTscreen.setTextSize(SMALL_FONT_SIZE);
   writeString(cmd, oldCmd2, 0, 50);
   strncpy(oldCmd2, cmd, CHAR_ARRAY_LENGTH);
   
}



void displayValues(float value)
{
   // Update only, if anything changed
   if((value != oldValue))
   {    
      TFTscreen.stroke(LCD_FONT_COLOR);
      TFTscreen.setTextSize(SMALL_FONT_SIZE);
      String valueStr = String(value, DECIMAL_PLACES) + " C";
      String oldValueStr = String(oldValue, DECIMAL_PLACES) + " C";
      
      valueStr.toCharArray(charArrayValue, CHAR_ARRAY_LENGTH);
      oldValueStr.toCharArray(oldCharArrayValue, CHAR_ARRAY_LENGTH);
      
      writeString(charArrayValue, oldCharArrayValue, 0, 60);
      // Safe old values
      oldValue = value;
   }

}



// ----------------------------------------------------------------------------
/// \brief     Force value update
/// \detail    Call one before change states: Updates all values
/// \warning   
/// \return    -
/// \todo     
void displayRefresh()
{
   first = 1;   
   oldCmd1[0] = "0";
   oldCmd2[0] = "0";
   oldCmd3[0] = "0";
   oldValue = -1.0;
   
}

// ----------------------------------------------------------------------------
/// \brief     Display Oscilloscope
/// \detail    Display set and actual temperatures
/// \warning   
/// \return    -
/// \todo      - 
void displayOscilloscope(uint16_t ADCvalue)
{
   if (first == 1) // Draw table whitout values
   {
      // TBD: Draw lines
      TFTscreen.line(0, 0, 50 , 50 ); //TBD

      first = 0;
   }
   
   uint8_t mappedADCvalue = map(ADCvalue, ADC_MIN_VALUE, ADC_MAX_VALUE, 0, TFTscreen.height() - UPPERBOUND_LCD); // Re-maps ADC Value to TFT-Screen
  
   // Write line -> Oscilloscope function
   // draw a line in a nice color
   // Fist of all: Delete old line with black
   TFTscreen.stroke(LCD_BACKGROUND_COLOR); // Cursor color black.
   TFTscreen.line(xPos, UPPERBOUND_LCD, xPos, TFTscreen.height()); // Delete Screen

   // Change color to LCD_CURSOR_COLOR and draw to lines
   // Line thickness: 2 Pixels (xpos + 1, xpos + 2)
   // Note: No overflow detection: No problem for display (tested)
   TFTscreen.stroke(LCD_CURSOR_COLOR); // Cursor color yellow
   TFTscreen.line(xPos + 1, UPPERBOUND_LCD, xPos + 1, TFTscreen.height()); // Draw 1 Line at pos + 1
   TFTscreen.line(xPos + 2, UPPERBOUND_LCD, xPos + 2, TFTscreen.height()); // Draw 1 Line at pos + 2

   // Draw measurement value
   TFTscreen.stroke(LCD_GRAPH_COLOR); // Graph color red
   TFTscreen.line(xPos, TFTscreen.height() - mappedADCvalue, xPos, TFTscreen.height());

   // if the graph has reached the screen edge start again
   if (xPos >= TFTscreen.width())
   {
      xPos = 0;
   }
   else
   {
      // increment the horizontal (sample) position:
      xPos++;
   }	
   
}


// ----------------------------------------------------------------------------
/// \brief     Display FullPower
/// \detail    For Full Power test: White screen
/// \warning   
/// \return    -
/// \todo      - 
void displayFullPower()
{
   TFTscreen.background(ST7735_WHITE);
   TFTscreen.stroke(ST7735_BLACK);
   TFTscreen.setTextSize(SMALL_FONT_SIZE);
   TFTscreen.text("Power Test", 0, 60);
   
}

// ----------------------------------------------------------------------------
/// \brief     Writes a string to the display
/// \detail    Deletes and writes new string to display
/// \warning
/// \return    -
/// \todo      
static void writeString(char *charArray, char *oldCharArray, uint8_t xPos, uint8_t yPos)
{
   // Delete old text -> Set to background color
   TFTscreen.stroke(LCD_BACKGROUND_COLOR);
   TFTscreen.text(oldCharArray, xPos, yPos);
   // Set new text
   TFTscreen.stroke(LCD_FONT_COLOR);
   TFTscreen.text(charArray, xPos, yPos);
   
}
