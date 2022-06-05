/*
 * common.c
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


#include "common.h"



/** @brief Init UART
 * 
 * @param baudrate Used baudrate
 */
void uart_init(uint32_t baudrate)
{
	//calculate baudrate register & split it into 2x1B register
    uint16_t ubrr = (F_CPU / 8 / baudrate) -1;
    UBRR0H = (uint8_t) (ubrr >> 8) ;
    UBRR0L = (uint8_t) (ubrr & 0xff);
    UCSR0B = (1<<TXEN0) | (1<<RXEN0); //enable RX&TX
    UCSR0A = (1<<U2X0);//UART double speed mode
}

/** @brief Transmit one character (blocking)
 * @param c Character to be transmitted
 * @note Blocking function!
 */
void uart_transmit(uint8_t c)
{
	while (!( UCSR0A & (1<<UDRE0))); //wait for empty data register
	UDR0 = c; //start TX by storing in the data register
}

/** @brief Transmit a string (blocking)
 * @param str String to be sent
 * @note Blocking function!
 */
void uart_sendstring(char * str )
{
	while ( * str) // send as long as not \0 terminated
	{
		//send & increment pointer
		uart_transmit(*str);
		str++;
	}
}

char* i16tos(uint16_t input) {
    // max number of numbers in uint16 is 5
    int8_t i = 5;
    static char str[6];

    str[i] = 0;
    while(i-- >= 0){
        str[i] = input % 10 + '0';
        input /= 10;
        if(input == 0){
            // convertion is finished, return
            break;
        }
    }
    // return string starting at the first non-zero number
    return &str[i];
}

void print_command(uint16_t* ir) {
    uart_sendstring("Loaded command: ");
	while(*ir){
		uart_sendstring(i16tos(*ir));
		ir++;
		uart_sendstring(", ");
	}
	uart_sendstring("\r\n");
}

int8_t str_equal(char* str1, char* str2) {
    while(*str1 || *str2){
        if(*str1 != *str2){
            return 0; // false
        }
        str1++; str2++;
    }
    return 1; // true
}

void clear_array(uint16_t* arr, uint8_t length) {
    while(length--){
        *arr = 0;
    }
}