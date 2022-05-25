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
