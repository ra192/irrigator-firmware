/*
 * irrigator.cpp
 *
 * Created: 05.10.2016 11:25:57
 * Author : yakov
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include "task.h"

void init()
{
	TCCR0A|=(1<<CS02)|(1<<CS01);
	TIMSK0=1<<TOIE0;
	
	DDRD|=(1<<PD5)|(1<<PD6);
	
	ADMUX|=1<<REFS0;
	ADCSRA = (1<<ADEN)|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0);
}

uint16_t tick_cnt;

ISR(TIMER0_OVF_vect) {
	tick_cnt++;
}

typedef struct
{
	uint8_t curr_state, prev_state;
	void(*handler)();	
} BUTTON;

void handle_button(BUTTON* button)
{	
	 	if(button->prev_state != button->curr_state)
   		{ 
   			if(!button->curr_state) button->handler();
			button->prev_state=button->curr_state;
		}
 	
}

BUTTON minus_button;

void handle_minus_button()
{
	PORTD^=1<<PD6;
}

BUTTON plus_button;

void handle_plus_button()
{
	PORTD^=1<<PD6;
}

#define BUTTON_DELAY 255
TASK scan_buttons_task;

void scan_buttons()
{
 	minus_button.curr_state=PINB & (1<<PINB1);
 	handle_button(&minus_button);

	plus_button.curr_state=PINB & (1<<PINB0);
	handle_button(&plus_button);	

	add_task(&scan_buttons_task);
}

#define TOGGLE_DELAY 1024
TASK toggle_led_task;

void toggle_led()
{
	PORTD^=1<<PD5;
	
	add_task(&toggle_led_task);
}

TASK start_adc_task;
TASK control_switch_task;

#define ADC_DELAY 1
void start_adc()
{
	ADCSRA |= (1<<ADSC);
	add_task(&control_switch_task);
}

uint16_t sens_low_val;
#define CONTROL_SWITCH_DELAY 1024
void control_switch()
{
	if(ADC<sens_low_val)
	{
		PORTD|=1<<PD3;
	} else
	{
		PORTD&=~(1<<PD3);
	}
	
	add_task(&start_adc_task);
}

int main(void)
{
	init();
	
	minus_button.handler=handle_minus_button;
	plus_button.handler=handle_plus_button;
	
	scan_buttons_task.delay=BUTTON_DELAY;
	scan_buttons_task.handler=scan_buttons;
	
	add_task(&scan_buttons_task);
	
	toggle_led_task.delay=TOGGLE_DELAY;
	toggle_led_task.handler=toggle_led;
		
	add_task(&toggle_led_task);
	
	start_adc_task.delay=CONTROL_SWITCH_DELAY;
	start_adc_task.handler=start_adc;
	
	add_task(&start_adc_task);
	
	control_switch_task.delay=ADC_DELAY;
	control_switch_task.handler=control_switch;
	
	while (1) 
    {
		manage_task_queue();
    }
}

