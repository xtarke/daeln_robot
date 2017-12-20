/*
 * ServoMotor.h
 *
 *  Created on: Jun 7, 2017
 *      Author: xtarke
 */

#ifndef PERIPHERAL_SERVOMOTOR_H_
#define PERIPHERAL_SERVOMOTOR_H_

#include <stdint.h>

#include "../peripheral/Pwm.h"
#include "CurrentProtection.h"

class ServoMotor {
private:
	Pwm pwm;
	uint8_t position[Pwm::N_PWMS];

	CurrentProtection motorProtection;

public:
	enum {N_MOTORS = Pwm::N_PWMS};

	/* Protection current in mA */
	ServoMotor(uint16_t _overloadMaxCurrent, uint16_t _overshootMaxCurrent);
	virtual ~ServoMotor();

	void set_position(uint8_t id, uint8_t pos);

	uint16_t getCurrent(uint8_t id)  {return motorProtection.getAverageCurrent(id);}

};

#endif /* PERIPHERAL_SERVOMOTOR_H_ */
