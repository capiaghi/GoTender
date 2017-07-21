/// ****************************************************************************
/// \file      GoTender.cpp
///
/// \brief     main code
///
/// \details   GoTender
///
/// \author    Christoph Capiaghi, Anne Liebold
///
/// \version   0.1
///
/// \date      20170430
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
/// \todo     - A lot to do
///
// ****************************************************************************
// Includes *******************************************************************


#include <SD.h>                   // SD Card Library (not used yet)

// Own Includes:
#include "Config.h"                 // Configuration, true false statement
#include "ButtonHandler.h"          // Read out Buttons
#include "Startup.h"                // Startup sequence
#include "PinMapping.h"             // All Pin Definitons
#include "Display.h"                // Display Handler
#include "Potentiometers.h"         // Read out Potentiometers Meat and Oven
#include "Controller.h"             // Controls the run state

// Private types **************************************************************
/// \brief Used States
/// \details States for the main state machine TBD
typedef enum stm_state_e
{
  STM_STATE_STARTUP,               /// Reserved and implemented for later: EEPROM loading
  STM_TIME_AND_DATE,               /// Set Hour, min, day, month and year (optional state)
  STM_STATE_SETTINGS,              /// Set Temperatures, smoker, timer yes or no
  STM_STATE_TIMER,                 /// Set Timer (optional state)
  STM_STATE_SUMMARY,               /// Shows all values. Clearance for start
  STM_STATE_RUN,                   /// Runs the Oven
  STM_STATE_FINISHED,              /// Meal finished
  STM_STATE_ERROR,                 /// Starts LED Test (small LEDs, red and green)
} stm_state_t;

typedef unsigned char stm_bool_t; 

//TBD

/// \brief Used SubStates
/// \details States for the settings state machine
typedef enum stm_substate_e
{
   STM_SUBSTATE_SET_TIME,                 /// RTC Time and Date
   STM_SUBSTATE_SET_TEMPERATURE_OVEN,     /// Set temperature
   STM_SUBSTATE_SET_TEMPERATURE_MEAT,
   STM_SUBSTATE_SET_SMOKER,               /// Enable or disable smoker
   STM_SUBSTATE_DONE,                     /// Next main state
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
} stm_substate_time_t;


//TBD END


// Private constants **********************************************************
const char       SOFTWARE_VERSION[10]  = "V0.1";
//const uint16_t   UART_SPEED            = 115200;
#define				UART_SPEED				( 115200 )


// Private macros *************************************************************


// Static variables ***********************************************************

static stm_state_t            stm_actState;    // Actual State variable
static stm_state_t            stm_newState;    // New State variable
static stm_bool_t             stm_entryFlag;   // Flag for handling the entry action
static stm_bool_t             stm_exitFlag;    // Flag for handling the exit action

static stm_bool_t             stm_sub_entryFlag;

static stm_substate_t         stm_SubState;
static stm_substate_time_t    stm_time_SubState;



// Private function prototypes ***********************************************


// Private functions **********************************************************

static uint16_t   value = 0;
static uint8_t    nextState = 0;
static uint16_t   oldValue = 0;


// Initialization **********************************************************
// The setup function runs once when you press reset or power the board
void setup() {
   // Initialize LEDs
   pinMode(GREEN_LED_PIN, OUTPUT);
   pinMode(RED_LED_PIN, OUTPUT);
   pinMode(YELLOW_LED_PIN, OUTPUT);

   // Initialize Buttons
   initButton();

   // Initialize Relais
   //initRelais();


   // Initialize state machine
   stm_entryFlag = TRUE;
   stm_sub_entryFlag  = TRUE;
   stm_exitFlag = FALSE;
   stm_newState = STM_STATE_STARTUP;
   stm_actState = STM_STATE_STARTUP;

   // Initialize Serial Port
   Serial.begin( UART_SPEED );

   // Initialize Display
   initDisplay();

   // wait for MAX chip to stabilize
   delay(500);
}

// Loop **********************************************************
// The loop function runs over and over again forever
void loop() {


// if(getButtonStateSmoker())
//{
//}
	

   updateButtonHandler();

   switch (stm_actState)
   {
      //==============================================================================
      // STM_STATE_STARTUP
      //==============================================================================
      case STM_STATE_STARTUP:
         /// - Path 1 \n
         /// Startup:
         /// Initialize Thermocouples
         /// Check for Errors

         // Entry action
         if (stm_entryFlag == TRUE)
         {
            #ifdef DEBUG
             Serial.println(F("Entered STM_STATE_STARTUP"));
            #endif
              
            displayTitle("Go Tender");
            displayCommand("Checking Sys");
            Serial.print("Software Version: ");
            Serial.println(SOFTWARE_VERSION);
            delay(500);
              
            // Startup
            if(startup())
            {
              Serial.println(F("Startup ok"));
              displayCommand("Sys ok");
            }
            else
            {
              Serial.println(F("Startup not ok"));
              displayCommand("Sys not ok: Exy");
              // TBD: Do something
            }
               

            delay(2000);
            
            displayTitle("Welcome");
            
            
            delay(1000);
            
            clearButtonAllFlags();
            displayCommand("Set Time?");
            displayCommandSmall1("Set Time: Enter");
            displayCommandSmall2("Next: Down");

            displayDate();

            stm_entryFlag = FALSE;
         }
         
         displayTemperatures();
         
         
         

         if(getButtonStateEnter())
         {
            stm_newState = STM_TIME_AND_DATE;
            stm_exitFlag = TRUE;
         }

         if(getButtonStateDown())
         {
            stm_newState = STM_STATE_SETTINGS;
            stm_exitFlag = TRUE;
         }


         
         
         // Exit
         if (stm_exitFlag == TRUE)
         {
           // do exit action
           clearButtonAllFlags();
           stm_exitFlag = FALSE;
           stm_actState = stm_newState;
           stm_entryFlag = TRUE;
         }
      break;
      
      
      //==============================================================================
      // STM_TIME_AND_DATE
      //==============================================================================
      case STM_TIME_AND_DATE:
      
         // Entry action
         if (stm_entryFlag == TRUE)
         {        
           #ifdef DEBUG
             Serial.println(F("Entered STM_TIME_AND_DATE"));
           #endif
           stm_time_SubState = STM_SUBSTATE_SET_HOUR;
           value = 0;
           clearButtonAllFlags();
           stm_entryFlag = FALSE;
         }
      

         switch(stm_time_SubState)
         {
            case STM_SUBSTATE_SET_HOUR:
               #ifdef DEBUG
                  Serial.println(F("STM_SUBSTATE_SET_HOUR"));
               #endif
               if(stm_sub_entryFlag == TRUE)
               {
                  displayTitle("SET_HOUR");
                  testDisplayOutput(value);
                  stm_sub_entryFlag = FALSE;
               }                        
               if(value > 23) value = 0;
               // Show value on display
               
               // Display only, if the value changes
               if (value != oldValue)
               {
                  testDisplayOutput(value);
                  oldValue = value;
               }
               
               if(nextState == 1)
               {
                  setTimeHour(value);
                  value = 1;
                  nextState = 0;
                  stm_sub_entryFlag = TRUE;
                  stm_time_SubState = STM_SUBSTATE_SET_MIN;
               }
            break;

            case STM_SUBSTATE_SET_MIN:
               #ifdef DEBUG
                  Serial.println(F("STM_SUBSTATE_SET_MIN"));
               #endif
               if(stm_sub_entryFlag == TRUE)
               {
                  displayTitle("SET_MIN");
                  testDisplayOutput(value);
                  stm_sub_entryFlag = FALSE;
               }  
               if(value > 59) value = 0;
               // Show value on display
               // Display only, if the value changes
               if (value != oldValue)
               {
                  testDisplayOutput(value);
                  oldValue = value;
               }
               
               
               if(nextState == 1)
               {
                  setTimeMin(value);
                  value = 1;
                  nextState = 0;
                  stm_sub_entryFlag = TRUE;
                  stm_time_SubState = STM_SUBSTATE_SET_DAY;
               }
            break;

            case STM_SUBSTATE_SET_DAY:
               #ifdef DEBUG
                  Serial.println(F("STM_SUBSTATE_SET_DAY"));
               #endif
               if(stm_sub_entryFlag == TRUE)
               {
                  displayTitle("SET_DAY");
                  testDisplayOutput(value);
                  stm_sub_entryFlag = FALSE;
               }  
               if(value > 31) value = 1;
               
               // Display only, if the value changes
               if (value != oldValue)
               {
                  testDisplayOutput(value);
                  oldValue = value;
               }
               
               if(nextState == 1)
               {
                  setTimeDay(value);
                  value = 1;
                  nextState = 0;
                  stm_sub_entryFlag = TRUE;
                  stm_time_SubState = STM_SUBSTATE_SET_MONTH;
               }
            break;

            case STM_SUBSTATE_SET_MONTH:
               #ifdef DEBUG
                  Serial.println(F("STM_SUBSTATE_SET_MONTH"));
               #endif
              // Show value on display
               if(stm_sub_entryFlag == TRUE)
               {
                  displayTitle("SET_MONTH");
                  testDisplayOutput(value);
                  stm_sub_entryFlag = FALSE;
               }  
               
               if(value > 12) value = 0;
               
               // Display only, if the value changes
               if (value != oldValue)
               {
                  testDisplayOutput(value);
                  oldValue = value;
               }

               if(nextState == 1)
               {
                  setTimeMonth(value);
                  value = 2017;
                  nextState = 0;
                  stm_sub_entryFlag = TRUE;
                  stm_time_SubState = STM_SUBSTATE_SET_YEAR;
               }
            break;

            case STM_SUBSTATE_SET_YEAR:
               #ifdef DEBUG
                  Serial.println(F("STM_SUBSTATE_SET_YEAR"));
               #endif
               if(stm_sub_entryFlag == TRUE)
               {
                  displayTitle("SET_YEAR");
                  testDisplayOutput(value);
                  stm_sub_entryFlag = FALSE;
               }  
               // Show value on display
               
               // Display only, if the value changes
               if (value != oldValue)
               {
                  testDisplayOutput(value);
                  oldValue = value;
               }
               if(nextState == 1)
               {
                  setTimeYear(value); 
                  nextState = 0;
                  stm_sub_entryFlag = TRUE;
                  stm_exitFlag = TRUE;
                  stm_newState = STM_STATE_SETTINGS;
                  
               }
            break;
         }

         if(getButtonStateUp())
         {
            value++;
         }

         if(getButtonStateEnter())
         {
            nextState = 1;
         }

         if(getButtonStateDown())
         {
            value--;
         }
         
         #ifdef DEBUG
            Serial.print("Value: ");
            Serial.println(value);
         #endif
         
         
         // Exit
         if (stm_exitFlag == TRUE)
         {
            // do exit action
            clearButtonAllFlags();
            stm_exitFlag = FALSE;
            stm_actState = stm_newState;
            stm_entryFlag = TRUE;
         }

      break;

      

      //==============================================================================
      // STM_STATE_SETTINGS
      //==============================================================================
      case STM_STATE_SETTINGS:
      /// - Path 2 \n
      /// Settings: Time, Oven Temperature, meat temperature, smoker on / off

         // Entry action
         if (stm_entryFlag == TRUE)
         {        
           #ifdef DEBUG
             Serial.println(F("Entered STM_STATE_SETTINGS"));
           #endif
           stm_SubState = STM_SUBSTATE_SET_TEMPERATURE_OVEN;
           clearButtonAllFlags();
           stm_entryFlag = FALSE;
         }
         //==============================================================================
         // SUB STATES SETTINGS
         // State machine with Time, Temperature, Finished
         switch (stm_SubState)
         {
               
            case STM_SUBSTATE_SET_TEMPERATURE_OVEN:

               
            if(stm_sub_entryFlag == TRUE)
            {
               #ifdef DEBUG
                  Serial.println(F("STM_SUBSTATE_SET_TEMPERATURE_OVEN"));
               #endif
               displayTitle("TEMP OVEN");
               testDisplayOutput(value);
               stm_sub_entryFlag = FALSE;
            }  
            
            value = getPotentiometerOvenVal();
            // Display only, if the value changes
            if (value != oldValue)
            {
               testDisplayOutput(value);
               Serial.println(value);  
               oldValue = value;
            }

                         

            if(getButtonStateEnter())
            {
               setTemperatureOvenSetPoint(value); //TBD : Tests. Warning: Sets double value
               stm_sub_entryFlag = TRUE;
               stm_SubState = STM_SUBSTATE_SET_TEMPERATURE_MEAT;
            }

            break;

            case STM_SUBSTATE_SET_TEMPERATURE_MEAT:
               
               if(stm_sub_entryFlag == TRUE)
               {
                  #ifdef DEBUG
                     Serial.println(F("STM_SUBSTATE_SET_TEMPERATURE_MEAT"));
                  #endif
                  displayTitle("TEMP MEAT");
                  testDisplayOutput(value);
                  stm_sub_entryFlag = FALSE;
               }   
               
               value = getPotentiometerMeatVal();
               // Display only, if the value changes
               if (value != oldValue)
               {
                  testDisplayOutput(value);
                  Serial.println(value);  
                  oldValue = value;
               }

               if(getButtonStateEnter())
               {
                  stm_sub_entryFlag = TRUE;
                  setTemperatureMeatSetPoint(value); //TBD : Tests. Warning: Sets double value
                  stm_SubState = STM_SUBSTATE_SET_SMOKER;
               }
            break;

            case STM_SUBSTATE_SET_SMOKER:
            
               if(stm_sub_entryFlag == TRUE)
               {
                  #ifdef DEBUG
                     Serial.println(F("STM_SUBSTATE_SET_SMOKER"));
                  #endif
                  displayTitle("Set Smoker");
                  displayCommandSmall1("Smoker yes: Press enter");
                  displayCommandSmall2("Smoker no: Press down");
                  stm_sub_entryFlag = FALSE;
               }   
               


               if(getButtonStateEnter())
               {
                  setSmoker(); // TBD test
                  stm_sub_entryFlag = TRUE;

                  stm_SubState = STM_SUBSTATE_DONE;
               }
               if(getButtonStateDown())
               {
                  resetSmoker(); // TBD test
                  stm_sub_entryFlag = TRUE;
                  stm_SubState = STM_SUBSTATE_DONE;
               }
               
            break;
            
            
            case STM_SUBSTATE_DONE:
               if(stm_sub_entryFlag == TRUE)
               {
                  #ifdef DEBUG
                     Serial.println(F("STM_SUBSTATE_DONE"));
                  #endif
                  displayTitle("Check");
                  displayCommand("Check input");
                  displayCommandSmall1("Okay: Press enter");
                  displayCommandSmall2("Again: Press down");
                  displaySmokerState();

                  clearButtonAllFlags();
                  stm_sub_entryFlag = FALSE;
               }  
               
               
               displayTemperatures();

               
               
               
               if(getButtonStateEnter())
               {
                  stm_newState = STM_STATE_TIMER;
                  stm_SubState = STM_SUBSTATE_SET_TEMPERATURE_OVEN;
                  stm_sub_entryFlag = TRUE;
                  stm_exitFlag = TRUE; 
               }
               if(getButtonStateDown())
               {
                  stm_newState = STM_STATE_SETTINGS;
                  stm_SubState = STM_SUBSTATE_SET_TEMPERATURE_OVEN;
                  stm_sub_entryFlag = TRUE;
                  stm_exitFlag = TRUE; 
               }
               
  
            break;
      }
         //==============================================================================
         // END SUBSTATES SETTINGS

         // Exit
         if (stm_exitFlag == TRUE)
         {
            // do exit action
            clearButtonAllFlags();
            stm_exitFlag = FALSE;
            stm_actState = stm_newState;
            stm_entryFlag = TRUE;
         }
      break;
      


      //==============================================================================
      // STM_STATE_TIMER
      //==============================================================================
      case STM_STATE_TIMER:
      /// - Path 3 \n
      /// Settings: Oven Temperature, meat temperature, smoker on / off

         // Entry action
         if (stm_entryFlag == TRUE)
         {
            #ifdef DEBUG
               Serial.println(F("Entered STM_STATE_TIMER"));
            #endif      
            displayTitle("TIMER");
            displayCommandSmall1("Not yet supported");
            stm_entryFlag = FALSE;
         }


         if(getButtonStateUp())
         {
            stm_newState = STM_STATE_SETTINGS;
            stm_exitFlag = TRUE;        
         }

         if(getButtonStateEnter())
         {

           
         }

         if(getButtonStateDown())
         {
            stm_newState = STM_STATE_SUMMARY;
            stm_exitFlag = TRUE;        
         }

         // Exit
         if (stm_exitFlag == TRUE)
         {
            // do exit action
            stm_exitFlag = FALSE;
            stm_actState = stm_newState;
            stm_entryFlag = TRUE;
         }

         // Exit
         if (stm_exitFlag == TRUE)
         {
            // do exit action
            clearButtonAllFlags();
            stm_exitFlag = FALSE;
            stm_actState = stm_newState;
            stm_entryFlag = TRUE;
         }
         break;
         
         //==============================================================================
         // STM_STATE_SUMMARY
         //==============================================================================
         case STM_STATE_SUMMARY:
            // Entry action
            if (stm_entryFlag == TRUE)
            {
               #ifdef DEBUG
                  Serial.println(F("Entered STM_STATE_SUMMARY"));
               #endif      
               displayTitle("Summary");
               displayCommand("Check input");
               displayCommandSmall1("Start: Press enter");
               displayCommandSmall2("Not Okay: Press up");
               displayRefresh(); 
               stm_entryFlag = FALSE;
            }
            
            
            displayTemperatures();
            
            
            if(getButtonStateUp())
            {
               stm_newState = STM_STATE_SETTINGS;
               stm_exitFlag = TRUE;        
            }
            
            if(getButtonStateEnter())
            {
               stm_exitFlag = TRUE;
               stm_newState = STM_STATE_RUN;
            }
            
            
            // Exit
            if (stm_exitFlag == TRUE)
            {
               // do exit action
               clearButtonAllFlags();
               stm_exitFlag = FALSE;
               stm_actState = stm_newState;
               stm_entryFlag = TRUE;
            }
         
         break;   
         
         

      //==============================================================================
      // STM_STATE_RUN
      //==============================================================================
      case STM_STATE_RUN:
      /// - Path 4 \n
      /// Settings: Oven Temperature, meat temperature, smoker on / off
         // Entry action
         if (stm_entryFlag == TRUE)
         {
            #ifdef DEBUG
               Serial.println(F("Entered STM_STATE_RUN"));
            #endif      
            displayTitle("RUN");
            stm_entryFlag = FALSE;
         }
         
         
         // LOOP
         displayTemperatures();
         
         
         if(getButtonStateUp())
         {
            stm_newState = STM_STATE_TIMER;
            stm_exitFlag = TRUE;        
         }

         if(getButtonStateEnter())
         {
           
         }

         if(getButtonStateDown())
         {
            stm_newState = STM_STATE_FINISHED;
            stm_exitFlag = TRUE;        
         }

         // Exit
         if (stm_exitFlag == TRUE)
         {
            // do exit action
            clearButtonAllFlags();
            stm_exitFlag = FALSE;
            stm_actState = stm_newState;
            stm_entryFlag = TRUE;
         }

      break;

      //==============================================================================
      // STM_STATE_FINISHED
      //==============================================================================
      case STM_STATE_FINISHED:
      /// - Path 5 \n
      /// Settings: Oven Temperature, meat temperature, smoker on / off

         if (stm_entryFlag == TRUE)
         {
            #ifdef DEBUG
               Serial.println(F("Entered STM_STATE_FINISHED"));
            #endif     
            displayTitle("FINISHED");            
            stm_entryFlag = FALSE;
         }


         if(getButtonStateUp())
         {
            stm_newState = STM_STATE_RUN;
            stm_exitFlag = TRUE;        
         }

         if(getButtonStateEnter())
         {
            
         }

         if(getButtonStateDown())
         {
            
         }

         // Exit
         if (stm_exitFlag == TRUE)
         {
            // do exit action
            clearButtonAllFlags();
            stm_exitFlag = FALSE;
            stm_actState = stm_newState;
            stm_entryFlag = TRUE;
         }

      break;

      //==============================================================================
      // STM_STATE_ERROR
      //==============================================================================
      case STM_STATE_ERROR:
      /// - Path 6 \n
      /// Error Handler

         if (stm_entryFlag == TRUE)
         {
            #ifdef DEBUG
               Serial.println(F("Entered STM_STATE_ERROR"));
            #endif
            displayTitle("!!!ERROR!!!");             
            stm_entryFlag = FALSE;
         }

      break;

      //==============================================================================
      // DEFAULT
      //==============================================================================

      default:
         //Error: Change to Error state
         #ifdef DEBUG
            Serial.println(F("Entered default"));
         #endif  
         stm_actState = STM_STATE_ERROR;
      break;
      }




    // Safety

    // if( ...)
    // Alles aus





      
      displayTime(); // TBD: Not every cycle

      
/*   analogWrite(PWM_PIN, 64); // 0 ... 255

  // read the input on analog pin 0:
  int sensorValue = analogRead(AIN0_PIN);
  String sensorValueStr = String(sensorValue);
 */

}
