/*
 * Usart.h
 *
 *  Created on: Dec 11, 2017
 *      Author: xtarke
 */

#ifndef PERIPHERAL_USART_H_
#define PERIPHERAL_USART_H_

#include <stdint.h>
#include "stm32f10x_dma.h"

class Usart {

public:
	Usart();
	virtual ~Usart();

	enum {TXBUFFERSIZE = 24 };

	void sendData(uint8_t *data, uint8_t size);

private:
	uint8_t TxBuffer[TXBUFFERSIZE];

};

#endif /* PERIPHERAL_USART_H_ */
