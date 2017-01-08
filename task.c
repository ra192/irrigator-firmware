#include "task.h"

void manage_task_queue()
{
	for(int i=0;i<TASK_QUEUE_SIZE;i++)
	{	TASK* task=task_queue[i];
		if (task!=0)
		{
			if(task->delay>0) task->delay--;
			else
			{
				task_queue[i]=0;
				task->handler();
			}
		}
	}
}

void add_task(TASK* task)
{
	for(int i=0;i<TASK_QUEUE_SIZE;i++)
	{
		TASK* itm=task_queue[i];
		if(itm==0)
		{
			task_queue[i]=task;
			break;
		}
	}
}