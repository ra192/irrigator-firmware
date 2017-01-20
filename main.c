/*
 * irrigator.cpp
 *
 * Created: 05.10.2016 11:25:57
 * Author : yakov
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>

#include "task.h"

void init()
{
	TCCR0|=(1<<CS02)|(1<<CS00);
	TIMSK=1<<TOIE0;
	
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

uint16_t EEMEM sens_high_eemem=512;
uint16_t sens_high_val;

uint16_t EEMEM sens_max_eemem=896;
uint16_t sens_max_val;

void handle_minus_button()
{
	sens_high_val--;
	eeprom_update_word(&sens_high_eemem, sens_high_val);
}

BUTTON plus_button;

void handle_plus_button()
{
	sens_high_val++;
	eeprom_update_word(&sens_high_eemem, sens_high_val);
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
	
	toggle_led_task.delay=TOGGLE_DELAY;
	add_task(&toggle_led_task);
}

TASK start_adc_task;
TASK control_switch_task;

#define ADC_DELAY 1
void start_adc()
{
	ADCSRA |= (1<<ADSC);
	control_switch_task.delay=ADC_DELAY;
	add_task(&control_switch_task);
}

#define CONTROL_SWITCH_DELAY 1024
void control_switch()
{
	if(ADC>sens_high_val && ADC<sens_max_val)
	{
		PORTD|=(1<<PD3)|(1<<PD6);
	} else
	{
		PORTD&=~((1<<PD3)|(1<<PD6));
	}
	
	start_adc_task.delay=CONTROL_SWITCH_DELAY;
	add_task(&start_adc_task);
}

int main(void)
{
	init();
	
	sens_high_val=eeprom_read_word(&sens_high_eemem);
	sens_max_val=eeprom_read_word(&sens_max_val);
	
	minus_button.handler=handle_minus_button;
	plus_button.handler=handle_plus_button;
	
	scan_buttons_task.handler=scan_buttons;
	add_task(&scan_buttons_task);
	
	toggle_led_task.handler=toggle_led;
	add_task(&toggle_led_task);
	
	start_adc_task.handler=start_adc;
	add_task(&start_adc_task);
	
	control_switch_task.handler=control_switch;
	
	while (1) 
    {
		manage_task_queue();
    }
}

