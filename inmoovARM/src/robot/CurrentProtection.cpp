/*
 * CurrentProtection.cpp
 *
 *  Created on: Nov 17, 2017
 *      Author: xtarke
 */

#include "CurrentProtection.h"
#include "../peripheral/Pwm.h"

#include "stm32f10x_tim.h"
#include "stm32f10x_rcc.h"
#include "cortexm/ExceptionHandlers.h"
#include <stddef.h>
#include <string.h>

extern "C" {
	CurrentProtection *currentProtectionClass = NULL;
}

/******************************************************************************/
 /*            STM32F10x TIM2 Peripheral Interrupt Handler:  exported to C    */
 /******************************************************************************/

extern "C" void
TIM2_IRQHandler(void)
{
	uint16_t ccr1 = 0;
	uint16_t capture = 0;

	if (TIM_GetITStatus(TIM2, TIM_IT_CC1) != RESET)
	{
		TIM_ClearITPendingBit(TIM2, TIM_IT_CC1);

		ccr1 = CurrentProtection::get_CCR1_Val();

		if (currentProtectionClass != NULL) {
			CurrentProtection_enqueue(currentProtectionClass);
			CurrentProtection_accValues(currentProtectionClass);

			/* Overshoot protection */
			CurrentProtection_overshootTest(currentProtectionClass);
		}

		/* Pin PC.06 toggling with frequency = 73.24 Hz */
		// GPIO_WriteBit(GPIOC, GPIO_Pin_13, (BitAction)(1 - GPIO_ReadOutputDataBit(GPIOC, GPIO_Pin_13)));

		capture = TIM_GetCapture1(TIM2);
		TIM_SetCompare1(TIM2, capture + ccr1);
	}
//	/* else if (TIM_GetITStatus(TIM2, TIM_IT_CC2) != RESET)
//	{
//		 TIM_ClearITPendingBit(TIM2, TIM_IT_CC2);
//
//		 /* Pin PC.07 toggling with frequency = 109.8 Hz */
//		 GPIO_WriteBit(GPIOC, GPIO_Pin_7, (BitAction)(1 - GPIO_ReadOutputDataBit(GPIOC, GPIO_Pin_7)));
//		 capture = TIM_GetCapture2(TIM2);
//		 TIM_SetCompare2(TIM2, capture + CCR2_Val);
//	}
//	else if (TIM_GetITStatus(TIM2, TIM_IT_CC3) != RESET)
//	{
//		TIM_ClearITPendingBit(TIM2, TIM_IT_CC3);
//
//		/* Pin PC.08 toggling with frequency = 219.7 Hz */
//		GPIO_WriteBit(GPIOC, GPIO_Pin_8, (BitAction)(1 - GPIO_ReadOutputDataBit(GPIOC, GPIO_Pin_8)));
//		capture = TIM_GetCapture3(TIM2);
//		TIM_SetCompare3(TIM2, capture + CCR3_Val);
//	}
//	else
//	{
//		TIM_ClearITPendingBit(TIM2, TIM_IT_CC4);
//
//		/* Pin PC.09 toggling with frequency = 439.4 Hz */
//		GPIO_WriteBit(GPIOC, GPIO_Pin_9, (BitAction)(1 - GPIO_ReadOutputDataBit(GPIOC, GPIO_Pin_9)));
//		capture = TIM_GetCapture4(TIM2);
//		TIM_SetCompare4(TIM2, capture + CCR4_Val);
//	}
}

CurrentProtection::CurrentProtection( ){

	uint16_t PrescalerValue = 0;

	/* Export my address to ISR C pointer */
	currentProtectionClass = this;

	/* Reset queue and accumulators */
	memset((void *)circularQueue,0,sizeof(circularQueue));
	memset((void *)accumulatedCurrent,0,sizeof(accumulatedCurrent));
	memset((void *)averageCurrent,0,sizeof(averageCurrent));


	/* TIM base config structure */
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;

	/* Configure TIM2 */
	/* ---------------------------------------------------------------
	TIM2 Configuration: Output Compare Timing Mode:
 	TIM2 counter clock at 6 MHz
 	CC1 update rate = TIM2 counter clock / CCR1_Val = 1kHz Hz

	CC2 update rate = TIM2 counter clock / CCR2_Val = 219.7 Hz <-- commented
	CC3 update rate = TIM2 counter clock / CCR3_Val = 439.4 Hz <-- commented
	CC4 update rate = TIM2 counter clock / CCR4_Val = 878.9 Hz <-- commented
	  --------------------------------------------------------------- */

	/*  Configure the nested vectored interrupt controller. */
	NVIC_InitTypeDef NVIC_InitStructure;

	/* Enable the TIM2 gloabal Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

	NVIC_Init(&NVIC_InitStructure);

	/* Enable system clock for TIM2 */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

	/* Compute the prescaler value */
	PrescalerValue = (uint16_t) (SystemCoreClock / 12000000) - 1;

	/* Time base configuration */
	TIM_TimeBaseStructure.TIM_Period = 65535;
	TIM_TimeBaseStructure.TIM_Prescaler = 0;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

	/* Prescaler configuration */
	TIM_PrescalerConfig(TIM2, PrescalerValue, TIM_PSCReloadMode_Immediate);

	/* Output Compare Timing Mode configuration: Channel1 */
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_Timing;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = CCR1_Val;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;

	TIM_OC1Init(TIM2, &TIM_OCInitStructure);

	TIM_OC1PreloadConfig(TIM2, TIM_OCPreload_Disable);

	/* Output Compare Timing Mode configuration: Channel2 */
	//TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	//TIM_OCInitStructure.TIM_Pulse = CCR2_Val;

	//TIM_OC2Init(TIM2, &TIM_OCInitStructure);

	//TIM_OC2PreloadConfig(TIM2, TIM_OCPreload_Disable);

	/* Output Compare Timing Mode configuration: Channel3 */
	//TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	//TIM_OCInitStructure.TIM_Pulse = CCR3_Val;

	//TIM_OC3Init(TIM2, &TIM_OCInitStructure);

	//TIM_OC3PreloadConfig(TIM2, TIM_OCPreload_Disable);

	/* Output Compare Timing Mode configuration: Channel4 */
	//TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	//TIM_OCInitStructure.TIM_Pulse = CCR4_Val;

	//TIM_OC4Init(TIM2, &TIM_OCInitStructure);

	//TIM_OC4PreloadConfig(TIM2, TIM_OCPreload_Disable);

	/* TIM IT enable */
	//TIM_ITConfig(TIM2, TIM_IT_CC1 | TIM_IT_CC2 | TIM_IT_CC3 | TIM_IT_CC4, ENABLE);
	TIM_ITConfig(TIM2, TIM_IT_CC1, ENABLE);

	/* TIM2 enable counter */
	TIM_Cmd(TIM2, ENABLE);
}

CurrentProtection::~CurrentProtection() {
	// TODO Auto-generated destructor stub
}


void CurrentProtection::enqueue(){
	adcs.get_alldata((uint16_t *)circularQueue[queueIndex++]);
	/* Limit for 16 samples */
	queueIndex &= 0xf;
}

void CurrentProtection::accValues(){

	uint8_t index = (queueIndex - 1) & 0xf;

	/* Accumulate all samples ignoring the next one */
	for (int i=0; i < Adc::N_CHANNELS; i++)
		accumulatedCurrent[i] += circularQueue[index][i] - circularQueue[queueIndex][i];

	/* Circuit gain is 12 *
	 * Shut resistor is 0.1 Ohm    *
	 *
	 * ADC = Vin * 2^12 / 3.3
	 *
	 * I = Vshunt / 0.1 Ohm = Vshunt * 10
	 *
	 * ADC = I * 2^12 * gain (12) / (10 * 3.3)
	 *
	 * I = ADC * 10 * 3.3 / (2^12 * 12)
	 *
	 * I(mA) = (ADC * 2750) >> 12
	 *
	 * ----------------------------------- */

	for (int i=0; i < Adc::N_CHANNELS; i++)
		averageCurrent[i] =  ((accumulatedCurrent[i] >> 4) * 2750 ) >> 12;

}

void CurrentProtection::overshootTest(){
	for (int i=0; i < Adc::N_CHANNELS; i++)
		if (averageCurrent[i] > overshootMaxCurrent)
			Pwm::emergencyDisable();
}

// C access functions for TIMER ISR
EXPORT_C void CurrentProtection_accValues(CurrentProtection* ptr){
		ptr->accValues();
}


// C access functions for TIMER ISR
EXPORT_C CurrentProtection* CurrentProtection_new()
{
    return new CurrentProtection();
}

// C access functions for TIMER ISR
EXPORT_C void CurrentProtection_delete(CurrentProtection* ptr)
{
    delete ptr;
}

// C access functions for TIMER ISR
EXPORT_C void CurrentProtection_enqueue(CurrentProtection* ptr)
{
    return ptr->enqueue();
}

EXPORT_C void CurrentProtection_overshootTest(CurrentProtection* ptr)
{
	return ptr->overshootTest();
}

