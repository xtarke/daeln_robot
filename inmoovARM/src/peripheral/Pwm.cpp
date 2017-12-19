/*
 * Pwm.cpp
 *
 *  Created on: Jun 6, 2017
 *      Author: xtarke
 */

#include "Pwm.h"
#include "stm32f10x_tim.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include <string.h>

Pwm::Pwm() {

	/* Compute the value to be set in ARR register to generate signal frequency at 50 Hz */
	TimerPeriod = 20000 - 1;

	/* Compute CCR1 value to generate a duty cycle at 50% for channels and 1N */
	ChannelPulses[0] = (uint16_t) (((uint32_t) 5 * (TimerPeriod - 1)) / 10);
	memset(&ChannelPulses[1],  ChannelPulses[0], 6 * sizeof(uint16_t));

	/* Configure Ports */
	/* TIM1, GPIOA, GPIOB, GPIOE and AFIO clocks enable */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1 | RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB, ENABLE);
	/* TIM4 clock enable */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);

	GPIO_InitTypeDef GPIO_InitStructure;

	/* GPIOA Configuration: Channel 1, 2 and 3 as alternate function push-pull */
	/* GPIO_Pin_11 is USB D- */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;		// Alternate function Push Pull
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* GPIOB Configuration: Channel 1, 2, 3 and 4 as alternate function push-pull */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	/* Time Base configuration */
	TIM_TimeBaseStructure.TIM_Prescaler = 72;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_Period = TimerPeriod;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;

	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);

	/* Channel 1, 2,3 and 4 Configuration in PWM mode */
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;
	TIM_OCInitStructure.TIM_Pulse = ChannelPulses[0];
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
	TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;
	TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
	TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCIdleState_Reset;

	TIM_OC1Init(TIM1, &TIM_OCInitStructure);
	TIM_OC1Init(TIM4, &TIM_OCInitStructure);

	TIM_OCInitStructure.TIM_Pulse = ChannelPulses[0];
	TIM_OC2Init(TIM1, &TIM_OCInitStructure);
	TIM_OC3Init(TIM1, &TIM_OCInitStructure);

	TIM_OC2Init(TIM4, &TIM_OCInitStructure);
	TIM_OC3Init(TIM4, &TIM_OCInitStructure);
	TIM_OC3Init(TIM4, &TIM_OCInitStructure);
	TIM_OC4Init(TIM4, &TIM_OCInitStructure);

}

Pwm::~Pwm() {

}

void Pwm::enable(){
	/* TIM1 counter enable */
	TIM_Cmd(TIM1, ENABLE);
	TIM_Cmd(TIM4, ENABLE);

	/* TIM1 Main Output Enable */
	TIM_CtrlPWMOutputs(TIM1, ENABLE);
	// TIM_CtrlPWMOutputs(TIM4, ENABLE);
}


void Pwm::set_pulse(uint8_t id, uint16_t pulse){
	if (id < 7 && pulse < TimerPeriod) {
		ChannelPulses[id] = pulse;

		switch (id){
		case 0:
			TIM1->CCR1 = pulse;
			break;
		case 1:
			TIM1->CCR2 = pulse;
			break;
		case 2:
			TIM1->CCR3 = pulse;
			break;
		case 3:
			TIM4->CCR1 = pulse;
			break;
		case 4:
			TIM4->CCR2 = pulse;
			break;
		case 5:
			TIM4->CCR3 = pulse;
			break;
		case 6:
			TIM4->CCR4 = pulse;
			break;
		}
	}
}
