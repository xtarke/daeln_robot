/*
 * Adc.h
 *
 *  Created on: Jun 28, 2017
 *      Author: xtarke
 */

#ifndef PERIPHERAL_ADC_H_
#define PERIPHERAL_ADC_H_

#include <stdint.h>

#define TEST_BUFFER_SIZE 8

void adc_init();
uint16_t adc_get_data(uint16_t sample);
void Adc_get_alldata(uint16_t *data);

uint8_t adc_test_complete();
void adc_test_read();


#endif /* PERIPHERAL_ADC_H_ */
