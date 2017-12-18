/*
 * Queue.h
 *
 *  Created on: May 24, 2017
 *      Author: xtarke
 */

#ifndef DATA_QUEUE_H_
#define DATA_QUEUE_H_

#include <stdint.h>

#define QUEUE_BUFFER_SIZE 50

#ifdef __cplusplus // only actually define the class if this is C++

class Queue {
private:
	volatile uint8_t buffer[QUEUE_BUFFER_SIZE];

	volatile uint8_t head;
	volatile uint8_t tail;
	volatile uint8_t current_size;

public:
	Queue();

	void enQueue(uint8_t _data);
	uint8_t deQueue();
	bool isEmpty() {return !current_size;};

	uint8_t get_size() {return current_size;};

	~Queue() {};
};

#else

typedef struct Queue Queue;

#endif

// access functions
#ifdef __cplusplus
    #define EXPORT_C extern "C"
#else
    #define EXPORT_C
#endif

EXPORT_C Queue* Queue_new(void);
EXPORT_C void Queue_delete(Queue*);
EXPORT_C void Queue_enqueue(Queue*, uint8_t);

#endif /* DATA_QUEUE_H_ */
