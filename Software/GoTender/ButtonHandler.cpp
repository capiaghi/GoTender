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
static uint8_t 	buttonStateUp = HIGH;
static uint8_t 	buttonStateEnter = HIGH;
static uint8_t 	buttonStateDown = HIGH;
static uint8_t  buttonStateSmoker = LOW;
// Last button state
static uint8_t 	lastbuttonStateUp = HIGH;
static uint8_t 	lastbuttonStateEnter = HIGH;
static uint8_t 	lastbuttonStateDown = HIGH;
static uint8_t  lastbuttonStateSmoker = LOW;
// Button pressed flag
static uint8_t 	buttonUpFlag = LOW;
static uint8_t 	buttonEnterFlag = LOW;
static uint8_t 	buttonDownFlag = LOW;
static uint8_t  buttonSmokerFlag = LOW;
 
// the following variables are long's because the time, measured in miliseconds,
// will quickly become a bigger number than can be stored in an int.
static long 		lastDebounceTimeUp = 0;  // the last time the output pin was toggled
static long 		lastDebounceTimeEnter = 0; 
static long 		lastDebounceTimeDown = 0; 
static long     lastDebounceTimeSmoker = 0;

// ----------------------------------------------------------------------------
/// \brief     Initialize buttosn
/// \detail    Turn on Pull-ups, define as input
/// \warning   
/// \return    void
/// \todo      
///
void initButton()
{
   pinMode(BUTTON_UP_PIN, INPUT_PULLUP);
   pinMode(BUTTON_ENTER_PIN, INPUT_PULLUP);
   pinMode(BUTTON_DOWN_PIN, INPUT_PULLUP);
   
   pinMode(BUTTON_SMOKER, INPUT);
}

// ----------------------------------------------------------------------------
/// \brief     Get state of Button Up
/// \detail    Gets the state of the button and resets the flag
/// \warning   
/// \return    state: 1 pressed, 0 not pressed
/// \todo      
///
uint8_t getButtonStateUp()
{
	if(buttonUpFlag == HIGH)
	{
		buttonUpFlag = LOW;
		return 1;
	}
	else
	{
		buttonUpFlag = LOW;
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
uint8_t getButtonStateEnter()
{
	if(buttonEnterFlag == HIGH)
	{
		buttonEnterFlag = LOW;
		return 1;
	}
	else
	{
		buttonEnterFlag = LOW;
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
uint8_t getButtonStateDown()
{
	if(buttonDownFlag == HIGH)
	{
		buttonDownFlag = LOW;
		return 1;
	}
	else
	{
		buttonDownFlag = LOW;
		return 0;
	}
}


// ----------------------------------------------------------------------------
/// \brief     Get state of Button Smoker
/// \detail    Gets the state of the button and resets the flag
/// \warning   
/// \return    state: 1 pressed, 0 not pressed
/// \todo      
///
uint8_t getButtonStateSmoker()
{
  if(buttonSmokerFlag == HIGH)
  {
    buttonSmokerFlag = LOW;
    return 1;
  }
  else
  {
    buttonSmokerFlag = LOW;
    return 0;
  }
}


void clearButtonAllFlags()
{
  buttonUpFlag      = LOW;
  buttonEnterFlag   = LOW;
  buttonDownFlag    = LOW;
  buttonSmokerFlag  = LOW;
}

// ----------------------------------------------------------------------------
/// \brief     Updates the flags of the buttons
/// \detail    Sets the flags of the 3 buttons
/// \warning   
/// \return    state: 1 pressed, 0 not pressed
/// \todo      
///
void updateButtonHandler()
{
  // Read out buttons
  int readingUp      = digitalRead(BUTTON_UP_PIN);
  int readingEnter   = digitalRead(BUTTON_ENTER_PIN);
  int readingDown    = digitalRead(BUTTON_DOWN_PIN);
  int readingSmoker  = digitalRead(BUTTON_SMOKER);

  // If the switch changed, due to noise or pressing:
  if (readingUp != lastbuttonStateUp) {
    // reset the debouncing timer
    lastDebounceTimeUp = millis();
  }
  // If the switch changed, due to noise or pressing:
  if (readingEnter != lastbuttonStateEnter) {
    // reset the debouncing timer
    lastDebounceTimeEnter = millis();
  }

  // If the switch changed, due to noise or pressing:
  if (readingDown != lastbuttonStateDown) {
    // reset the debouncing timer
    lastDebounceTimeDown = millis();
  }

    // If the switch changed, due to noise or pressing:
  if (readingSmoker != lastbuttonStateSmoker) {
    // reset the debouncing timer
    lastDebounceTimeSmoker = millis();
  }
  

   if ((millis() - lastDebounceTimeUp) > DEBOUNCE_DELAY) {
    // whatever the reading is at, it's been there for longer
    // than the debounce delay, so take it as the actual current state:

    // if the button state has changed:
    if (readingUp != buttonStateUp) {
      buttonStateUp = readingUp;

      if (buttonStateUp == LOW) {
        buttonUpFlag = HIGH;
      }
    }
  }

  if ((millis() - lastDebounceTimeEnter) > DEBOUNCE_DELAY) {
    // whatever the reading is at, it's been there for longer
    // than the debounce delay, so take it as the actual current state:

    // if the button state has changed:
    if (readingEnter != buttonStateEnter) {
      buttonStateEnter = readingEnter;

      if (buttonStateEnter == LOW) {
        buttonEnterFlag = HIGH;
      }
    }
  }

  if ((millis() - lastDebounceTimeDown) > DEBOUNCE_DELAY) {
    // whatever the reading is at, it's been there for longer
    // than the debounce delay, so take it as the actual current state:

    // if the button state has changed:
    if (readingDown != buttonStateDown) {
      buttonStateDown = readingDown;

      if (buttonStateDown == LOW) {
        buttonDownFlag = HIGH;
      }
    }
  }


    if ((millis() - lastDebounceTimeSmoker) > DEBOUNCE_DELAY) {
    // whatever the reading is at, it's been there for longer
    // than the debounce delay, so take it as the actual current state:

    // if the button state has changed:
    if (readingSmoker != buttonStateSmoker) {
      buttonStateSmoker = readingSmoker;

      if (buttonStateSmoker == HIGH) {
        buttonSmokerFlag = HIGH;
      }
    }
  }

  lastbuttonStateUp 	    = readingUp;
  lastbuttonStateEnter 	  = readingEnter;
  lastbuttonStateDown 	  = readingDown;
  lastbuttonStateSmoker   = readingSmoker;
}
