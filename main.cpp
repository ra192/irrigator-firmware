/*
 * irrigator.cpp
 *
 * Created: 05.10.2016 11:25:57
 * Author : yakov
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>

void init()
{
	TCCR0A|=(1<<CS02)|(1<<CS01);
	TIMSK0=1<<TOIE0;
}

uint16_t tick_cnt;

ISR(TIMER0_OVF_vect) {
	tick_cnt++;
}

#define BUTTON_HOLD_DELAY 5


typedef struct
{
	uint8_t curr_state, prev_state, last_change_tick;
	void(*handler)();	
} BUTTON;

void handle_button(BUTTON button)
{
	if(button.prev_state != button.curr_state)
	{
		uint8_t delay=(tick_cnt<button.last_change_tick)?button.last_change_tick:tick_cnt-button.last_change_tick;
		if(delay>BUTTON_HOLD_DELAY && !button.curr_state)(*button.handler)();
		
		button.prev_state=button.curr_state;
		button.last_change_tick=tick_cnt;
	}	
}

typedef struct  
{
	uint16_t delay;
	void(*handler)();
} TASK;

#define TASK_QUEUE_SIZE 5
TASK* task_queue[TASK_QUEUE_SIZE];

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
				(*task->handler)();	
			}
		}
	}
}

void add_task(TASK* task)
{
	for(int i=0;i<TASK_QUEUE_SIZE;i++)
	{
		TASK* task=task_queue[i];
		if(task==0)
		{
			task_queue[i]=task;
			break;
		}
	}
}

BUTTON button_minus_button;
TASK button_minus_task;

void button_minus_handler()
{
	PORTB^=1<<PB4;
}

#define BUTTON_DELAY 5

void button_minus()
{
	button_minus_button.curr_state=PINB & (1<<PINB0);
	handle_button(button_minus_button);
	
	button_minus_task.delay=BUTTON_DELAY;
	add_task(&button_minus_task);
}

#define TOGGLE_DELAY 255
TASK toggle_led_task;

void toggle_led()
{
	PORTB^=1<<PB3;
	
	toggle_led_task.delay=TOGGLE_DELAY;
	add_task(&toggle_led_task);
}

int main(void)
{
	init();
	
	button_minus_button.handler=button_minus_handler;
	button_minus_task.handler=button_minus;
	
	toggle_led_task.handler=toggle_led;
	
	add_task(&button_minus_task);
		
	DDRB|=1<<PB3;
	
    /* Replace with your application code */
    while (1) 
    {
		manage_task_queue();
    }
}

