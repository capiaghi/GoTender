// ****************************************************************************
/// \file      ButtonHandler.h
///
/// \brief     Sets and return Button flags
///
/// \details   Button Handler
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
#ifndef __BUTTONHANDLER_H__
#define __BUTTONHANDLER_H__

// Include ********************************************************************
#include "Arduino.h"

// Exported types *************************************************************
/// \brief     Optional description
/// \details   Optional description (more detailed)


// Exported constants *********************************************************
const long       DEBOUNCE_DELAY = 50;    // the debounce time; increase if the output flickers

// Buttons TBD
#define BUTTON_UP_PIN                   ( 0 ) // Up
#define BUTTON_ENTER_PIN                ( 1 ) // Enter
#define BUTTON_DOWN_PIN                 ( 2 ) // Down

// Exported macros ************************************************************

// Exported functions *********************************************************
class ButtonHandler
{
  public:
	ButtonHandler::ButtonHandler(const uint8_t buttonUpPin, const uint8_t buttonEnterPin, const uint8_t buttonDownPin);
	~ButtonHandler();
	
	void		  updateButtonHandler();
  void      clearAllFlags();
	uint8_t 	getButtonStateUp();
	uint8_t 	getButtonStateEnter();
	uint8_t 	getButtonStateDown();
	
  private:
	uint8_t 	_m_buttonUpPin;
	uint8_t		_m_buttonEnterPin;	
	uint8_t		_m_buttonDownPin;
	uint8_t 	buttonStateUp;
	uint8_t 	buttonStateEnter;
	uint8_t 	buttonStateDown;
	uint8_t 	lastbuttonStateUp;
	uint8_t 	lastbuttonStateEnter;
	uint8_t 	lastbuttonStateDown;
	uint8_t 	buttonUp;
	uint8_t 	buttonEnter;
	uint8_t 	buttonDown;

	// the following variables are long's because the time, measured in miliseconds,
	// will quickly become a bigger number than can be stored in an int.
	long 		lastDebounceTimeUp;  // the last time the output pin was toggled
	long 		lastDebounceTimeEnter; 
	long 		lastDebounceTimeDown; 
};
#endif
