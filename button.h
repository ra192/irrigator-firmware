/*
 * button.h
 *
 * Created: 31.01.2017 15:14:00
 *  Author: ra192
 */ 


#ifndef BUTTON_H_
#define BUTTON_H_

#include <avr/io.h>

#define HOLD_HANDLER_CNT 32

typedef struct
{
	uint8_t curr_state, prev_state, hold_cnt;
	
	void(*press_handler)();
	void(*hold_handler)();
} BUTTON;

void handle_button(BUTTON* button);

#endif /* BUTTON_H_ */