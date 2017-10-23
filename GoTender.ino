

/// ****************************************************************************
/// \file      GoTender.cpp
///
/// \brief     main code
///
/// \details   GoTender
///
/// \author    Christoph Capiaghi, Anne Liebold
///
/// \version   0.7
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
  STM_STATE_TIMER,                 /// Yes or No
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


//TBD END


// Private constants **********************************************************
const char       SOFTWARE_VERSION[10]  = "V0.7";
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
static stm_substate_time_t    stm_time_SubState;




// Private function prototypes ***********************************************




// Private functions **********************************************************

static uint16_t   value = 0;
static uint8_t    nextState = 0;
static uint16_t   oldValue = 0;
static bool       timerSet = false;
static bool       sysOkay = true;




// ISR **********************************************************
// ----------------------------------------------------------------------------
/// \brief     Timer interrupt 
/// \detail    For measurement: Every 10 ms an interrupt
/// \warning   
/// \return   
/// \todo      Check, if 10 ms is correct
///


uint8_t runMeasurement;
ISR(TIMER3_COMPA_vect)
{
   runMeasurement = 1;
}



// PWM *********************************************************************************
// -------------------------------------------------------------------------------------
/// \brief     PWM Signal
/// \detail    For 1000 ms PWM Signal
/// \warning   
/// \return   
/// \todo      Check, if its working
///

const int pwmPeriod = 6000;
float pwmDuty = 512;    // 50% der Periode ist ein - Periodendauer MAX für Timer3 =8.3s
float pwmDuty2 = 256;   // 25% der Periode ist ein
float newPwmDuty             =    0; // adc variable für


// All Values for TIME setting, 3 in total: Heater /Smoker /timer (input) **********************************

int LowerLevelHeater = 0;
int HigherLevelHeater = 0;

int LowerLevelSomker = 0;
int HigherLevelSomker = 0;


unsigned long previousMillisHeater = 0;
unsigned long currentMillisHeater = 0;

unsigned long previousMillisSmoker = 0;
unsigned long currentMillisSmoker = 0;

unsigned long previousMillisTimer = 0;
unsigned long currentMillisTimer = 0;


double intervalHighHeater = 10000; // modify so it is 40 second intervals - probably needs to be in ms 
double intervalLowHeater = 120000; // modify so it is 10 min intervals$ 

double intervalHighSmoker = 5000; // modify so it is 40 second intervals - probably needs to be in ms 
double intervalLowSmoker = 300000; // modify so it is 5 min intervals$ 

int smokerCounter = 0; //used to determine how many pulses the smoker does

boolean ovenState = false; //TODO: unless you can read ovenState you'll need to keep track everywhere it is changed
boolean smokerState = false; //TODO: unless you can read ovenState you'll need to keep track everywhere it is changed
boolean runSmoker = 0; // inside RUN state check variable, if smoker was set in SETTING state


//  Timer ****************************************************************************
// Variables for the timer

boolean runTimer = 0;




// Initialization **********************************************************
// The setup function runs once when you press reset or power the board


void setup() {


   //@Anne: Bitte kontrolliere folgenden Code
   // Er sollte alle 10 ms ein Timer-Interrupt auslösen (für deine Regelung)
   // und setzt ein Flag runMeasurement auf 1
   
   // Timer for Controller
   // Init timer 3, interrupts every second
   TCCR3A = 0;
   TCCR3B = 0xc;     // CTC mode, prescaler = 256+ (clk/256 (From prescaler)

   OCR3A = 625;      // 16 MHz / 256 = 62500 gives exactly one second (2 Output Compare Register)
                     // Speedup: 10 ms = 625

   TIMSK3 |= 0x02;   // Enable output compare match ints
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

   
    //Countdown
      // Count-down timer with 21s
   //  tDown.setCounter(0, 0, 21, tDown.COUNT_DOWN);
    // Call print_time2() method every 1s.
   //  tDown.setInterval(print_time2, 1000);



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
              displayCommand("Sys not ok: Exy");
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
                  displayCommandSmall1("Smoker Yes: Press Enter");
                  displayCommandSmall2("Smoker No: Press Down");
                  stm_sub_entryFlag = FALSE;
               }   
               


               if(getButtonStateEnter())
               {
                  armSmoker( true ); 
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
               Serial.println(F("Entered STM_STATE_TIMER"));
            #endif      
            stm_time_SubState = STM_SUBSTATE_SET_HOUR_TIMER;
            nextState = 0;
            value = 0;
            stm_entryFlag = FALSE;
            clearButtonAllFlags();
         }
		 
         switch(stm_time_SubState)
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
                  stm_time_SubState = STM_SUBSTATE_SET_MIN_TIMER;
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
			
			if( getArmSmokerState() )
			{
				runSmoker = 1;
			}
			else
			{
				runSmoker = 0;
			}
			

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
//%%%%%%%%%%%%%%%%%%%%% ANNE %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
/*
//2 Punkt Regler mit LED Anzeige
// Gelbe LED geht nicht :-(
// Wenn Relais_heater aus = LED = rot
// Wenn Temperatur Oven noch nicht erreicht -> Grüne LED Blinkt
// Wenn Temperatur Oven erreicht - bis zur Abbruchbedingung  - Grüne LED leuchtet konstant



          if((getTemperatureOven() > getTemperatureOvenSetPoint()) || (getTemperatureMeat() >= getTemperatureMeatSetPoint()))
          {
            #ifdef DEBUG
               Serial.println(F("heater Relais aus"));
            #endif
            startHeater( false );
            
            digitalWrite(RED_LED_PIN, HIGH);
            digitalWrite(YELLOW_LED_PIN, HIGH);
            digitalWrite(GREEN_LED_PIN, LOW);
         }
             else if(getTemperatureOven() < getTemperatureOvenSetPoint()-1)
             {
             #ifdef DEBUG
                          Serial.println(F("heater Relais ein"));
             #endif
             startHeater( true );


                          
                          if(getTemperatureOven() < getTemperatureOvenSetPoint())
                          {

                          #ifdef DEBUG
                          Serial.println(F("LED BLink heat up"));
                          #endif
                          
                          digitalWrite(GREEN_LED_PIN, HIGH);

                          delay (1000);
                          digitalWrite(GREEN_LED_PIN, LOW);
   
                          delay (1000);
                          }
                          else
                          {
                          digitalWrite(GREEN_LED_PIN, HIGH);
                          digitalWrite(RED_LED_PIN, LOW);
                         }   

                         
             digitalWrite(RED_LED_PIN, LOW);
             digitalWrite(YELLOW_LED_PIN, LOW);     
             }
                else 
               {
                startHeater( false );
                
                digitalWrite(RED_LED_PIN, HIGH);
                digitalWrite(YELLOW_LED_PIN, HIGH);
                digitalWrite(GREEN_LED_PIN, LOW);
               }
          
//if ( runMeasurement == 1) // Interrupt generated by timer
//{
//
//        digitalWrite(GREEN_LED_PIN, !digitalRead(GREEN_LED_PIN)); // Toggelt die grüne LED 
//   runMeasurement = 0;
//}

*/
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%  PWM Anne %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


LowerLevelHeater = getTemperatureOvenSetPoint()-3;
HigherLevelHeater = getTemperatureOvenSetPoint()+3;


if (runSmoker == 0) {
	
	startSmoker( false );
  
   // OFF: SetPoint + 5 or meat set point reached
	if ((getTemperatureOven() >= (getTemperatureOvenSetPoint()+5)) || (getTemperatureMeat() >= getTemperatureMeatSetPoint()))
	{
		startHeater( false );
		displayCommandSmall1("Heater Stop");
	    digitalWrite(RED_LED_PIN, HIGH);
	    digitalWrite(GREEN_LED_PIN, LOW);
	    digitalWrite(YELLOW_LED_PIN, LOW);
                          
		ovenState = false;
    }
	// ON
	else if(LowerLevelHeater <= getTemperatureOven())     
         {             

                          // #ifdef DEBUG
                          // Serial.println(F("Pulsed signal"));
                         // #endif
                         
                           displayCommandSmall1("Heater On");
                           digitalWrite(GREEN_LED_PIN, HIGH);
                           digitalWrite(RED_LED_PIN, LOW);
                           digitalWrite(YELLOW_LED_PIN, LOW);

                currentMillisHeater = millis();

                if ((currentMillisHeater - previousMillisHeater) >= intervalHighHeater) {     
                   if (ovenState) {
                        //  #ifdef DEBUG
                        //  Serial.println(F("Heater Stop"));
                        //  #endif
                          
                         startHeater( false );
                         displayCommandSmall1("Heater Stop");
                       
                         previousMillisHeater = currentMillisHeater;
                         ovenState = false;
                  } 
                  else if ((currentMillisHeater - previousMillisHeater) >= intervalLowHeater) {

                         // #ifdef DEBUG
                         // Serial.println(F("Heater On"));
                         // #endif
                          
                          startHeater( true );
                          displayCommandSmall1("Heater On");
                          
                          previousMillisHeater = currentMillisHeater;
                          ovenState = true; 
                  }
              } 

            
           
        } 
          else if((getTemperatureOven() <= getTemperatureOvenSetPoint()-4))
            { 
                          startHeater( true );
                          ovenState = true;
                           
                         // #ifdef DEBUG
                         // Serial.println(F("LED BLink heat up"));
                         // #endif
                          displayCommandSmall1("Heating Up");
                          digitalWrite(YELLOW_LED_PIN, HIGH);
                          digitalWrite(GREEN_LED_PIN, LOW);
                          digitalWrite(RED_LED_PIN, LOW);
            } 


}



if (runSmoker == 1)
{
 //displayCommandSmall2("Smoker aktive");
 currentMillisSmoker= millis();
  
        if (getTemperatureOven() >= getTemperatureOvenSetPoint()-2) {
            if (runSmoker == 0) {
              displayCommandSmall2("Smoker On");
              previousMillisSmoker = currentMillisSmoker;
              smokerCounter = 0;
              smokerState = true;
              
              startHeater( false );
              startSmoker( true );
              
              
              startHeater( false );
              startSmoker( true );
              ovenState = false;
            }
            runSmoker = 1;

        }
        if((runSmoker == 1) && (smokerCounter < 7)) {
                if (((currentMillisSmoker - previousMillisSmoker) >= intervalHighSmoker)) {     
                         if(smokerState == true) {
                          
                         // #ifdef DEBUG
                         // Serial.println(F("Smoker Stop"));
                         // #endif
                         
                          digitalWrite(YELLOW_LED_PIN, LOW);
                          digitalWrite(GREEN_LED_PIN, HIGH); 
                          digitalWrite(RED_LED_PIN, LOW); 
                          
                         startSmoker( false );
                         displayCommandSmall2("Smoker Stop");
                         
                         previousMillisSmoker = currentMillisSmoker;
                         smokerState = false;
                         smokerCounter = smokerCounter + 1;
                         }
                       if (((currentMillisSmoker - previousMillisSmoker) >= intervalLowSmoker)) {
                          if (smokerState == false) {
                          
                            //#ifdef DEBUG
                            //Serial.println(F("Smoker On"));
                            //#endif

                            startHeater( false );
                            startSmoker( true );
                            ovenState = false;

                            digitalWrite(YELLOW_LED_PIN, HIGH);
                            digitalWrite(GREEN_LED_PIN, HIGH); 
                            digitalWrite(RED_LED_PIN, LOW);
                            
                            displayCommandSmall2("Somker On");
                            
                            previousMillisSmoker = currentMillisSmoker;
                            smokerState = true; 
                          }
                  }
                }
                  if (smokerState == FALSE) {
                        if ((getTemperatureOven() >= getTemperatureOvenSetPoint()+5) || (getTemperatureMeat() >= getTemperatureMeatSetPoint()))
                        {
     
                          startHeater( false );
                             
                          digitalWrite(RED_LED_PIN, HIGH);
                          digitalWrite(GREEN_LED_PIN, LOW);
                          digitalWrite(YELLOW_LED_PIN, LOW);

                          displayCommandSmall1("Heater Stop");
                          
                          ovenState = false;
                         }
                     
      else if(LowerLevelHeater <= getTemperatureOven())     
         {              


                      displayCommandSmall1("Heater On");
                      digitalWrite(GREEN_LED_PIN, HIGH);
                      digitalWrite(RED_LED_PIN, LOW);
                      digitalWrite(YELLOW_LED_PIN, LOW);


                currentMillisHeater = millis();

                if ((currentMillisHeater - previousMillisHeater) >= intervalHighHeater) {     
                   if (ovenState) {
    
                         startHeater( false );
                         displayCommandSmall1("Heater Stop - Pulse");
                        
                         previousMillisHeater = currentMillisHeater;
                         ovenState = false;
                  } 
                  else if ((currentMillisHeater - previousMillisHeater) >= intervalLowHeater) {


                          
                          startHeater( true );
                          displayCommandSmall1("Heater On - Pulse");
                        
                          previousMillisHeater = currentMillisHeater;
                          ovenState = true; 
                  }
              } 

        }  
        
   else if((getTemperatureOven() <= getTemperatureOvenSetPoint()-4))
   { 
     startHeater( true );
      ovenState = true;

                      displayCommandSmall1("Heating Up");
                      digitalWrite(YELLOW_LED_PIN, HIGH);
                      digitalWrite(GREEN_LED_PIN, LOW);
                      digitalWrite(RED_LED_PIN, LOW);                  
                          
                          
  }
                 
              } 
        }
        if (smokerCounter == 7) {
			startSmoker( false );
			displayCommandSmall2("Smoker Stop");
        }
}












             

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%  PWM Anne %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

//%%%%%%%%%%%%%%%%%%%%% ANNE %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

          
         if(getButtonStateUp())
         {
			stm_newState = STM_STATE_SETTINGS;
            stm_exitFlag = TRUE;        
         }

         if(getButtonStateEnter())
         {
            
			stm_newState = STM_STATE_FINISHED;
            stm_exitFlag = TRUE;
         }

         if(getButtonStateDown())
         {
            stm_newState = STM_STATE_SET_TIMER;
            stm_exitFlag = TRUE;        
         }
		 
		 if ( getButtonStateSmoker() )
		{
			runSmoker = 1;
			smokerCounter = 0;
			Serial.println(F("ButtonStateSmoker"));
		}

		 

         // Exit
         if (stm_exitFlag == TRUE)
         {
            // do exit action
            clearButtonAllFlags();
            startHeater( false ); // Switch off Heater
            startSmoker( false ); // Switch off Smoker
			// All LEDs off
			digitalWrite(YELLOW_LED_PIN, LOW);
            digitalWrite(GREEN_LED_PIN, LOW);
            digitalWrite(RED_LED_PIN, LOW);  
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



   // Safety system
   if ( getTemperatureOven() > MAX_TEMPERATURE_OVEN) sysOkay = false;
   if ( getTemperatureMeat() > MAX_TEMPERATURE_MEAT) sysOkay = false;
      
      

   // Safety: If system is not okay -> Disable heaters
   if ( !sysOkay )
   {
      emergencyOff();
   }

   
 
   displayTime(); // TBD: Not every cycle
   displayTimer(); 
}
