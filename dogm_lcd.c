/*! 
  \file       dogm_lcd.c
  \brief      Library for a EA DOGM162L-A 16x2 display

              This library includes all functions to work with the EA DOGM162L-A 
			  16x2 display. The library is written for the Arduino Uno Board
			  with Atmega328p microcontroller with a 16MHz clock. Before the 
			  display function could be used the SPI interface of the controller
			  must be initialized in master mode with a speed of (fosc /32).The
			  functions are segmented in control functions and write functions.

  \attention  The information contained herein is confidential property of the
              Institute of Embedded Systems - Technikum Wien. The use, copying,
              transfer or disclosure of such information is prohibited except 
              by express written agreement with the Embedded Systems Institute.

  \author     H. Schuster
  \date       25/10/2011
  \version    1.0

*/

#ifndef F_CPU
	#define F_CPU 16000000
#endif

#include "dogm_lcd.h"
#include <stdarg.h>
#include <stdio.h>

// Ports for the display (Arduino I/O Board FH-TW::Embsys)
#define PORT_DIRECTION DDRB
#define PORT_VALUE PORTB
#define RS DDB1
#define SCK DDB5
#define MISO DDB4
#define MOSI DDB3
#define SS DDB2

// Maximum defines for the display
#define MAX_ROW 1
#define MAX_COL 15

// Defines for the RS pin
#define RS_DATA PORT_VALUE |= (1 << RS);
#define RS_INSTRUCTION PORT_VALUE &= ~(1 << RS);

// Defines for the SS pin
#define SS_UNSELECT PORT_VALUE |= (1 << SS);
#define SS_SELECT PORT_VALUE &= ~(1 << SS);

// Internal functions to write a byte to the display
void writeCommand(uint8_t cmd);


/*********************************************************************/
 /**
 * \brief  Function to initialize SPI for the LCD display (DOGM1162L-A)
 *
 *         This function initializes the SPI interface of the 
 *         Atmega 328P-PU for the usage with the Arduino I/O Board
 *         of the FH Technikum Wien. 
 *
 * \param       No input parameter
 * \return		No return value
 *
 */
void lcdSpiInit()
{
	DDRB |= (1 << DDB3) | (1 << DDB5) | (1 << DDB2); // MOSI, SS, SCK output
	DDRB &= ~(1 << DDB4); // MISO input

	//init SPI
	// SPI2X...Double SPI Speed Bit
	// SPE...SPI Enable Bit
	// MSTR...Master SPI Mode
	// SPR1...SCK Frequency set to Fosc/32
	SPSR |= (1<<SPI2X);  
	SPCR |= ((1<<SPE) | (1<<MSTR) | (1<<SPR1) | (1<<CPOL) | (1<<CPHA));
}

/*********************************************************************/
 /**
 * \brief  Function to initialize the LCD display (DOGM1162L-A)
 *
 *         This function initializes the used LCD display and sets
 *         the cursor to the home position. The functions set ups
 *         the display type, the contrast, the voltage (5V). The
 *         timing depend on the used SPI configuration (fosc/32)
 *
 * \param       No input parameter
 * \return		No return value
 *
 */
void lcdInit()
{
	SS_UNSELECT
	_delay_ms(1);
	
	PORT_DIRECTION |= (1 << RS); // RS output
	RS_INSTRUCTION
	// wait 60ms to get stable VDD
	_delay_ms(60);
	// 8 bit data, 2 line data, instruction table 1
	writeCommand(0x39);
	_delay_us(20);

	// 1/4 Bias, 2 line LCD
 	writeCommand(0x1C);
	_delay_us(20);
	
	// Booster off, contrast C5, set C4
	writeCommand(0x52);
	_delay_us(20);

	// Set voltage follower and gain
	writeCommand(0x69);
	_delay_ms(200);
	
	// Set Contrast C3, C2, C1
	writeCommand(0x74);;
	_delay_us(20);
	
	// Switch back to instruction table 0
	writeCommand(0x38);
	_delay_us(20);
	
	// Display on, cursor on, cursor blink
	writeCommand(0x0f);
	_delay_us(20);
	
	// Clear the display
	writeCommand(0x01);
	_delay_ms(1);

	// Set cursor auto increment
	writeCommand(0x06);
	_delay_us(20);

	SS_UNSELECT
}

/*********************************************************************/
 /**
 * \brief  Function switch the display on and off
 *
 *         With this function it is possible to switch the display
 *         on and off. When a already initiailized display is 
 *         switched off the display is not cleared. When the display
 *         is switched on again the text is still there and the 
 *         cursor is switched on.
 *
 * \param       mode (LCD_ON / LCD_OFF)
 * \return		No return value
 *
 */
void lcdOnOff(uint8_t mode)
{
	RS_INSTRUCTION
	
	if(mode == LCD_ON)
	{
		writeCommand(0x0F);
		_delay_us(20);
	}
	else if (mode == LCD_OFF)
	{
		writeCommand(0x08);
		_delay_us(20);
	}
}

/*********************************************************************/
 /**
 * \brief  Function to clear the display
 *
 *         This function clears the display and switches on the 
 *         cursor.
 *
 * \param       No input parameter
 * \return		No return value
 *
 */
void lcdClear()
{
	RS_INSTRUCTION
	writeCommand(0x01);
	_delay_ms(1);
}

/*********************************************************************/
 /**
 * \brief  Function to write a charakter to the display
 *
 *         This function writes a single character to the display.
 *         Beside the write command it also sets the RS pin for
 *         the transfer of a databyte.
 *
 * \param       character x
 * \return		No return value
 *
 */
void lcdWriteChar(char x)
{	
	RS_DATA
	writeCommand(x);
	_delay_us(20);
}

/*********************************************************************/
 /**
 * \brief  Function to write a string to the display
 *
 *         This function writes a string to the display. It uses
 *         the lcdWriteChar function to write the single character.
 *         It is possible to choose if both or only one row of
 *         the Display is used
 *
 * \param       row (starting row), twoLines ()
 * \return		returns the number of written characters or -1 if
 *				string was too long
 *
 */
int  lcdWriteString(uint8_t row,uint8_t twoLines, const char * format, ...)
{
	char text[33];
	uint8_t pos = 0;
	uint8_t length = 0;
	uint8_t maxLength = 17;
	va_list args;

	// set the cursor and the rigth length
	if(twoLines == TWO_LINES_ON)
	{
		maxLength = 33;
		lcdSetCursor(0, 0);
	}
	else	
		lcdSetCursor(row, 0);	

	// Generate the string for the display
	va_start( args, format );
    length = vsnprintf(text, maxLength, format, args );
	va_end( args );

	while(pos < maxLength)
	{
		if (pos == 16 && twoLines == TWO_LINES_ON)
			lcdSetCursor(1, 0);
		if(pos >= length && length != -1)
			lcdWriteChar(' ');
		else	
			lcdWriteChar(text[pos]);
		pos++;
	}
	
	if(length == -1)
		return ERROR;
	
	return OK;
}

/*********************************************************************/
 /**
 * \brief  Function to set the cursor to a specific position
 *
 *         This function sets the cursor to a specific position. The
 *         position is represented by the row and the column. If the 
 *         user input crosses the maximum values the position is set
 *         to the maximum values.
 *
 * \param       row and col (column)
 * \return		No return value
 *
 */
void lcdSetCursor(uint8_t row, uint8_t col)
{
	RS_INSTRUCTION
	if(row > MAX_ROW)
		row = MAX_ROW;
	if(col > MAX_COL)
		col = MAX_COL;

	if(row == LINE2)
		row = 0x40;

	row += col;

	// Set DDRAM Address
	writeCommand(0x80|row);
	_delay_us(20);
}

/*********************************************************************/
 /**
 * \brief  Function to switch the cursor and the cursor position on 
 *         and off
 *
 *         With this function it is possible to switch the cursor and
 *         the cursor position on and off.
 *
 * \param       cursorOnOff (CURSOR_ON / CURSOR_OFF)
 * \param       positionOnOff (POSITION_ON / POSITION_ON)
 * \return		No return value
 *
 */
void lcdCursorOnOff(uint8_t cursorOnOff, uint8_t positionOnOff)
{
	RS_INSTRUCTION

	if(cursorOnOff == CURSOR_ON)
		cursorOnOff = 0x02;
	else	
		cursorOnOff = 0x00;

	if(positionOnOff == POSITION_ON)
		cursorOnOff |= 0x01;
	else	
		cursorOnOff &= 0xFE;
		
	writeCommand(0x0C|cursorOnOff);
	_delay_us(20);
}

/*********************************************************************/
 /**
 * \brief  Function to send a byte to the display via SPI
 *
 *         This function send a byte via SPI to the display. Before
 *         sending the byte the slave select (SS) pin is set to zero.
 *         After wrtiting the byte to the send register of the SPI
 *         the function waits that the message was send successfully.
 *
 * \param       cmd (command to be send)
 * \return		No return value
 *
 */
void writeCommand(uint8_t cmd)
{
	SS_SELECT
	SPDR = cmd;
	while(!(SPSR & (1 << SPIF)));
	SS_UNSELECT
}




