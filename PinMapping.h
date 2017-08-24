// ****************************************************************************
/// \file      PinMapping.h
///
/// \brief     Defines Pins of the Board
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
#ifndef __PINMAPPING_H__
#define __PINMAPPING_H__

// Include ********************************************************************
#include "Arduino.h"
// Exported types *************************************************************
/// \brief     Optional description
/// \details   Optional description (more detailed)


// Exported constants *********************************************************

// Exported macros ************************************************************
// Mapping according "pins_arduino.h"

// Buttons TBD
#define BUTTON_UP_PIN                   ( 32 ) // Up
#define BUTTON_ENTER_PIN              	 ( 34 ) // Enter, ( 1 ) not functional
#define BUTTON_DOWN_PIN                 ( 36 ) // Down

#define BUTTON_SMOKER						 ( 24 ) // Smoker with LED

// LEDs TBD
#define GREEN_LED_PIN                   ( 26 ) // Green: Ok / Running
#define YELLOW_LED_PIN                  ( 31 ) // Not Ok / Error
#define RED_LED_PIN                     ( 28 ) // Not Ok / Error

// LCD Pins
#define SPI_CS_LCD_PIN                  ( 53 ) // Chip Select Display
#define DC_LCD_PIN                      ( 40 )  // LCD
#define RESET_LCD_PIN                   ( 38 )  // Reset Pin of the LCD


// Thermocouples
#define SPI_CS_TC_OVEN_PIN              ( 6 )  // Chip Select Thermocouple Oven
#define SPI_CS_TC_MEAT_PIN              ( 4 )  // Chip Select Thermocouple Meat

// Prototyping Shield
// Uno: A4 (SDA), A5 (SCL)
#define SPI_CS_SD_PIN                   ( 10 )  // Chip Select SD Card TBD!!! Has to be Pin 10 (fix)

// ADC Pins
#define POTI_OVEN_PIN                        ( A0 ) // Channel for Analog Digital Converter
#define POTI_MEAT_PIN                        ( A1 ) // Channel for Analog Digital Converter

// DO NOT USE A4 AND A5:
// Uno, Ethernet	A4 (SDA), A5 (SCL)
// Mega2560	20 (SDA), 21 (SCL)
// -> Connect A4 with 20 and A5 with 21

// PWM
#define RELAIS_HEATER                         ( 2 )  // TBD: Rename to heater
#define RELAIS_SMOKER                         ( 3 )  // TBD: Rename to heater
// D3, D2 Relais

// RTC TBD
// I2C


// Exported functions *********************************************************

#endif
