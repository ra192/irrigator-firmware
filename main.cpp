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
uint8_t button_prev_state, button_state, last_change_tick;

void handle_button(void(*callback)())
{
	if(button_prev_state != button_state)
	{
		uint8_t delay=(tick_cnt<last_change_tick)?last_change_tick:tick_cnt-last_change_tick;
		if(delay>BUTTON_HOLD_DELAY)(*callback)();
		
		button_prev_state=button_state;
		last_change_tick=tick_cnt;
	}	
}

int main(void)
{
	init();
	
	DDRB|=1<<PB3;
	
    /* Replace with your application code */
    while (1) 
    {
		if(tick_cnt%255 == 0) {
			PORTB^=1<<PB3;
		}
		
		handle_button();
    }
}

