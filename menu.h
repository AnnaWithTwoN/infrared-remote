/*
 * ir.h
 * 
 * This module is responsible for the infrared record / replay
 * 
 * TBD: extend this file header with infos & name
 */

#ifndef _MENU_H_
#define _MENU_H_

#include "dogm_lcd.h"

/** @brief Init UI/LCD
 *
 * This function initializes the SPI interface
 * and displays the welcome message.
 */
void ui_init();

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
uint8_t ui_get_selection(uint8_t *index, char *ir_name);

void add_name();
void menu_start();
void menu_selection_start();
uint8_t load_name();
uint8_t menu_sub_selection(uint8_t cursor_pos);

#endif /* _MENU_H_ */
