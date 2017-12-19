/*
 * Serial.h
 *
 *  Created on: May 29, 2017
 *      Author: xtarke
 */

#ifndef COMM_SERIAL_H_
#define COMM_SERIAL_H_

#include "../peripheral/Usart.h"
#include "../data/Queue.h"

#define DATA_MAX_SIZE	64

class Serial {
private:

	/* USB VCP control */
	volatile bool USBpackageReady = false;
	uint8_t USBindex = 0;
	uint8_t USBsize = 0;
	uint8_t USBpackageData[DATA_MAX_SIZE];

	void checkUSBpackage();

	/* UART control */
	volatile bool UARTpackageReady = false;
	uint8_t UARTindex = 0;
	uint8_t UARTsize = 0;
	uint8_t UARTpackageData[DATA_MAX_SIZE];

	void checkUARTpackage();

	Usart usart;

	Queue USBQueue;
	Queue UARTQueue;

public:
	Serial();
	virtual ~Serial();

	uint8_t isPackgageReady();
	void check_package();
	void consume(uint8_t interface);

	uint8_t get_data(uint8_t i, uint8_t interface);

	/* Comm packge is: *
	 *
	 * -------------------------------------------------
	 * | PKG_START | PKG_SIZE | < PAYLOAD > | CHECKSUM |
	 * ------------------------------------- -----------
	 *
	 * Assuming PKG_START and PKG_SIZE aren't payloads
	 *
	 *
	 * Payload is:
	 * ---------------------------------------------
	 * | PKG_CMD | < PWM_DATA > or < ADC_DATA> ... |
	 * ---------------------------------------------
	 *
	 */

	enum COMM_INTERFACE { USBCOM = 1, UARTCOM = 2 };

	enum PKG_DEF {PKG_START = 0x7E, PKG_MAX_SIZE = 24, PKG_HEADER_SIZE = 2};

	enum PGK_INDEX { PKG_CMD_IDX = 0 };
	enum PKG_TYPE {PWM_DATA = 0x01, ADC_DATA = 0x11, ADC_ALL_DATA = 0x13};

	enum PWM_PKG_DEF {SERVO_ID_IDX = 1,
		SERVO_DATA_IDX = 2, SERVO_PAYLOAD_SIZE = 3, SERVO_ACK_CMD = 0x80};

	enum ADC_SINGLE_PKG_DEF {ADC_ID_IDX = 1,
		ADC_DATA_IDX = 2, ADC_PAYLOAD_SIZE = 4, ADC_ACK_CMD = 0x90};


	void getPaylod(uint8_t *data, uint8_t size, uint8_t interface);
	void makeAndSend(uint8_t *data, uint8_t size, uint8_t interface);

};

#endif /* COMM_SERIAL_H_ */
