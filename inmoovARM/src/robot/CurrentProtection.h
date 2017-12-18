/*
 * CurrentProtection.h
 *
 *  Created on: Nov 17, 2017
 *      Author: xtarke
 */

#ifndef ROBOT_CURRENTPROTECTION_H_
#define ROBOT_CURRENTPROTECTION_H_

#include "../peripheral/Adc.h"

#include <stdint.h>

#ifdef __cplusplus // only actually define the class if this is C++

class CurrentProtection {
public:
	CurrentProtection();
	virtual ~CurrentProtection();

	enum {QUEUE_SIZE = 16};

	static uint16_t get_CCR1_Val() {return 12000;};

	void enqueue();
	void accValues();

	uint16_t getAverageCurrent(uint8_t channel) {return averageCurrent[channel & 0xf]; }
	uint16_t getADC(uint8_t channel) {return circularQueue[0][channel]; }

	void setProtection(uint16_t _overloadMaxCurrent, uint16_t _overshootMaxCurrent){
		overloadMaxCurrent = 	_overloadMaxCurrent;
		overshootMaxCurrent = _overshootMaxCurrent;
	}

	inline uint16_t getOverloadMaxCurrent() {return overloadMaxCurrent; }
	inline uint16_t getOvershootMaxCurrent() {return overshootMaxCurrent; }
	void overshootTest();

private:
	uint16_t CCR1_Val = 12000;

	Adc adcs;

	/* Circular queue and current index */
	volatile uint16_t circularQueue[QUEUE_SIZE][Adc::N_CHANNELS];
	volatile uint8_t queueIndex = 0;

	volatile uint16_t accumulatedCurrent[Adc::N_CHANNELS];
	volatile uint32_t averageCurrent[Adc::N_CHANNELS];

	/* Maximum allowed motor current */
	uint16_t overloadMaxCurrent = 2000;
	uint16_t overshootMaxCurrent = 6000;

	//volatile uint16_t CCR2_Val = 27309;
	//volatile uint16_t CCR3_Val = 13654;
	//volatile uint16_t CCR4_Val = 6826;
};

#else

typedef struct CurrentProtection CurrentProtection;

#endif

// access functions
#ifdef __cplusplus
    #define EXPORT_C extern "C"
#else
    #define EXPORT_C
#endif

EXPORT_C CurrentProtection* CurrentProtection_new();
EXPORT_C void CurrentProtection_delete(CurrentProtection*);
EXPORT_C void CurrentProtection_enqueue(CurrentProtection*);
EXPORT_C void CurrentProtection_accValues(CurrentProtection*);
EXPORT_C void CurrentProtection_overshootTest(CurrentProtection*);

#endif /* ROBOT_CURRENTPROTECTION_H_ */
