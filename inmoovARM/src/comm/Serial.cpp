/*
 * Serial.cpp
 *
 *  Created on: May 29, 2017
 *      Author: xtarke
 */

#include <string.h>
#include "Serial.h"
#include "../peripheral/Timer.h"

extern "C" {
	//#include "./usb/hw_config.h"
	//#include "usb_desc.h"
	//#include "usb_pwr.h"
	//#include "usb_init.h"
	#include <stdlib.h>
	#include <stdint.h>

	void *USBQueue_ptr = NULL;
	void *UARTQueue_ptr = NULL;
	void send_data(uint8_t *packet_buffer, uint8_t packet_size);
}


Serial::Serial() {

	USBQueue_ptr = (void *)&USBQueue;
	UARTQueue_ptr = (void *)&UARTQueue;

	USBpackageReady = 0;
	USBindex = 0;
	USBsize = 0;

	UARTpackageReady = 0;
	UARTindex = 0;
	UARTsize = 0;
}

Serial::~Serial() {


}


void Serial::Talk(ServoMotor &motors){

	uint8_t responseData[PKG_MAX_SIZE];
	uint8_t comInterface = isPackgageReady();


	if (comInterface == 0){
		check_package();
	}
	else
	{
		uint8_t pkg_type = get_data(Serial::PKG_CMD_IDX, comInterface);
		uint8_t servo_id;
		uint8_t servo_position;
		uint16_t servo_current;

		switch (pkg_type){
			case Serial::PWM_DATA:
				/* Copy all data ignoring Checksum and header to make response pkg */
				getPaylod(responseData + Serial::PKG_HEADER_SIZE,
									Serial::SERVO_PAYLOAD_SIZE, comInterface);

				/* Get data */
				servo_id = responseData[Serial::SERVO_ID_IDX + Serial::PKG_HEADER_SIZE];
				servo_position = responseData[Serial::SERVO_DATA_IDX + Serial::PKG_HEADER_SIZE];
				motors.set_position(servo_id, servo_position);

				/* Send response data */
				responseData[Serial::PKG_CMD_IDX + Serial::PKG_HEADER_SIZE] = Serial::SERVO_ACK_CMD;
				makeAndSend(responseData, Serial::SERVO_PAYLOAD_SIZE, comInterface);

				break;

			case Serial::ADC_DATA:
				/* Copy all data without Checksum and header to make response pkg */
				getPaylod(responseData + Serial::PKG_HEADER_SIZE,
									Serial::SERVO_PAYLOAD_SIZE, comInterface);
				/* Get data */
				servo_id = responseData[Serial::PKG_HEADER_SIZE + Serial::ADC_ID_IDX];
				servo_current = motors.getCurrent(servo_id);

				/* Send response data */
				responseData[Serial::PKG_HEADER_SIZE + Serial::PKG_CMD_IDX] = Serial::ADC_ACK_CMD;
				/* Send current high and low data */
				responseData[Serial::PKG_HEADER_SIZE + Serial::PKG_CMD_IDX + Serial::ADC_DATA_IDX] = (servo_current >> 8);
				responseData[Serial::PKG_HEADER_SIZE + Serial::PKG_CMD_IDX + Serial::ADC_DATA_IDX + 1] = servo_current & 0xff;

				makeAndSend(responseData, Serial::ADC_PAYLOAD_SIZE, comInterface);
				break;

			case Serial::ADC_ALL_DATA:
				/* Send response data */
				responseData[Serial::PKG_HEADER_SIZE + Serial::PKG_CMD_IDX] = Serial::ADC_ACK_CMD;

				for (uint8_t i=0; i < ServoMotor::N_MOTORS*2; i+=2){
					servo_current = motors.getCurrent(i / 2);
					/* Send current high and low data */
					//responseData[Serial::PKG_HEADER_SIZE + Serial::ADC_ALL_DATA_IDX + i] = (servo_current >> 8);
					//responseData[Serial::PKG_HEADER_SIZE + Serial::ADC_ALL_DATA_IDX + i + 1] = servo_current & 0xff;
					responseData[Serial::PKG_HEADER_SIZE + Serial::ADC_ALL_DATA_IDX + i] = 0;
					responseData[Serial::PKG_HEADER_SIZE + Serial::ADC_ALL_DATA_IDX + i + 1] = 7; //servo_current & 0xff;
				}

				makeAndSend(responseData, Serial::ADC_ALL_PAYLOAD_SIZE, comInterface);
				break;

			default:
				break;

		}
		consume(comInterface);
	}
}

uint8_t Serial::get_data(uint8_t i, uint8_t interface){

	uint8_t _i = i;// & (DATA_MAX_SIZE - 1);

	switch (interface){
		case USBCOM:
			return USBpackageData[_i];
		case UARTCOM:
			return UARTpackageData[_i];
	}

	return 0xff;
}


void Serial::consume(uint8_t interface){

	switch (interface){
		case USBCOM:

			USBpackageReady = false;
			USBindex = 0;
			USBsize = 0;
			break;

		case UARTCOM:
			UARTpackageReady = false;
			UARTindex = 0;
			UARTsize = 0;
			break;

		default:
			break;
	}



}

void Serial::getPaylod(uint8_t *data, uint8_t size, uint8_t interface){

	uint8_t _size = size & (DATA_MAX_SIZE - 1);

	switch (interface){
		case USBCOM:
			memcpy(data, USBpackageData, _size);
			break;

		case UARTCOM:
			memcpy(data, UARTpackageData, _size);
			break;
		default:
			break;
	}

}

void Serial::makeAndSend(uint8_t *data, uint8_t paylodsize, uint8_t interface){

	uint8_t checksum = 0xff;

	/* Checksum */
	for (uint8_t i = PKG_HEADER_SIZE; i < paylodsize; i++){
		checksum-= data[i];
	}

	data[0] = PKG_START;
	data[1] = paylodsize;
	data[PKG_HEADER_SIZE + paylodsize] = checksum;


	switch (interface){

	case USBCOM:
		/* Size: Header + payload + checksum */
		send_data(data, PKG_HEADER_SIZE + paylodsize + 1);
		break;
	case UARTCOM:
		usart.sendData(data, PKG_HEADER_SIZE + paylodsize + 1);
		break;
	default:
		break;
	}

}

uint8_t Serial::isPackgageReady(){

	if (USBpackageReady)
		return USBCOM;
	else if (UARTpackageReady)
		return UARTCOM;
	else
		return 0;
}

void Serial::check_package(){
	checkUSBpackage();
	checkUARTpackage();

}

void Serial::checkUSBpackage(){

	volatile uint8_t data;

	while(!USBQueue.isEmpty()){

		data = USBQueue.deQueue();

		switch(USBindex){

		/* New package */
		case 0:
			/* Check start delimiter: */
			if(data == PKG_START){
				USBindex++;
			}
			break;

		/* Size is next */
		case 1:
			USBsize = data;
			USBindex++;
			break;

		/* Payload and checksum */
		default:
			/* if(index < ((size) + 2)){
				packageData[(index) - 2] = data;
				index++;
			}else{
				for(i=0; i<(index-2); i++){
					data += packageData[i];
				}
				if(data != 0xFF){

					packageReady = false;
					index = 0;
					size = 0;
				}
				else{
					packageReady = true;
					index = 0;
				}
			}*/

			/* Package size plus 2 */
			if (USBindex < (USBsize + 2)){
				USBpackageData[(USBindex) - 2] = data;
				USBindex++;
			}
			else{
				USBpackageReady = true;
				USBindex = 0;
				USBsize = 0;
			}


			break;
			}
		}
}

#include "diag/Trace.h"

void Serial::checkUARTpackage(){

	volatile uint8_t data;
	static uint8_t pkgBegin = false;

	if (pkgBegin == true && Timer::isTimeouted_01() ){
		UARTpackageReady = false;
		UARTindex = 0;
		UARTsize = 0;
		pkgBegin = false;
	}

	while(!UARTQueue.isEmpty()){

		data = UARTQueue.deQueue();

		switch(UARTindex){

		/* New package */
		case 0:
			/* Check start delimiter: */
			if(data == PKG_START){
				pkgBegin = true;
				Timer::setTimeout_01(100);
				UARTindex++;
			}
			break;

		/* Size is next */
		case 1:
			UARTsize = data;
			UARTindex++;
			break;

		/* Payload and checksum */
		default:
			/* Package size plus 2 */
			if (UARTindex < (UARTsize + 2)){
				UARTpackageData[(UARTindex) - 2] = data;
				UARTindex++;
			}
			else{
				UARTpackageReady = true;
				UARTindex = 0;
				UARTsize = 0;
				pkgBegin = false;
			}
			break;
			}
		}
}
