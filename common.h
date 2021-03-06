/*
 * common.h
 * 
 * Copyright 2020 Benjamin Aigner <aignerb@technikum-wien.at>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 * 
 * 
 */

#ifndef _COMMON_H_
#define _COMMON_H

//include all modules
#include <avr/io.h>
#include <util/delay.h>
#include "eeprom.h"
#include "ir.h"
#include "menu.h"

#define INFO_LOGS 1
#define DEBUG_LOGS 0

// Display part
#define POS_CURSOR_INIT 0
#define POS_CURSOR_REC 1
#define POS_CURSOR_REPL 7
#define POS_CURSOR_DEL 13

#define BUTTONS_MASK ((1<<PD2)|(1<<PD3)|(1<<PD4)|(1<<PD5))
#define SOME_BUTTON_PRESSED ((PIND & BUTTONS_MASK) != BUTTONS_MASK)

#define BUTTON_RIGHT (!(PIND & (1<<PD4)))
#define BUTTON_LEFT (!(PIND & (1<<PD5)))
#define BUTTON_DOWN (!(PIND & (1<<PD3)))
#define BUTTON_UP (!(PIND & (1<<PD2)))

#define COMMAND_RECORD 0
#define COMMAND_REPLAY 1
#define COMMAND_DELETE 2

/** @brief Length of IR timings array
 * @warning Size in bytes is double (we allocate this number of uint16_t)
 */
#define MAX_IR_EDGES 250

/** @brief Max length of any IR command name
 * 
 * Please take care of the length, do not accept input names longer than this array!
 */
#define MAX_NAME_LEN 10

#define IR_EDGES_ARR_LENGTH (MAX_IR_EDGES * 2)

#define FULL_COMMAND_ARR_LENGTH (IR_EDGES_ARR_LENGTH + MAX_NAME_LEN)


/** @brief Array of timestamps between edges
 * 
 * This array is used as global "scratchpad" to store IR edges on recording
 * (because the EEPROM is too slow) or as memory for replaying a command.
 * (EEPROM is too slow on loading -> timings must be in the array before
 * replay starts).
 * 
 * @see MAXSIZE_IR_TIMINGS
 */
extern uint16_t  ir_timings[MAX_IR_EDGES];

/** @brief IR command name string
 * 
 * This array is used to store a name temporarily (either for replay or record).
 * 
 * @see MAXSIZE_IR_NAME
 */
extern char ir_name[MAX_NAME_LEN];

extern uint8_t menu;
extern int8_t cursor;
extern uint8_t line;
extern char *currentwrite;
extern char *REC;
extern char *REPL;
extern char *DEL;
extern int8_t digit;

////////////////////////////////////////////////////////////////////////
/////////// UART functions (from lecture)
////////////////////////////////////////////////////////////////////////


/** @brief Init UART
 * 
 * @param baudrate Used baudrate
 */
void uart_init(uint32_t baudrate);

/** @brief Transmit one character (blocking)
 * @param c Character to be transmitted
 * @note Blocking function!
 */
void uart_transmit(uint8_t c);

/** @brief Transmit a string (blocking)
 * @param str String to be sent
 * @note Blocking function!
 */
void uart_sendstring(char * str );

char* i16tos(uint16_t input);

void print_command(uint16_t* ir);

int8_t str_equal(char* str1, char* str2);

void clear_array(uint16_t* arr, uint16_t length);


#endif /* _COMMON_H_ */
