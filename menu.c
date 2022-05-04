/*
 * ir.c
 * 
 * This module is responsible for the infrared record / replay
 * 
 * TBD: extend this file header with infos & name
 */

#include "common.h"


/** @brief Init UI/LCD
 *
 * This function initializes the SPI interface
 * and displays the welcome message.
 */
void ui_init()
{
	uart_sendstring("TBD:");
	uart_sendstring((char *)__func__);
	uart_sendstring("\r\n");
	
	lcdSpiInit();
	lcdInit();
	lcdWriteChar('B');
	//TBD: call the init function of dogm_lcd
	
	//TBD: display welcome message.
}

/** @brief Main UI/menu/LCD function
 * 
 * This function is called each time when the program requests user input
 * This function blocks as long as no command is selected.
 * 
 * When the function returns, the return value is a number for the
 * selected command. This number is used in main.c to do the corresponding
 * action (record/replay/delete).
 * 
 * @param index This pointer is set here to have an index value for main.c
 * @param ir_name This string is set here to a command name (for store)
 * @return Selected command
 * 
 * TBD: clearify the command return by using macros
 */
uint8_t ui_get_selection(uint8_t *index, char *ir_name)
{
	//this delay is necessary, otherwise this function is called
	// as fast as possible (flooding of the terminal)
	_delay_ms(1000); 
	
	uart_sendstring("TBD:");
	uart_sendstring((char *)__func__);
	uart_sendstring("\r\n");
	
	return 0;
}
