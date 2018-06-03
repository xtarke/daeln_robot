/*
 * Adc.cpp
 *
 *  Created on: Jun 28, 2017
 *      Author: xtarke
 */

#include "Adc.h"

#include <string.h>
#include "stm32f10x_adc.h"
#include "stm32f10x_dma.h"

volatile uint16_t ADC1ConvertedValues[TEST_BUFFER_SIZE];
volatile uint16_t samples = 0;
volatile uint8_t sampled = 0;

void ADC1_2_IRQHandler(void)
{
	/* ADC value */
	if (samples < TEST_BUFFER_SIZE)
		ADC1ConvertedValues[samples] = ADC_GetConversionValue(ADC1);
	else{
		ADC_GetConversionValue(ADC1);
		sampled = 1;
	}

	samples++;
}


void adc_init() {

	/* Reset data */
	samples = 0;
	sampled = 0;
	memset((void *)ADC1ConvertedValues,0,sizeof(ADC1ConvertedValues));

	/* Clocks */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	/* Enable ADC1 and GPIOC clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_GPIOA, ENABLE);

	NVIC_InitTypeDef NVIC_InitStructure;
	/* Configure and enable ADC interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = ADC1_2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	/* Analog INPUT */
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	ADC_InitTypeDef ADC_InitStructure;
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NbrOfChannel = 1;
	ADC_Init(ADC1, &ADC_InitStructure);
	ADC_ITConfig(ADC1, ADC_IT_EOC, ENABLE);

	/* ADC1 regular channels configuration */
	/* PA_0 is channel 0 */
	ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_28Cycles5);

	/* Enable ADC1 */
	ADC_Cmd(ADC1, ENABLE);

	/* Enable ADC1 reset calibration register */
	ADC_ResetCalibration(ADC1);
	/* Check the end of ADC1 reset calibration register */
	while(ADC_GetResetCalibrationStatus(ADC1));

	/* Start ADC1 Software Conversion */
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}


uint8_t adc_test_complete(){
	return sampled;
}

void adc_test_read(){
	sampled = 0;
}

uint16_t adc_get_data(uint16_t sample){

	if (sample < TEST_BUFFER_SIZE)
		return ADC1ConvertedValues[sample];

	return 0;
}

void Adc_get_alldata(uint16_t *data){
	memcpy(data, (void *)ADC1ConvertedValues, sizeof(ADC1ConvertedValues));
}

