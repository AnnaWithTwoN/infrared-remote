/*
 * ir.h
 * 
 * This module is responsible for the infrared record / replay
 * 
 * TBD: extend this file header with infos & name
 */

#include "common.h"
#include "avr/io.h"
#include "ir.h"
#include <stdio.h>
#include "timer.h"

/**
 * @brief ISR called when edge changes on PD6
 * 
 */
ISR(TIMER1_CAPT_vect)
{
    if(recording)
    {
        current_timestamp = ICR1;
        TCCR1B ^= _BV(ICES1);
        TCNT1 = 0;
    }
}

/**
 * @brief Timer used for triggering the carrier frequency during replay
 * 
 */
ISR(TIMER1_COMPA_vect)
{
    if(replaying)
    {
        IR_LED_DDR ^= _BV(IR_LED_PIN);
        toggle_flag = 1;
    }
}

/**
 * @brief ISR called when no more edges has been detected during recording
 * 
 */
ISR(TIMER1_OVF_vect){
    recording = 0;
    uart_sendstring("OVERFLOW detected. Stopping recording.\n");
}

/**
 * @brief Watchdog interrupt used for no input during IR recording.
 * 
 */
ISR(WDT_vect)
{
    if(recording)
    {
        uart_sendstring("TIMEOUT WHILE RECORDING\n");
        wait_for_start = 0;
        recording = 0;
    }
}

/** @brief Record an IR command
 * 
 * This function records an IR command to the given uint16 array pointer.
 * It returns when the record is finished (either OK or with error).
 * 
 * @param ir Pointer to array, where the timings should be stored
 * @return 0 on success, error code otherwise
 * 
 * 
 */

uint8_t ir_record_command(uint16_t * ir)
{
	uart_sendstring("Starting IR recording...\r\n");
	if(*ir>0) return IR_ARRAY_NOT_EMPTY;
	//The edges with an odd index are falling edges, the even ones are rising edges.
	char debug_string[100];
	uint16_t* ip;
	ip = ir;
	current_timestamp = 0;
	recording = 1;
	enable_watchdog();
	do{}
	while(wait_for_start && bit_is_set(PINB,0));
	disable_watchdog();
	
	enable_input_capture();
	
	while(recording)
	{
		uart_sendstring("Inside IR recording\r\n");
		if((ip-ir)> MAX_IR_EDGES)
		{
			disable_input_capture();
			uart_sendstring("Array limit exceeded\r\n");
			return ARRAY_LIMIT_EXCEEDED;
		}
		if(current_timestamp)
		{
			*ip = current_timestamp;
			current_timestamp = 0;
			ip++;
			//TCNT1 = 0;
		}
	}
	disable_input_capture();

	
	uart_sendstring("uart recording successful\r\n");
	uint16_t sum = 0;
	for(uint8_t i = 0;i<MAX_IR_EDGES;i++)
		{
			if(!(*(ir+i))) break;
			//sprintf(debug_string,"The %d. timestamp is %d\n",i,ir_timings[i]);
			//uart_sendstring(debug_string);
			sum+=*(ir+i);
		}
	//sprintf(debug_string,"The total command time is %d ticks.\n",sum);
	//uart_sendstring(debug_string);

	uart_sendstring("Recording finished");
	if(ip-ir==0)
	{
		uart_sendstring("No IR data was recorded.\n");
		return IR_NO_DATA;
	}
	return IR_RECORDING_SUCCESSFUL;	
}


/** @brief Replay an IR command
 * 
 * This function replays a command with the given timings from ir
 * array.
 * 
 * @param ir Pointer to array, where the timings are.
 * @return 0 on success, error code otherwise
 * 
 * TBD: implement error codes
 */
uint8_t ir_play_command(uint16_t * ir)
{
	uint8_t debug = 1;
	IR_LED_DDR |= _BV(IR_LED_PIN);//set OC2A as output
	char debug_string[100];
	uint16_t* ip;
	ip = ir;
	replaying = 1;
	toggle_flag = 0;
	enable_carrier_freq();
	OCR1A = *ip;
	enable_replay_timer();
	TCNT1 = 0;
	while (*ip != 0 && ir-ip<MAX_IR_EDGES)
	{
		/* code */
		if(toggle_flag)
		{
			ip++;
			toggle_flag = 0;
			OCR1A = *ip;
			//TCCR2B |= _BV(CS12);
			TCNT1 = 0;
		}
		/*
		uart_sendstring("Current value of ip:\t");
		sprintf(debug_string,"%d\n",*ip);
		uart_sendstring(debug_string);*/

		
	}
	disable_carrier_freq();
	disable_replay_timer();
	replaying = 0;
	IR_LED_PORT &= ~_BV(IR_LED_PIN);
	
	uart_sendstring("Replaying finished\n");
	return 0;
}
