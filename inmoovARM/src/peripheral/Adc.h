/*
 * Adc.h
 *
 *  Created on: Jun 28, 2017
 *      Author: xtarke
 */

#ifndef PERIPHERAL_ADC_H_
#define PERIPHERAL_ADC_H_

#include <stdint.h>

class Adc {

public:
	Adc();
	virtual ~Adc();

	enum {N_CHANNELS = 7};

	uint16_t get_data(uint8_t channel);

	void get_alldata(uint16_t *data);

private:
	volatile uint16_t ADC1ConvertedValues[N_CHANNELS];

};

#endif /* PERIPHERAL_ADC_H_ */
