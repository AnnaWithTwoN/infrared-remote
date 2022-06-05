//#include "avr/iom328p.h"
#include <avr/io.h>
#include "ir.h"

/**
 * @brief Initialization of TIMER1 for input capture with needed interrupts.
 * 
 * 256 prescaler --> one timer cycle is 16 us. Overflow ISR is called after approx 1 second.
 */
void enable_input_capture(void){
     TCCR1B |= _BV(CS12) | _BV(ICNC1);
     TIMSK1 |= _BV(TOIE1) | _BV(ICIE1);
     TCNT1 = 0;
}

/**
 * @brief Disables the interrupts that are needed for the input capture.
 * 
 */
void disable_input_capture()
{
     TIMSK1 &= ~(_BV(TOIE1) | _BV(ICIE1));
}


/**
 * @brief Sets up the timer for replaying a command.
 * 
 */
void enable_carrier_freq(){
     
     IR_LED_DDR |= _BV(IR_LED_PIN);
     //init timer0 for PWM
	TCCR2A |= _BV(COM2A0) | _BV(WGM21);	//ctc
	TCCR2B |= _BV(CS21); //8 prescale
	OCR2A =  25; //50 DTC was 25
}


/**
 * @brief Disables the timer needed for replaying a command.
 * 
 */
void disable_carrier_freq(){
     TCCR2B &= ~(_BV(CS20) | _BV(CS21) | _BV(CS22));
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