/*
 * comm.h
 *
 *  Created on: Jan 3, 2018
 *      Author: Renan Augusto Starke
 */

#ifndef COMM_H_
#define COMM_H_

#include <stdint.h>
#include "esp/uart.h"

enum PKG_DEF {PKG_START = 0x7E, PKG_MAX_SIZE = 24, PKG_HEADER_SIZE = 2};

void makeAndSend(uint8_t *data, uint8_t paylodsize);

/* Returns the number of bytes actually available for reading.
 * No wait
 */
inline int uart_rxfifo_size(int uart_num) {
  	return FIELD2VAL(UART_STATUS_RXFIFO_COUNT, UART(uart_num).STATUS);
}


#endif /* COMM_H_ */
