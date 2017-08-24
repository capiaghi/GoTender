

/// ****************************************************************************
/// \file      GoTender.cpp
///
/// \brief     main code
///
/// \details   GoTender
///
/// \author    Christoph Capiaghi, Anne Liebold
///
/// \version   0.6
///
/// \date      20170817
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
const char       SOFTWARE_VERSION[10]  = "V0.6";
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


double intervalHighHeater = 20000; // modify so it is 5 second intervals - probably needs to be in ms -- 10sec
double intervalLowHeater = 120000; // modify so it is 20 seconds (2min) intervals$ ---1 min

double intervalHighSmoker = 20000; // modify so it is 5 second intervals - probably needs to be in ms --30sec
double intervalLowSmoker = 300000; // modify so it is 20 seconds (2min) intervals$ --- 5min

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


// PWM  *****************************************************************

// initialize the timer1 interrupt for the frequency variable output
// Timer3.initialize(33333);                   // initialize timer1, and set a 33.333ms period -> 30Hz
 
//   Timer3.pwm(RELAIS_HEATER, pwmDuty);     // setup pwm on pin 9, 50% duty cycle

// initialize the timer2 interrupt for the duty cycle variable output

//   FlexiTimer2::set(1, generatPWM);           // first interrupt after 1ms -> then reconfigured
//   FlexiTimer2::start();

 
   
}

/* CAPC
// ----------------------------------------------------------------------------------------------

// PWM COMMAND *****************************************************************
// Interrupt service routine for timer2 overflow interrupt

              void generatPWM(){

                static char state;

                 newPwmDuty = (float)analogRead(RELAIS_HEATER)/1023;    
                
                if(state){
                  if(newPwmDuty > 0){
                    startHeater( true ); 
                    //digitalWrite(RED_LED_PIN, HIGH);
                  }

                  FlexiTimer2::stop();
                  FlexiTimer2::set(pwmPeriod*newPwmDuty, generatPWM);
                  FlexiTimer2::start();

                  state= 0;
                }
                else{
                 if(newPwmDuty <1){
                   startHeater( false );
                   //digitalWrite(RED_LED_PIN, LOW);
                 }
                 
                  FlexiTimer2::stop();
                  FlexiTimer2::set(pwmPeriod - pwmPeriod*newPwmDuty, generatPWM);
                  FlexiTimer2::start();

                  state= 1;
                 
                } 
              }

// ----------------------------------------------------------------------------------------------             

*/
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
                  stm_SubState = STM_SUBSTATE_DONE;
               }
            break;

            /* TBD: Not used anymore: Set smoker through button
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
                  //setSmoker(); 
                  stm_sub_entryFlag = TRUE;

                  stm_SubState = STM_SUBSTATE_DONE;
               }
               if(getButtonStateDown())
               {
                  //resetSmoker(); 
                  stm_sub_entryFlag = TRUE;
                  stm_SubState = STM_SUBSTATE_DONE;
               }
               
            break;
            
            */
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
      /// Sets timer

         // Entry action
         if (stm_entryFlag == TRUE)
         {
            #ifdef DEBUG
               Serial.println(F("Entered STM_STATE_TIMER"));
            #endif      
            displayTitle("TIMER");
            displayCommandSmall1("Set: Press enter");
            displayCommandSmall2("NO: Press down");   
            stm_time_SubState = STM_SUBSTATE_SET_HOUR_TIMER;
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

         if(getButtonStateUp())
         {
            stm_newState = STM_STATE_SETTINGS;
            stm_exitFlag = TRUE;        
         }

         if(getButtonStateEnter())
         {
              stm_newState = STM_STATE_TIMER;
              stm_SubState = STM_SUBSTATE_SET_TIMER;
              timerSet     = true; // Yes, timer is set
              stm_sub_entryFlag = TRUE;
           
         }

         if(getButtonStateDown())
         {
            timerSet     = false;   // No, timer is disabled. Do not use timer
            stm_newState = STM_STATE_SUMMARY;   
            stm_exitFlag = TRUE;
                
         }



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
            
            if (timerSet)
            {
               startTimer(); // Reads actual time and calculates end time
            }
            
            stm_entryFlag = FALSE;
         }
         
         
         // Timer expired?
         if (timerSet)
         {
            if ( timerExpired() )
            {
               stm_exitFlag = TRUE;
               // Next state correct? TBD Anne ;-)
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
  
   if ((getTemperatureOven() >= (getTemperatureOvenSetPoint()+5)) || (getTemperatureMeat() >= getTemperatureMeatSetPoint()))
          {

                          #ifdef DEBUG
                          Serial.println(F("Pulsed signal and heater stop"));
                          #endif
  
           // Timer3.disablePwm(RELAIS_HEATER);
            startHeater( false );
            displayCommandSmall1("Heater off");
            
                          digitalWrite(RED_LED_PIN, HIGH);
                          delay (1000);
                          
                          digitalWrite(RED_LED_PIN, LOW);
                          delay (1000);
                          digitalWrite(GREEN_LED_PIN, LOW);
            ovenState = false;
          }
      else if(LowerLevelHeater <= getTemperatureOven())     
         {             

                          #ifdef DEBUG
                          Serial.println(F("Pulsed signal"));
                          #endif
                      digitalWrite(GREEN_LED_PIN, HIGH);

                currentMillisHeater = millis();

                if ((currentMillisHeater - previousMillisHeater) >= intervalHighHeater) {     
                   if (ovenState) {
                          #ifdef DEBUG
                          Serial.println(F("heater stop"));
                          #endif
                          
                         startHeater( false );
                         displayCommandSmall1("Heater Low");
                       
                         previousMillisHeater = currentMillisHeater;
                         ovenState = false;
                  } 
                  else if ((currentMillisHeater - previousMillisHeater) >= intervalLowHeater) {

                          #ifdef DEBUG
                          Serial.println(F("heater on"));
                          #endif
                          
                          startHeater( true );
                          displayCommandSmall1("Heater High");
                          
                          previousMillisHeater = currentMillisHeater;
                          ovenState = true; 
                  }
              } 

            
           
        } 
          else if((getTemperatureOven() <= getTemperatureOvenSetPoint()-4))
            { 
                          startHeater( true );
                           ovenState = true;
                           
                          #ifdef DEBUG
                          Serial.println(F("LED BLink heat up"));
                          #endif
                          displayCommandSmall1("Heating up");
                          
                          digitalWrite(GREEN_LED_PIN, HIGH);
                          delay (1000);
                          
                          digitalWrite(GREEN_LED_PIN, LOW);
                          delay (1000);  
            } 


}



if (getSmokerState() == TRUE)
{
 //displayCommandSmall2("Smoker aktive");
 currentMillisSmoker= millis();
  
        if (getTemperatureOven() >= getTemperatureOvenSetPoint()-2) {
            if (runSmoker == 0) {
              displayCommandSmall2("Smoker run");
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
        if((runSmoker == 1) && (smokerCounter < 3)) {
                if (((currentMillisSmoker - previousMillisSmoker) >= intervalHighSmoker)) {     
                         if(smokerState == true) {
                          #ifdef DEBUG
                          Serial.println(F("Smoker stop"));
                          #endif
                          digitalWrite(RED_LED_PIN, HIGH);
                          
                         startSmoker( false );
                         displayCommandSmall2("Smoker Low");
                         
                         previousMillisSmoker = currentMillisSmoker;
                         smokerState = false;
                         smokerCounter = smokerCounter + 1;
                         }
                       if (((currentMillisSmoker - previousMillisSmoker) >= intervalLowSmoker)) {
                          if (smokerState == false) {
                            #ifdef DEBUG
                            Serial.println(F("Smoker on"));
                            #endif

                            startHeater( false );
                            startSmoker( true );
                            ovenState = false;
                            displayCommandSmall2("Somker High");
                            
                            previousMillisSmoker = currentMillisSmoker;
                            smokerState = true; 
                          }
                  }
                }
                  if (smokerState == FALSE) {
                        if ((getTemperatureOven() >= getTemperatureOvenSetPoint()+5) || (getTemperatureMeat() >= getTemperatureMeatSetPoint()))
                        {

                          #ifdef DEBUG
                          Serial.println(F("Pulsed signal and heater stop"));
                          #endif
  
     
                          startHeater( false );
                             
                          digitalWrite(RED_LED_PIN, LOW);
      
                          digitalWrite(GREEN_LED_PIN, LOW);
                        
                          ovenState = false;
                         }
                     
      else if(LowerLevelHeater <= getTemperatureOven())     
         {              
                          #ifdef DEBUG
                          Serial.println(F("Pulsed signal"));
                          #endif

                           digitalWrite(RED_LED_PIN, HIGH);


                currentMillisHeater = millis();

                if ((currentMillisHeater - previousMillisHeater) >= intervalHighHeater) {     
                   if (ovenState) {
                          #ifdef DEBUG
                          Serial.println(F("heater stop"));
                          #endif
                          
                         startHeater( false );
                         displayCommandSmall1("Heater Low 1min");
                        
                         previousMillisHeater = currentMillisHeater;
                         ovenState = false;
                  } 
                  else if ((currentMillisHeater - previousMillisHeater) >= intervalLowHeater) {

                          #ifdef DEBUG
                          Serial.println(F("heater on"));
                          #endif
                          
                          startHeater( true );
                          displayCommandSmall1("Heater High 30s");
                        
                          previousMillisHeater = currentMillisHeater;
                          ovenState = true; 
                  }
              } 

            

           
        }  
   else if((getTemperatureOven() <= getTemperatureOvenSetPoint()-4))
   { 
     startHeater( true );
      ovenState = true;
                          #ifdef DEBUG
                          Serial.println(F(" heating up"));
                          #endif
                          displayCommandSmall1("Heating up");
           
                          
                          digitalWrite(GREEN_LED_PIN, HIGH);
                          delay (1000);
                          
                          digitalWrite(GREEN_LED_PIN, LOW);
                          delay (1000);  
  }
                  

              } 
        }
        if (smokerCounter == 3) {
          runSmoker = 0;
          //resetSmoker(); TBD: Was ist das?
          displayCommandSmall2("Smoker stop");
        }
}






             

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%  PWM Anne %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

//%%%%%%%%%%%%%%%%%%%%% ANNE %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

          
         if(getButtonStateUp())
         {
            stm_newState = STM_STATE_TIMER;
            stm_exitFlag = TRUE;        
         }

         if(getButtonStateEnter())
         {
            stm_newState = STM_STATE_SETTINGS;
            stm_exitFlag = TRUE;
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

}
