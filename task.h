#ifndef TASK_H
#define TASK_H

#include <avr/io.h>

#define TASK_QUEUE_SIZE 5

typedef struct
{
	uint16_t delay;
	void(*handler)();
} TASK;

TASK* task_queue[TASK_QUEUE_SIZE];

void manage_task_queue();
void add_task(TASK* task);

#endif
