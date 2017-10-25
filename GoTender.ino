

/// ****************************************************************************
/// \file      GoTender.cpp
///
/// \brief     main code
///
/// \details   GoTender
///
/// \author    Christoph Capiaghi, Anne Liebold
///
/// \version   0.8
///
/// \date      20170825
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
/// \todo     - Testing
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
//#include "Countimer.h"              // Counter von GitHub
//#include "TimerThree.h"               // für PWM Signal
//#include "FlexiTimer2.h"            // für PWM Signal

// Private types **************************************************************
/// \brief Used States
/// \details States for the main state machine TBD
typedef enum stm_state_e
{
  STM_STATE_STARTUP,               /// Reserved and implemented for later: EEPROM loading
  STM_TIME_AND_DATE,               /// Set Hour, min, day, month and year (optional state)
  STM_STATE_SETTINGS,              /// Set Temperatures, smoker, timer yes or no
  STM_STATE_SET_TIMER,             /// Set Timer (optional state)
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
   STM_SUBSTATE_SET_TIMER,                /// thet clock
   STM_SUBSTATE_DONE,                     /// Next main state
} stm_substate_t;

/// \brief Used SubStates time
/// \details States for the settings state machine
typedef enum stm_substate_time_e
{
   STM_SUBSTATE_SET_HOUR,           /// RTC Hour
   STM_SUBSTATE_SET_MIN,            /// RTC Min
   STM_SUBSTATE_SET_HOUR_TIMER,     /// RTC Hour TIMER
   STM_SUBSTATE_SET_MIN_TIMER,      /// RTC Min TIMER
   STM_SUBSTATE_SET_DAY,            /// RTC Day
   STM_SUBSTATE_SET_MONTH,          /// RTC Month
   STM_SUBSTATE_SET_YEAR,           /// RTC YEAR
} stm_substate_time_t;


typedef enum stm_subState_control_e
{
   STM_SUBSTATE_HEATING,            /// Temperature < SetTemperature - LOWER_BOUND
   STM_SUBSTATE_PULSING,            /// Temperature < SetTemperature + UPPER_BOUND
   STM_SUBSTATE_OFF,                /// Temperature > UPPER_BOUND
   STM_SUBSTATE_WAIT                /// Cooling down till temperature < SetTemperature - LOWER_BOUND
   
} stm_subState_control_t;



#define UPPER_LEVEL_TEMP      ( 4 )    // SET_POINT + UPPER_LEVEL_TEMP        
#define LOWER_LEVEL_TEMP      ( 5 )    // SET_POINT - LOWER_LEVEL_TEMP


#define HEATER_ON_TIME_S      ( 30 )    // in seconds
#define HEATER_OFF_TIME_S     ( 10*60 ) // 
#define SMOKER_ON_TIME_S      ( 40 )    // 
#define SMOKER_OFF_TIME_S     ( 7*60 )  // 

#define MAX_SMOKER_TIME_S     ( 80 )    // Safety system: Smoker is maximum 80 seconds allowed

#define NUM_OF_SMOKER_CYCLES_BUTTON ( 1 ) // After pressing the button, 1 smoker cycle is executed

uint8_t  numOfSmokerCycles          = 7;   // Predefined vaule for number of smoker cycle. Switch override this value with NUM_OF_SMOKER_CYCLES_BUTTON.
uint8_t  currentNumOfSmokerCycles   = 0;   // used to determine how many pulses the smoker does
uint16_t maxSmokerTimeS             = 0;
uint16_t pulsingCounterS            = 0;
uint16_t pulsingCounterSmokerS      = 0;
uint16_t LowerLevelHeater           = 0;
uint16_t UpperLevelHeater           = 0;
bool first                          = true;

// Temperature profile
//     HEATER OFF
// ------------------------------------------------------ setPoint + UPPER_LEVEL_TEMP
//     PULSING: 40 s on / 5 min off AND SMOKER CONTROL
// ------------------------------------------------------ setPoint
//     PULSING: 40 s on / 5 min off AND SMOKER CONTROL
// ------------------------------------------------------ setPoint - LOWER_LEVEL_TEMP
//     HEATER ON



// Private constants **********************************************************
const char        SOFTWARE_VERSION[10]  = "V0.8";
#define				UART_SPEED				( 115200 )
#define           MAX_TEMPERATURE_OVEN ( 300 )
#define           MAX_TEMPERATURE_MEAT ( 300 )


// Private macros *************************************************************


// Static variables ***********************************************************

static stm_state_t            stm_actState;    // Actual State variable
static stm_state_t            stm_newState;    // New State variable
static stm_bool_t             stm_entryFlag;   // Flag for handling the entry action
static stm_bool_t             stm_exitFlag;    // Flag for handling the exit action

static stm_bool_t             stm_sub_entryFlag;

static stm_substate_t         stm_SubState;
static stm_substate_time_t    stm_SubState_time;
static stm_subState_control_t stm_subState_control;


// Private function prototypes ***********************************************


// Private functions **********************************************************

static uint16_t   value       = 0;
static uint8_t    nextState   = 0;
static uint16_t   oldValue    = 0;
static bool       timerSet    = false;
static bool       sysOkay     = true;




// ISR **********************************************************
// ----------------------------------------------------------------------------
/// \brief     Timer 3 interrupt 
/// \detail    For measurement: Every 1 s an interrupt
/// \warning   
/// \return   
/// \todo      
///
uint8_t sysTick;
ISR(TIMER3_COMPA_vect)
{
   sysTick = 1;
}

/// \brief     Timer 4 interrupt 
/// \detail    For measurement: Every 100 ms an interrupt
/// \warning   
/// \return   
/// \todo      
///
uint8_t sysTickFast;
ISR(TIMER4_COMPA_vect)
{
   sysTickFast = 1;
}







// Initialization **********************************************************
// The setup function runs once when you press reset or power the board


void setup() {
  
   // Timer for Controller
   // Init timer 3, interrupts every second
   TCCR3A = 0;
   TCCR3B = 0xc;     // CTC mode, prescaler = 256+ (clk/256 (From prescaler)

   OCR3A = 62500;    // 16 MHz / 256 = 62500 gives exactly one second (2 Output Compare Regis
   TIMSK3 |= 0x02;   // Enable output compare match ints
   
   
   // Init timer 4, interrupts every 100 ms
   TCCR4A = 0;
   TCCR4B = 0xc;
   
   OCR4A = 6250;      // 16 MHz / 256 = 62500 gives exactly one second (2 Output Compare Register) -> 6250: 100 ms
   
   TIMSK4 |= 0x02;   // Enable output compare match ints
   
   
   sei(); // Enable interrupts

  
   // Initialize LEDs
   pinMode(GREEN_LED_PIN, OUTPUT);
   pinMode(RED_LED_PIN, OUTPUT);
   pinMode(YELLOW_LED_PIN, OUTPUT);

   // Initialize Buttons
   initButton();

   // Initialize Relais
   pinMode(RELAIS_HEATER, OUTPUT);
   pinMode(RELAIS_SMOKER, OUTPUT);

   
   // Initialize Serial Port
   Serial.begin( UART_SPEED );

   // Initialize state machine
   stm_entryFlag = TRUE;
   stm_sub_entryFlag  = TRUE;
   stm_exitFlag = FALSE;
   stm_newState = STM_STATE_STARTUP;
   stm_actState = STM_STATE_STARTUP;



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
            delay(800);
              
            // Startup
            if(startup())
            {
              Serial.println(F("Startup ok"));
              displayCommand("Sys ok");
            }
            else
            {
              Serial.println(F("Startup not ok"));
              displayCommand("Sys not ok");
              // TBD: Do something
            }
               

            delay(2000);
            
            displayTitle("Welcome");
            
            
            delay(2000);
            
            clearButtonAllFlags();
            displayCommand("Set Time?");
            displayCommandSmall1("Set Time: Press Enter");
            displayCommandSmall2("Next:     Press Down");

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
           stm_SubState_time = STM_SUBSTATE_SET_HOUR;
           value = 0;
           clearButtonAllFlags();
           stm_entryFlag = FALSE;
         }
      

         switch(stm_SubState_time)
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
                  stm_SubState_time = STM_SUBSTATE_SET_MIN;
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
                  stm_SubState_time = STM_SUBSTATE_SET_DAY;
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
                  stm_SubState_time = STM_SUBSTATE_SET_MONTH;
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
                  stm_SubState_time = STM_SUBSTATE_SET_YEAR;
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
                  setTemperatureOvenSetPoint( value ); //TBD : Tests. Warning: Sets double value
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
                  testDisplayOutput( value );
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
                  setTemperatureMeatSetPoint( value ); //TBD : Tests. Warning: Sets double value
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
                  displayCommandSmall1("Smoker Yes: Press Enter");
                  displayCommandSmall2("Smoker No: Press Down");
                  stm_sub_entryFlag = FALSE;
               }   
               


               if(getButtonStateEnter())
               {
                  armSmoker( true );
                  currentNumOfSmokerCycles = 0;
                  pulsingCounterSmokerS = 0;                  
                  stm_sub_entryFlag = TRUE;
                  stm_SubState = STM_SUBSTATE_SET_TIMER;
               }
               if(getButtonStateDown())
               {
                  armSmoker( false ); 
                  stm_sub_entryFlag = TRUE;
                  stm_SubState = STM_SUBSTATE_SET_TIMER;
               }
               
            break;
			
			
			case STM_SUBSTATE_SET_TIMER:
			
			   if(stm_sub_entryFlag == TRUE)
               {
                  #ifdef DEBUG
                     Serial.println(F("STM_SUBSTATE_SET_TIMER"));
                  #endif
                  displayTitle("Timer");
                  displayCommandSmall1("Set Timer: Press Enter");
                  displayCommandSmall2("Next:      Press Down");
                  displaySmokerState();
                  clearButtonAllFlags();
                  stm_sub_entryFlag = FALSE;
               }   
			   
			   
			  if(getButtonStateEnter())
               {
                  timerSet     = true; // Yes, timer is set 
                  stm_newState = STM_STATE_SET_TIMER;
                  stm_sub_entryFlag = TRUE;
                  stm_SubState = STM_SUBSTATE_DONE;
                  stm_exitFlag = TRUE; 
               }
               if(getButtonStateDown())
               {
                  timerSet     = false;   // No, timer is disabled. Do not use timer
                  setTimerHour(0);
                  setTimerMin(0);
				  
                  stm_newState = STM_STATE_SUMMARY;
                  stm_sub_entryFlag = TRUE;
                  stm_SubState = STM_SUBSTATE_DONE;
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
      // STM_STATE_SET_TIMER
      //==============================================================================
      case STM_STATE_SET_TIMER:
      /// - Path 3 \n
      /// Sets timer

         // Entry action
         if (stm_entryFlag == TRUE)
         {
            #ifdef DEBUG
               Serial.println(F("Entered STM_STATE_SET_TIMER"));
            #endif      
            stm_SubState_time = STM_SUBSTATE_SET_HOUR_TIMER;
            nextState = 0;
            value = 0;
            stm_entryFlag = FALSE;
            clearButtonAllFlags();
         }
		 
         switch(stm_SubState_time)
         {
            case STM_SUBSTATE_SET_HOUR_TIMER:
               #ifdef DEBUG
                  Serial.println(F("STM_SUBSTATE_SET_HOUR_TIMER"));
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
                  setTimerHour(value);
                  value = 1;
                  nextState = 0;
                  stm_sub_entryFlag = TRUE;
                  stm_SubState_time = STM_SUBSTATE_SET_MIN_TIMER;
               }
            break;

            case STM_SUBSTATE_SET_MIN_TIMER:
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
                  setTimerMin(value);
                  nextState = 0;
                  stm_sub_entryFlag = TRUE;
                  stm_exitFlag = TRUE;
                  stm_newState = STM_STATE_SUMMARY;
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
               displayCommand("Check Input");
               displayCommandSmall1("GoTender Start: Press Enter");
               displayCommandSmall2("Again:          Press Up");
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
               first = true;
               stm_subState_control = STM_SUBSTATE_HEATING;
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
            displayRefresh();
            
            pulsingCounterS = 0;
            
            // Hyster
            UpperLevelHeater = getTemperatureOvenSetPoint() + UPPER_LEVEL_TEMP;
            LowerLevelHeater =  getTemperatureOvenSetPoint() - LOWER_LEVEL_TEMP;
            
            // Timer set?
            if ( timerSet )
            {
               startTimer(); // Reads actual time and calculates end time
               Serial.println(F("Starts Timer"));
            }
            
            stm_entryFlag = FALSE;
         }
         
         
         // Timer expired?
         if ( timerSet )
         {
            if ( timerExpired() )
            {
                stm_newState = STM_STATE_FINISHED;
                stm_exitFlag = TRUE;
                 #ifdef DEBUG
                    Serial.println(F("TIMER EXPIRED"));
                #endif     
            }
         }
         
         // LOOP
         displayTemperatures();
         

         // Every second
         if ( sysTick )
         {
            if ( getSmokerState() )
            {
               maxSmokerTimeS++;
            }
            else
            {
               maxSmokerTimeS = 0;
            }
            
            sysTick = 0;
         
            switch(stm_subState_control)
            {
               
               // Low temperature: Switch heater on, smoker is off.
               case STM_SUBSTATE_HEATING:
               
                  if ( first )
                  {
                     displayCommandSmall1("Heater On");
                     displayCommandSmall2("Smoker Off");
                     startHeater( true );
                     startSmoker( false );
                     
                     first = false;
                     #ifdef DEBUG
                        Serial.println(F("STM_SUBSTATE_HEATING"));
                     #endif   
                  }
                  
                  // Exit Condition
                  if ( getTemperatureOven() > LowerLevelHeater )
                  {
                     first = true;
                     stm_subState_control = STM_SUBSTATE_PULSING;
                     
                  }  
               break;
               
               // 40 s on, 5 min off
               case STM_SUBSTATE_PULSING:
               
                  switch( pulsingCounterS )
                  {
                     case 0: // Start
                        startHeater( true );
                        displayCommandSmall1("Pulse On");
                        pulsingCounterS++;
                        #ifdef DEBUG
                           Serial.println(F("STM_SUBSTATE_PULSING: Pulse On"));
                        #endif
                     break;
                     
                     
                     case HEATER_ON_TIME_S: // On time expired, switch off
                        startHeater( false );
                        displayCommandSmall1("Pulse Off");
                        pulsingCounterS++;
                        #ifdef DEBUG
                           Serial.println(F("STM_SUBSTATE_PULSING: Pulse Off"));
                        #endif 
                     break;
                     
                     case ( HEATER_OFF_TIME_S + HEATER_ON_TIME_S ):
                        pulsingCounterS = 0;
                     break;
                     
                     default:
                        pulsingCounterS++;
                     break;
                  }
                        
                  
                  // Exit Condition
                  if ( getTemperatureOven() >  UpperLevelHeater )
                  {
                     pulsingCounterS = 0;
                     stm_subState_control = STM_SUBSTATE_OFF;
                  }
                  
                  if ( getTemperatureOven() < LowerLevelHeater ) 
                  {
                     pulsingCounterS = 0;
                     stm_subState_control = STM_SUBSTATE_HEATING;
                  }
                  
                  
               break;
               
               case STM_SUBSTATE_OFF:
                  startHeater( false );
                  displayCommandSmall1("Heater Stop");
                  #ifdef DEBUG
                     Serial.println(F("STM_SUBSTATE_OFF: Heater off"));
                  #endif
                  stm_subState_control = STM_SUBSTATE_WAIT;
                  

                     
               break;
               
               case STM_SUBSTATE_WAIT:
                  #ifdef DEBUG
                     Serial.println(F("STM_SUBSTATE_WAIT: All off"));
                  #endif
                  // Exit
                  if ( getTemperatureOven() <= LowerLevelHeater )
                  {
                     stm_subState_control = STM_SUBSTATE_PULSING;
                  }
               break;
               
               
               default:
                  displayCommandSmall1("ERROR");
                  displayCommandSmall2("DEFAULT");
                  Serial.println(F("STM_SUBSTATE_DEFAULT"));
                  Serial.println(F("ERROR"));
               break;
               
            }
            
            
            // Smoke: Only if heater off, smoker armed and x times per Intervall
            if ( !getHeaterState() && getArmSmokerState() )
            {
               if ( pulsingCounterS != 0 )
               {
                  if ( currentNumOfSmokerCycles < numOfSmokerCycles )
                  {
                     if ( pulsingCounterSmokerS == 0) // Smoker On
                     {
                        startSmoker( true );
                        displayCommandSmall2("Smoker Start");
                        pulsingCounterSmokerS++;
                     }     
                     else if ( pulsingCounterSmokerS == SMOKER_ON_TIME_S ) // On time expired, switch off
                     {
                        startSmoker( false );
                        displayCommandSmall2("Smoker Stop");
                        pulsingCounterSmokerS++;
                     }
                     else if ( pulsingCounterSmokerS == ( SMOKER_ON_TIME_S + SMOKER_OFF_TIME_S ) )
                     {
                        pulsingCounterSmokerS = 0;
                        currentNumOfSmokerCycles++;
                     }
                     else
                     {
                        pulsingCounterSmokerS++;
                     }

                  }
                  else
                  {
                     startSmoker( false );
                     displayCommandSmall2("Smoker Stop");
                  }
               }
            }
            else
            {
               startSmoker( false );
            }
            
         }

         
         if( getButtonStateUp() )
         {
            stm_newState = STM_STATE_SETTINGS;
            stm_exitFlag = TRUE;        
         }

         if( getButtonStateEnter() )
         {
            
            stm_newState = STM_STATE_FINISHED;
            stm_exitFlag = TRUE;
         }

         if( getButtonStateDown() )
         {
            stm_newState = STM_STATE_SET_TIMER;
            stm_exitFlag = TRUE;        
         }
		 
         if ( getButtonStateSmoker() )
         {
            currentNumOfSmokerCycles = 0;
            numOfSmokerCycles = NUM_OF_SMOKER_CYCLES_BUTTON;
            armSmoker( true );
            Serial.println(F("ButtonStateSmoker"));
         }

         
         // Meat temperature reached
         if( getTemperatureMeat() >= getTemperatureMeatSetPoint() )
         {
            Serial.println(F("Reached Meat SetPoint"));
            stm_newState = STM_STATE_FINISHED;
            stm_exitFlag = TRUE;
         }
         
		 

         // Exit
         if (stm_exitFlag == TRUE)
         {
            // do exit action
            clearButtonAllFlags();
            startHeater( false ); // Switch off Heater
            startSmoker( false ); // Switch off Smoker
            
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
            displayCommand("GoTender Stop");

                     
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
            sysOkay = false;
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


   //==============================================================================
   // Safety system
   //==============================================================================
   if ( getTemperatureOven() > MAX_TEMPERATURE_OVEN)  sysOkay = false; // Overtemperature Oven
   if ( getTemperatureMeat() > MAX_TEMPERATURE_MEAT)  sysOkay = false; // Overtemperature Meat
   if ( getHeaterState() && getSmokerState() )        sysOkay = false; // Heater and Smoker both on
   if ( maxSmokerTimeS >= MAX_SMOKER_TIME_S )         sysOkay = false; // Smoker too long in ON state
  
      
      
   // Safety: If system is not okay -> Disable heaters
   if ( !sysOkay )
   {
      emergencyOff();
      displayTitle("EMERGENCY");
      displayCommand("GoTender Stop");
      digitalWrite(RED_LED_PIN, HIGH);
   }

   
   //==============================================================================
   // Display time and timer
   //==============================================================================
   
   if ( sysTickFast ) // TBD Testing
   {
      sysTickFast = 0;
      displayTime();
      displayTimer();
      
      if (getHeaterState() )
      {
         digitalWrite(GREEN_LED_PIN, HIGH);
      }
      else
      {
         digitalWrite(GREEN_LED_PIN, LOW);
      }
      
      if( getSmokerState() )
      {
         digitalWrite(YELLOW_LED_PIN, HIGH);
      }
      else
      {
         digitalWrite(YELLOW_LED_PIN, LOW);
      }
   }
      


}
