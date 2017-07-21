// ****************************************************************************
/// \file      Display.h
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
#ifndef __DISPLAY_H__
#define __DISPLAY_H__

// Include ********************************************************************
#include "Arduino.h"
#include "PinMapping.h"
#include "Constants.h"
#include "VoltageMeasurement.h"
#include "CurrentMeasurement.h"
#include <TFT.h>                    // Arduino LCD Library
#include <SPI.h>                    // Display and SD card


// Exported types *************************************************************
/// \brief     Optional description
/// \details   Optional description (more detailed)


// Exported constants *********************************************************
// Sets the size of text that follows. The default size is "1".
// Each change in size increases the text by 10 pixels in height.
// That is, size 1 = 10 pixels, size 2 =20 pixels, and so on.
const uint8_t    SMALL_FONT_SIZE    = 1; // Font size of text (display)
const uint8_t    MEDIUM_FONT_SIZE   = 2;
const uint8_t    BIG_FONT_SIZE      = 3;

const uint8_t    UPPERBOUND_LCD       = 30; // For oscilloscope function
                                            // Leave this section (0 - x in y -direction) empty
                                            // Used to not overwrite the title of the screen
                                            // ex. "Voltage"

// Exported macros ************************************************************
// This Colors are supported (copied from lib):
// ST7735_BLACK   0x0000
// ST7735_BLUE    0x001F
// ST7735_RED     0xF800
// ST7735_GREEN   0x07E0
// ST7735_CYAN    0x07FF
// ST7735_MAGENTA 0xF81F
// ST7735_YELLOW  0xFFE0  
// ST7735_WHITE   0xFFFF
#define LCD_BACKGROUND_COLOR     ( ST7735_BLACK )
#define LCD_FONT_COLOR           ( ST7735_WHITE )
#define LCD_CURSOR_COLOR         ( ST7735_YELLOW )
#define LCD_GRAPH_COLOR          ( ST7735_RED )

#define DECIMAL_PLACES           ( 3 ) // Decimal places display output


// Exported functions *********************************************************
void initDisplay();
void displayTitle(char *state);

void displayCommand1(char *cmd);       // Underneath Title (Size small)
void displayCommand2(char *cmd);      // Underneath Command (Size small)


void displayRefresh();        // Call once before displayTemperatures -> Refreshes all values
void displayOscilloscope(uint16_t ADCvalue);
void displayValues(float value);
void displayFullPower();


static void writeString(char *charArray, char *oldCharArray, uint8_t xPos, uint8_t yPos);

#endif
