/*
 * ServoMotor.cpp
 *
 *  Created on: Jun 7, 2017
 *      Author: xtarke
 */

#include "ServoMotor.h"
#include <string.h>
#include <stdint.h>

#include "diag/Trace.h"

ServoMotor::ServoMotor(uint16_t _overloadMaxCurrent, uint16_t _overshootMaxCurrent) {
	//Initialize motor at 50% position
	memset(position, 50, sizeof(position));

	for (int i = 0; i < Pwm::N_PWMS; i++){
		/* Servo pulse is 700u to 2500u       *
		 * channel pulse 20000 is 20ms (50Hz) *
		 * f(x) = x.18 + 700                  *
		 * where x is servo position in %     */
		pwm.set_pulse(i, 2500);		//50*18 + 700;
	}

	motorProtection.setProtection(_overloadMaxCurrent, _overshootMaxCurrent);

	pwm.enable();
}

void ServoMotor::set_position(uint8_t id, uint8_t pos)
{
	if (id < Pwm::N_PWMS){
		uint16_t pos_ = (pos > 100) ? 100 : pos;

		position[id] = pos_;
		pwm.set_pulse(id, pos_ * 18 + 700);
	}
}

ServoMotor::~ServoMotor() {
	// TODO Auto-generated destructor stub
}

