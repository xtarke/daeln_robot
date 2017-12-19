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

	uint8_t responseData[Serial::PKG_MAX_SIZE];

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
		uint8_t comInterface = serialComm.isPackgageReady();

		if (comInterface == 0){
			serialComm.check_package();
		}
		else
		{
			uint8_t pkg_type = serialComm.get_data(Serial::PKG_CMD_IDX, comInterface);
			uint8_t servo_id;
			uint8_t servo_position;
			uint16_t servo_current;

			switch (pkg_type){
				case Serial::PWM_DATA:
					/* Copy all data ignoring Checksum and header to make response pkg */
					serialComm.getPaylod(responseData + Serial::PKG_HEADER_SIZE,
										Serial::SERVO_PAYLOAD_SIZE, comInterface);

					/* Get data */
					servo_id = responseData[Serial::SERVO_ID_IDX + Serial::PKG_HEADER_SIZE];
					servo_position = responseData[Serial::SERVO_DATA_IDX + Serial::PKG_HEADER_SIZE];
					motors.set_position(servo_id, servo_position);

					/* Send response data */
					responseData[Serial::PKG_CMD_IDX + Serial::PKG_HEADER_SIZE] = Serial::SERVO_ACK_CMD;
					serialComm.makeAndSend(responseData, Serial::SERVO_PAYLOAD_SIZE, comInterface);

					break;

				case Serial::ADC_DATA:
					/* Copy all data without Checksum and header to make response pkg */
					serialComm.getPaylod(responseData + Serial::PKG_HEADER_SIZE,
										Serial::SERVO_PAYLOAD_SIZE, comInterface);
					/* Get data */
					servo_id = responseData[Serial::PKG_HEADER_SIZE + Serial::ADC_ID_IDX];
					servo_current = motors.getCurrent(servo_id);

					/* Send response data */
					responseData[Serial::PKG_HEADER_SIZE + Serial::PKG_CMD_IDX] = Serial::ADC_ACK_CMD;
					/* Send current high and low data */
					responseData[Serial::PKG_HEADER_SIZE + Serial::PKG_CMD_IDX + Serial::ADC_DATA_IDX] = (servo_current >> 8);
					responseData[Serial::PKG_HEADER_SIZE + Serial::PKG_CMD_IDX + Serial::ADC_DATA_IDX + 1] = servo_current & 0xff;

					serialComm.makeAndSend(responseData, Serial::ADC_PAYLOAD_SIZE, comInterface);
					break;

				default:
					break;

			}
			serialComm.consume(comInterface);
			seconds++;

			//uint16_t x = ADC_GetConversionValue(ADC1);

			blinkLed.turnOn();

		 }

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
