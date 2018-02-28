/*
 * Usart.cpp
 *
 *  Created on: Dec 11, 2017
 *      Author: xtarke
 */

#include "Usart.h"
#include <stdint.h>
#include <string.h>

#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_dma.h"
#include "cortexm/ExceptionHandlers.h"


extern "C" {
	#include "../data/Queue.h"

	extern void *UARTQueue_ptr;
}

extern "C"
void USART2_IRQHandler(void)
{
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
	{
		/* Read one byte from the receive data register */
		// RxBuffer[RxCounter++] = USART_ReceiveData(USART2);
		// RxCounter = RxCounter & 0xf;

		uint8_t RxBuffer = USART_ReceiveData(USART2);

		if (UARTQueue_ptr != NULL)
			Queue_enqueue((Queue *)UARTQueue_ptr, RxBuffer);
	}
}

extern "C"
void USART3_IRQHandler(void)
{

	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)
	{
		/* Read one byte from the receive data register */
		// RxBuffer[RxCounter++] = USART_ReceiveData(USART2);
		// RxCounter = RxCounter & 0xf;

		uint8_t RxBuffer = USART_ReceiveData(USART3);

		if (UARTQueue_ptr != NULL)
			Queue_enqueue((Queue *)UARTQueue_ptr, RxBuffer);
	}
}


extern "C"
void DMA1_Channel7_IRQHandler(void){

	if (DMA_GetITStatus(DMA1_IT_TC7))
	{
	   /* Clear DMA Stream Transfer Complete interrupt pending bit */
	   DMA_ClearITPendingBit(DMA1_IT_GL7 | DMA1_IT_TC7);
	}
   DMA_Cmd(DMA1_Channel7, DISABLE);
}

extern "C"
void DMA1_Channel2_IRQHandler(void){

	if (DMA_GetITStatus(DMA1_IT_TC2))
	{
	   /* Clear DMA Stream Transfer Complete interrupt pending bit */
	   DMA_ClearITPendingBit(DMA1_IT_GL2 | DMA1_IT_TC2);
	}
   DMA_Cmd(DMA1_Channel2, DISABLE);
}


uint8_t Usart::putChar (uint8_t c)
{

	while (!(USART3->SR & USART_FLAG_TXE));

	USART3->DR = (c & 0xFF);

	return (c);
}

Usart::Usart() {

	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	DMA_InitTypeDef DMA_InitStructure;

	/* Enable peripheral clock    *
	 * USART 3 is in PORTB */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

	/* Enable tx IRQ */
	NVIC_InitTypeDef NVIC_InitStructure;

	/* Enable the USARTz Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 6;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	/* Configure USART3 Rx as input floating */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	/* Configure USARTy Tx as alternate function push-pull */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	/* DMA TX Buffer */
	/* Each DMA channel is hardwire for specific peripheral *
	 * Channel 1: ADC, TIM2 e TIM4
	 * Channel 2: SPI, USART3tx
	 * (...)                  	 */

	DMA_DeInit(DMA1_Channel2);	/* Channel 1 is used by ADC */
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&USART3->DR;
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)TxBuffer;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
	DMA_InitStructure.DMA_BufferSize = 1;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	DMA_Init(DMA1_Channel2, &DMA_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

	/* Configure USART2 */
	/* PB.11 -> RX
	 * PA.10 -> TX       */
	USART_Init(USART3, &USART_InitStructure);

	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);

	DMA_ITConfig(DMA1_Channel2, DMA_IT_TC, ENABLE);

	/* Enable USARTy DMA TX request */
	// USART_DMACmd(USART2, USART_DMAReq_Tx, ENABLE);

	USART_DMACmd(USART3, USART_DMAReq_Tx, ENABLE);

	/* Enable USART2 */
	USART_Cmd(USART3, ENABLE);
}

//Usart::Usart() {
//
//	GPIO_InitTypeDef GPIO_InitStructure;
//	USART_InitTypeDef USART_InitStructure;
//	DMA_InitTypeDef DMA_InitStructure;
//
//	/* Enable peripheral clock    *
//	 * USART 1 and 2 are in PORTA */
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);
//	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
//	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
//
//	/* Enable tx IRQ */
//	NVIC_InitTypeDef NVIC_InitStructure;
//
//	/* Enable the USARTz Interrupt */
//	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 6;
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//	NVIC_Init(&NVIC_InitStructure);
//
//	/* Configure USART2 Rx as input floating */
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
//	GPIO_Init(GPIOA, &GPIO_InitStructure);
//
//	/* Configure USARTy Tx as alternate function push-pull */
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
//	GPIO_Init(GPIOA, &GPIO_InitStructure);
//
//	/* DMA TX Buffer */
//	DMA_DeInit(DMA1_Channel7);	/* Channel 1 is used by ADC */
//	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&USART2->DR;
//	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)TxBuffer;
//	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
//	DMA_InitStructure.DMA_BufferSize = 1;
//	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
//	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
//	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
//	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
//	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
//	DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
//	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
//	DMA_Init(DMA1_Channel7, &DMA_InitStructure);
//
//	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel7_IRQn;
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//	NVIC_Init(&NVIC_InitStructure);
//
//	USART_InitStructure.USART_BaudRate = 115200;
//	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
//	USART_InitStructure.USART_StopBits = USART_StopBits_1;
//	USART_InitStructure.USART_Parity = USART_Parity_No;
//	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
//	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
//
//	/* Configure USART2 */
//	/* PA.3 -> RX
//	 * PA.2 -> TX       */
//	USART_Init(USART2, &USART_InitStructure);
//
//	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
//
//	DMA_ITConfig(DMA1_Channel7, DMA_IT_TC, ENABLE);
//
//	/* Enable USARTy DMA TX request */
//	// USART_DMACmd(USART2, USART_DMAReq_Tx, ENABLE);
//
//	/* Enable USART2 */
//	USART_Cmd(USART2, ENABLE);
//}

Usart::~Usart() {
	// TODO Auto-generated destructor stub
}

//void Usart::sendData(uint8_t *data, uint8_t size){
//
//	uint8_t _size = size;//(size < TXBUFFERSIZE ? size : TXBUFFERSIZE);
//
//	/* Wait until USARTy TX DMA1 Channel  Transfer Complete *
//	 * Protection for fast transfers 						*/
//	while (DMA_GetCurrDataCounter(DMA1_Channel7) > 1);
//
//	memcpy(TxBuffer, data, _size);
//
//	/* Specifies buffer size to send */
//	DMA_SetCurrDataCounter(DMA1_Channel7, _size);
//
//	DMA_Cmd(DMA1_Channel7, ENABLE);
//	USART_DMACmd(USART3, USART_DMAReq_Tx, ENABLE);
//
//}

void Usart::sendData(uint8_t *data, uint8_t size){

	uint8_t _size = size;//(size < TXBUFFERSIZE ? size : TXBUFFERSIZE);

	/* Wait until USARTy TX DMA1 Channel  Transfer Complete *
	 * Protection for fast transfers 						*/
	while (DMA_GetCurrDataCounter(DMA1_Channel2) > 1);

	memcpy(TxBuffer, data, _size);

	/* Specifies buffer size to send */
	DMA_SetCurrDataCounter(DMA1_Channel2, _size);

	DMA_Cmd(DMA1_Channel2, ENABLE);
	USART_DMACmd(USART3, USART_DMAReq_Tx, ENABLE);

}

