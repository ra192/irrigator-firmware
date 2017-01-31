/*
 * button.c
 *
 * Created: 31.01.2017 15:15:55
 *  Author: ra192
 */ 
#include "button.h"

void handle_button(BUTTON* button)
{
	if(!button->curr_state)
	{
		if(button->prev_state)
		{
			button->hold_cnt=0;
			if(button->press_handler) button->press_handler();
			
			button->prev_state=button->curr_state;
		}
		else
		{
			if(!button->hold_cnt++ % HOLD_HANDLER_CNT)
			{
				if(button->hold_handler) button->hold_handler();
			}
		}
	}
	else if(!button->prev_state) button->prev_state=button->curr_state;
}