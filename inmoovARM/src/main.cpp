//
// This file is part of the GNU ARM Eclipse distribution.
// Copyright (c) 2014 Liviu Ionescu.
//

// ----------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "diag/Trace.h"

#include "peripheral/Timer.h"
#include "peripheral/BlinkLed.h"

#include "comm/Serial.h"
#include "robot/ServoMotor.h"

#include "peripheral/Adc.h"

#include "robot/CurrentProtection.h"
#include "./peripheral/Usart.h"


extern "C" {
	#include "./usb/hw_config.h"
	#include "usb_desc.h"
	#include "usb_pwr.h"
	#include "usb_init.h"
}


// Definitions visible only within this translation unit.
namespace
{
  // ----- Timing definitions -------------------------------------------------

  // Keep the LED on for 2/3 of a second.
  constexpr Timer::ticks_t BLINK_ON_TICKS = Timer::FREQUENCY_HZ * 3 / 4;
  constexpr Timer::ticks_t BLINK_OFF_TICKS = Timer::FREQUENCY_HZ - BLINK_ON_TICKS;
}

// ----- main() ---------------------------------------------------------------

// Sample pragmas to cope with warnings. Please note the related line at
// the end of this function, used to pop the compiler diagnostics status.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wmissing-declarations"
#pragma GCC diagnostic ignored "-Wreturn-type"


int main(int argc, char* argv[])
{
	uint32_t seconds = 0;

	// Send a greeting to the trace device (skipped on Release).
	// trace_puts("Hello ARM World!");

	// At this stage the system clock should have already been configured
	// at high speed.
	// trace_printf("System clock: %u Hz\n", SystemCoreClock);

	Timer timer;
	timer.start();

	BlinkLed blinkLed;
	Serial serialComm;

	// Perform all necessary initializations for the LED.
	blinkLed.powerUp();

	Set_USBClock();
	USB_Interrupts_Config();
	USB_Init();

	ServoMotor motors(2000, 5000);

	//-----------------------------------------------------
	// Infinite loop
	while (1)
	{

		serialComm.Talk(motors);


		// Count seconds on the trace device.
		//trace_printf("Second %u\n", seconds);

		//timer.sleep(1000);




		//for (int i=0; i < Adc::N_CHANNELS; i++) {

//		trace_printf("ADC: %u %u %u %u %u %u %u \n",  motors.getCurrent(0),
//				motors.getCurrent(0),
//				motors.getCurrent(2),
//				motors.getCurrent(3),
//				motors.getCurrent(4),
//				motors.getCurrent(5),
//				motors.getCurrent(6));

//		timer.sleep(1000);
		//}


	}



	// Infinite loop, never return.
}

#pragma GCC diagnostic pop

// ----------------------------------------------------------------------------
