// ****************************************************************************
/// \file      ButtonHandler.c
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
#include "ButtonHandler.h"

// Private constants **********************************************************


// Constructor
// What should happen when someone creates an instance of your class
ButtonHandler::ButtonHandler(const uint8_t buttonUpPin, const uint8_t buttonEnterPin, const uint8_t buttonDownPin)
{
	_m_buttonUpPin		= buttonUpPin;
	_m_buttonEnterPin	= buttonEnterPin;
	_m_buttonDownPin	= buttonDownPin;
	
	buttonStateUp 			= 0;
	buttonStateEnter 		= 0;
	buttonStateDown 		= 0;
	lastbuttonStateUp 		= 0;
	lastbuttonStateEnter 	= 0;
	lastbuttonStateDown 	= 0;
	buttonUp 				= 0;
	buttonEnter 			= 0;
	buttonDown 				= 0;
}

// Destructor
ButtonHandler::~ButtonHandler()
{
	
}

// ----------------------------------------------------------------------------
/// \brief     Get state of Button Up
/// \detail    Gets the state of the button and resets the flag
/// \warning   
/// \return    state: 1 pressed, 0 not pressed
/// \todo      
///
uint8_t ButtonHandler::getButtonStateUp()
{
	if(buttonUp == 1)
	{
		buttonUp = 0;
		return 1;
	}
	else
	{
		buttonUp = 0;
		return 0;
	}
}

// ----------------------------------------------------------------------------
/// \brief     Get state of Button Enter
/// \detail    Gets the state of the button and resets the flag
/// \warning   
/// \return    state: 1 pressed, 0 not pressed
/// \todo      
///
uint8_t ButtonHandler::getButtonStateEnter()
{
	if(buttonEnter == 1)
	{
		buttonEnter = 0;
		return 1;
	}
	else
	{
		buttonEnter = 0;
		return 0;
	}
}

// ----------------------------------------------------------------------------
/// \brief     Get state of Button Down
/// \detail    Gets the state of the button and resets the flag
/// \warning   
/// \return    state: 1 pressed, 0 not pressed
/// \todo      
///
uint8_t ButtonHandler::getButtonStateDown()
{
	if(buttonDown == 1)
	{
		buttonDown = 0;
		return 1;
	}
	else
	{
		buttonDown = 0;
		return 0;
	}
}

void ButtonHandler::clearAllFlags()
{
  buttonUp = 0;
  buttonEnter = 0;
  buttonDown = 0;
}

// ----------------------------------------------------------------------------
/// \brief     Updates the flags of the buttons
/// \detail    Sets the flags of the 3 buttons
/// \warning   
/// \return    state: 1 pressed, 0 not pressed
/// \todo      
///
void ButtonHandler::updateButtonHandler()
{
  // Read out buttons
  int reading1 = digitalRead(_m_buttonUpPin);
  int reading2 = digitalRead(_m_buttonEnterPin);
  int reading3 = digitalRead(_m_buttonDownPin);

  // If the switch changed, due to noise or pressing:
  if (reading1 != lastbuttonStateUp) {
    // reset the debouncing timer
    lastDebounceTimeUp = millis();
  }
  // If the switch changed, due to noise or pressing:
  if (reading2 != lastbuttonStateEnter) {
    // reset the debouncing timer
    lastDebounceTimeEnter = millis();
  }

  // If the switch changed, due to noise or pressing:
  if (reading3 != lastbuttonStateDown) {
    // reset the debouncing timer
    lastDebounceTimeDown = millis();
  }
  

   if ((millis() - lastDebounceTimeUp) > DEBOUNCE_DELAY) {
    // whatever the reading is at, it's been there for longer
    // than the debounce delay, so take it as the actual current state:

    // if the button state has changed:
    if (reading1 != buttonStateUp) {
      buttonStateUp = reading1;

      if (buttonStateUp == LOW) {
        buttonUp = 1;
      }
    }
  }

  if ((millis() - lastDebounceTimeEnter) > DEBOUNCE_DELAY) {
    // whatever the reading is at, it's been there for longer
    // than the debounce delay, so take it as the actual current state:

    // if the button state has changed:
    if (reading2 != buttonStateEnter) {
      buttonStateEnter = reading2;

      if (buttonStateEnter == LOW) {
        buttonEnter = 1;
      }
    }
  }

  if ((millis() - lastDebounceTimeDown) > DEBOUNCE_DELAY) {
    // whatever the reading is at, it's been there for longer
    // than the debounce delay, so take it as the actual current state:

    // if the button state has changed:
    if (reading3 != buttonStateDown) {
      buttonStateDown = reading3;

      if (buttonStateDown == LOW) {
        buttonDown = 1;
      }
    }
  }

  lastbuttonStateUp 	= reading1;
  lastbuttonStateEnter 	= reading2;
  lastbuttonStateDown 	= reading3;
}
