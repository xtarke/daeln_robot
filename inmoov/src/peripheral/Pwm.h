/*
 * Pwm.h
 *
 *  Created on: Jun 6, 2017
 *      Author: xtarke
 */

#ifndef PERIPHERAL_PWM_H_
#define PERIPHERAL_PWM_H_

#include <stdint.h>

#include "stm32f10x_tim.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"

class Pwm {

public:
	enum {N_PWMS = 7};

	Pwm();
	virtual ~Pwm();

	void enable();
	void set_pulse(uint8_t id, uint16_t pulse);

	uint16_t get_maxPulse(){return TimerPeriod;}

	static void emergencyDisable(){
		/* TIM1 Main Output Enable */
		TIM_CtrlPWMOutputs(TIM1, DISABLE);
		TIM_CtrlPWMOutputs(TIM4, DISABLE);

		TIM_Cmd(TIM1, DISABLE);
	}

private:
	uint16_t TimerPeriod;
	uint16_t ChannelPulses[N_PWMS];

};

#endif /* PERIPHERAL_PWM_H_ */
