/*
 * ir.h
 * 
 * This module is responsible for the infrared record / replay
 * 
 * TBD: extend this file header with infos & name
 */


#ifndef _IR_H_
#define _IR_H_

#include "avr/interrupt.h"

/** @brief Record an IR command
 * 
 * This function records an IR command to the given uint16 array pointer.
 * It returns when the record is finished (either OK or with error).
 * 
 * @param ir Pointer to array, where the timings should be stored
 * @return 0 on success, error code otherwise
 * 
 * TBD: implement error codes (example: no IR command recorded)
 */
uint8_t ir_record_command(uint16_t * ir);


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
uint8_t ir_play_command(uint16_t * ir);

/**
 * @brief Enables the input capture functionality on Arduino pin 8
 * 
 */
void enable_input_capture();
/**
 * @brief Disables the input capture functionality on Arduino pin 8
 * 
 */
void disable_input_capture();
/**
 * @brief Enables the timer for counting ticks when replaying a command.
 * 
 */
void enable_replay_timer();
/**
 * @brief Disables the timer for counting ticks when replaying a command.
 * 
 */
void disable_replay_timer();

/**
 * @brief Disables timer for generating carrier frequency on Arduino pin 8
 * 
 */
void disable_carrier_freq();
/**
 * @brief Enables timer for generating carrier frequency on Arduino pin 8
 * 
 */
void enable_carrier_freq();
/**
 * @brief Enables watchdog for 8 sec timeout interrupt.
 * 
 */
void enable_watchdog();
/**
 * @brief Disables watchdog timer.
 * 
 */
void disable_watchdog();


extern volatile uint16_t current_timestamp;
extern volatile uint16_t recording;
extern volatile uint8_t replaying;
extern volatile uint8_t toggle_flag;
extern volatile uint8_t wait_for_start;

#define IR_SENSOR_DDR DDRB
#define IR_SENSOR_PORT PORTB
#define IR_SENSOR_PIN 0

#define IR_LED_DDR DDRB
#define IR_LED_PORT PORTB
#define IR_LED_PIN 3

enum {IR_RECORDING_SUCCESSFUL=0,IR_REPLAY_SUCCESSFUL=0,ARRAY_LIMIT_EXCEEDED,IR_NO_DATA,IR_ARRAY_NOT_EMPTY};
	
#endif /* _IR_H_ */
