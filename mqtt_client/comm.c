/*
 * comm.c
 *
 *  Created on: Jan 3, 2018
 *      Author: Renan Augusto Starke
 */

#include <stdint.h>
#include "esp/uart.h"

#include "comm.h"

static void send_data(uint8_t *packet_buffer, uint8_t packet_size){

	uint8_t i = 0;

	for (i=0; i < packet_size; i++)
		uart_putc(0, packet_buffer[i]);
}




void makeAndSend(uint8_t *data, uint8_t paylodsize) {

	uint8_t checksum = 0xff;

	/* Checksum */
	for (uint8_t i = 0; i < paylodsize; i++){
		checksum-= data[i + PKG_HEADER_SIZE];
	}

	data[0] = PKG_START;
	data[1] = paylodsize;
	data[PKG_HEADER_SIZE + paylodsize] = checksum;


	/* Size: Header + payload + checksum */
	send_data(data, PKG_HEADER_SIZE + paylodsize + 1);
}
