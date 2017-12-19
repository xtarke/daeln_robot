/*
 * Queue.cpp
 *
 *  Created on: May 24, 2017
 *      Author: xtarke
 */

#include "Queue.h"
#include <string.h>
#include "diag/Trace.h"

Queue::Queue() {

	/* clean buffer */
	memset((void *)buffer, 0, QUEUE_BUFFER_SIZE);

	/* Init data */
	head = 0;
	tail = QUEUE_BUFFER_SIZE - 1;
	current_size = 0;

}

void Queue::enQueue(uint8_t _data){

	/* If there is enough space */
    if (head < QUEUE_BUFFER_SIZE){
    	current_size++;
    	tail  = (tail + 1) % QUEUE_BUFFER_SIZE;
        buffer[tail] = _data;
    }
#ifdef DEBUG
    else
    	trace_puts("Queue is Full!");
#endif
}

uint8_t Queue::deQueue(){

	uint8_t data = 0;

	if (current_size > 0){
		current_size--;
		data = buffer[head];
		head = (head + 1) % QUEUE_BUFFER_SIZE;
	}

	return data;
}


//int some_class::some_method(float f)
//{
//    return static_cast<int>(f);
//}

// access functions
EXPORT_C Queue* Queue_new(void)
{
    return new Queue();
}

EXPORT_C void Queue_delete(Queue* ptr)
{
    delete ptr;
}

EXPORT_C void Queue_enqueue(Queue* ptr, uint8_t _data)
{
    return ptr->enQueue(_data);
}
