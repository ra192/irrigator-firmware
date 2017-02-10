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
#include "button.h"

void init()
{
	TCCR0|=(1<<CS02)|(1<<CS00);
	TIMSK=1<<TOIE0;
	
	DDRD|=(1<<PD3)|(1<<PD5)|(1<<PD6);
	DDRC|=(1<<PC1);
	
	ADMUX|=1<<REFS0;
	ADCSRA = (1<<ADEN)|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0);
}

uint16_t tick_cnt;

ISR(TIMER0_OVF_vect) {
	tick_cnt++;
}

uint16_t EEMEM sens_high_eemem=512;
uint16_t sens_high_val;

uint16_t EEMEM sens_max_eemem=896;
uint16_t sens_max_val;

#define HOLD_CNT 16

BUTTON minus_button;

void press_minus_button()
{
	sens_high_val--;
	eeprom_update_word(&sens_high_eemem, sens_high_val);
}

void hold_minus_button()
{
	sens_high_val-=HOLD_CNT;
	eeprom_update_word(&sens_high_eemem, sens_high_val);
}

BUTTON plus_button;

void press_plus_button()
{
	sens_high_val++;
	eeprom_update_word(&sens_high_eemem, sens_high_val);
}

void hold_plus_button()
{
	sens_high_val+=HOLD_CNT;
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

	scan_buttons_task.delay=BUTTON_DELAY;
	add_task(&scan_buttons_task);
}

#define TOGGLE_DELAY 2048
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
	PORTC|=1<<PORTC1;
	
	ADCSRA |= (1<<ADSC);
	control_switch_task.delay=ADC_DELAY;
	add_task(&control_switch_task);
}

#define CONTROL_SWITCH_DELAY 2048
void control_switch()
{
	PORTC&=~(1<<PORTC1);
	
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
	sens_max_val=eeprom_read_word(&sens_max_eemem);
	
	minus_button.press_handler=press_minus_button;
	minus_button.hold_handler=hold_minus_button;
	
	plus_button.press_handler=press_plus_button;
	plus_button.hold_handler=hold_plus_button;
	
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

