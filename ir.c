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
#include "stdio.h"
#include "inttypes.h"
#include <stdint.h>
volatile uint16_t current_timestamp=0;
volatile uint16_t recording=0;
volatile uint8_t replaying=0;
volatile uint8_t toggle_flag=0;
volatile uint8_t wait_for_start=0;


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
	wait_for_start = 1;
	recording = 1;
	enable_watchdog();
	do{}
	while(wait_for_start && bit_is_set(PINB,0));
	disable_watchdog();
	
	enable_input_capture();
	
	while(recording)
	{
		//uart_sendstring("Inside IR recording\r\n");
		// if(TCNT1>32000)
		// {
		// 	sprintf(debug_string, "TCNT1 values is %d\r\n",TCNT1);
		// 	uart_sendstring(debug_string);
		// }
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


	// uint16_t sum = 0;
	// for(uint8_t i = 0;i<MAX_IR_EDGES;i++)
	// 	{
	// 		sprintf(debug_string,"The %d. timestamp is %d\r\n",i,ir[i]);
	// 		uart_sendstring(debug_string);
	// 		sum+=*(ir+i);
	// 	}
	// sprintf(debug_string,"The total command time is %d ticks.\r\n",sum);
	// uart_sendstring(debug_string);

	
	if(ip-ir==0)
	{
		uart_sendstring("No IR data was recorded.\r\n");
		return IR_NO_DATA;
	}

	uart_sendstring("Recording finished\r\n");
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
	uint8_t debug = 0;
	IR_LED_DDR |= _BV(IR_LED_PIN);//set OC2A as output
	char debug_string[100];
	uint16_t* ip;
	ip = ir;
	if(debug==10)
	{
		uart_sendstring("The contents of the array are:\r\n");
		while(*ip && ip-ir<=MAX_IR_EDGES)
		{
			sprintf(debug_string, "%d\r\n",*ip);
			uart_sendstring(debug_string);
			ip++;
		}
		ip = ir;

	}
	replaying = 1;
	toggle_flag = 0;
	enable_carrier_freq();
	OCR1A = *ip;
	enable_replay_timer();
	TCNT1 = 0;
	uint8_t cntr = 0;
	while (*ip > 0 && ip-ir<MAX_IR_EDGES)
	{
		/* code */
		if(debug)
		{
			sprintf(debug_string, "The current pointer value is %d\r\n",*ip);
			uart_sendstring(debug_string);
		}
		
		if(toggle_flag)
		{
			ip++;
			toggle_flag = 0;
			OCR1A = *ip;
			TCNT1 = 0;
			if(debug)uart_sendstring("Toggle flag raised, stepping pointer.\r\n");
			//cntr++;
		}
		
		// uart_sendstring("Current value of ip:\t");
		// sprintf(debug_string,"%d\n",*ip);
		// uart_sendstring(debug_string);

		
	}
	replaying = 0;
	disable_carrier_freq();
	disable_replay_timer();
	
	IR_LED_PORT &= ~_BV(IR_LED_PIN);
	
	uart_sendstring("Replaying finished\n");
	return 0;
}

void enable_input_capture(void){

	TCCR1B =  _BV(CS12) | _BV(ICNC1);
	// TCCR1B |= _BV(CS12) | _BV(ICNC1);
	// TCCR1B &= ~_BV(ICES1);
	TIMSK1 |= _BV(TOIE1) | _BV(ICIE1);
	TCNT1 = 0;
}

/**
 * @brief Disables the interrupts that are needed for the input capture.
 *
 */
void disable_input_capture()
{
	TCNT1 = 0;
    TCCR1B &= ~(_BV(CS12) | _BV(CS10) | _BV(CS11)) ;
}


/**
 * @brief Sets up the timer for replaying a command.
 *
 */
void enable_carrier_freq(){

     IR_LED_DDR |= _BV(IR_LED_PIN);
     //init timer0 for PWM
	TCCR0A |= _BV(COM0A0) | _BV(WGM01);	//ctc
	TCCR0B |= _BV(CS01); //8 prescale
	OCR0A =  25; //50 DTC was 25
}


/**
 * @brief Disables the timer needed for replaying a command.
 *
 */
void disable_carrier_freq(){
     TCCR0B &= ~(_BV(CS00) | _BV(CS01) | _BV(CS02));
     IR_LED_DDR &= ~_BV(IR_LED_PIN);
}

/**
 * @brief Enables the timer required for triggering edge changes.
 *
 */
void enable_replay_timer()
{
     TCCR1A = 0;
     TCCR1B = _BV(WGM12) | _BV(CS12);
     TIMSK1 |= _BV(OCIE1A);
}

/**
 * @brief Disables the timer required for triggering edge changes.
 *
 */
void disable_replay_timer()
{
     TCCR1B &= ~0x07;
}

/**
 * @brief Enables watchdog timer for 8 sec timeout in interrupt mode.
 *
 */
void enable_watchdog()
{
     WDTCSR |= _BV(WDCE) | _BV(WDE);
     WDTCSR = 0x71; //8 sec timeout

}

/**
 * @brief Disables watchdog timer.
 *
 */
void disable_watchdog()
{

     WDTCSR |= _BV(WDCE) | _BV(WDE);
     WDTCSR = 0x00;
}

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
 * @brief ISR called when no edges has been detected during recording for approx. 1 sec
 * 
 */
ISR(TIMER1_OVF_vect){
	if(recording)
	{
		recording = 0;
    	uart_sendstring("Timer overflow detected. Stopping recording.\n");
	}
    
}

/**
 * @brief Watchdog interrupt used for no input during IR recording.
 * 
 */
ISR(WDT_vect)
{
    if(wait_for_start)
    {
        uart_sendstring("TIMEOUT WHILE RECORDING\n");
        wait_for_start = 0;
        recording = 0;
    }
}