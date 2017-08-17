// ****************************************************************************
/// \file      Startup.cpp
///
/// \brief     Startup Sequence
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
// ****************************************************************************
// Includes *******************************************************************
#include "Startup.h"



// Private constants **********************************************************

// ----------------------------------------------------------------------------
/// \brief     Settings for GoTender
/// \detail    Set time and temperature
/// \warning   
/// \return    if finished: return 1
/// \todo
uint8_t startup()
{
	uint8_t stateTC 	= checkThermoCouples();
	uint8_t stateRTC 	= checkRTC();
  uint8_t stateRTCT  = checkRTCT();
	
	return (stateTC && stateRTC); // Only when both okay return true
}

