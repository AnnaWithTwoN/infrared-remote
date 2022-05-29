/*
 * ir.c
 * 
 * This module is responsible for the infrared record / replay
 * 
 * TBD: extend this file header with infos & name
 */

#include "common.h"

int8_t digit = 0;
char nameadd[17] = "                ";
char *currentwrite;
char *REC = "REC";
char *REPL = "REPL";
char *DEL = "DEL";

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
	//lcdWriteChar('B');
	//TBD: call the init function of dogm_lcd
	lcdOnOff(LCD_ON);
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
uint8_t ui_get_selection(uint8_t *index, char *ir_name) // allow the link between the display part and the main part
{
	//this delay is necessary, otherwise this function is called
	// as fast as possible (flooding of the terminal)
	//_delay_ms(1000); 
	
	uart_sendstring("TBD:");
	uart_sendstring((char *)__func__);
	uart_sendstring("\r\n");
	
	if((line == 2) && (menu == 1)){ // If we are in the "record" part, we call the function add_name and return 0 (use by the switch case in main.c)
		add_name();
		return 0;
	}
	else if((line == 2) && (menu == 2)){ // if we are in the "replay" part, we call the function load_name and return 1.
		if (load_name() == 1) return 1;
	}	
	else if((line == 2) && (menu == 3)){ // if we are in the "delete" part, we call the function load_name and return 2
		load_name();
		return 2;
	}
	return 0;
}

void menu_start(void){
	// Initialize the display by showing the 3 options : REC, REPL, DEL
	// REC part
	lcdSetCursor(LINE1,1);
	currentwrite = REC;
	while(*currentwrite) lcdWriteChar(*currentwrite++);
	// REPL part
	lcdSetCursor(LINE1,6);
	currentwrite = REPL;
	while(*currentwrite) lcdWriteChar(*currentwrite++);
	// DEL part
	lcdSetCursor(LINE1,12);
	currentwrite = DEL;
	while(*currentwrite) lcdWriteChar(*currentwrite++);
	// Initialize cursor on position option 1 : REC
	lcdSetCursor(LINE1,POS_CURSOR_REC);		
}

uint8_t load_name(void){
	char *NO = "NO COMMAND";
	uint8_t index=0;
	uint8_t result;
	uint8_t val;
	
	result = eeprom_get_next_command(index, ir_name);
	lcdSetCursor(LINE2,0);
	// allow the naviagation between all the command saved
	if(BUTTON_UP){  // show the next command
		result = eeprom_get_next_command(index, ir_name);
		if (result == -1) {
			currentwrite = NO;
			while(*currentwrite) lcdWriteChar(*currentwrite++);
		} else {
			currentwrite = ir_name;
			while(*currentwrite) lcdWriteChar(*currentwrite++);
		}
		_delay_ms(200);
	}
	if(BUTTON_DOWN){ // show the previous command
		result = eeprom_get_prev_command(index, ir_name);
		if (result == -1) {
			currentwrite = NO;
			while(*currentwrite) lcdWriteChar(*currentwrite++);
		} else {
			currentwrite = ir_name;
			while(*currentwrite) lcdWriteChar(*currentwrite++);
		}
	_delay_ms(200);
	}
	
	if(BUTTON_LEFT){ // exit the sub menu to the main menu (REC, REPL, DEL)
		lcdClear();
		line = 1;
		menu_start();
	}
	
	if(BUTTON_RIGHT){ // validate the selection of the command
		val = 1;
		return val;
	}
	
}

void add_name(void){
	char *READY = "READY";
	// allow the navigation between each letter of the name thanks to the "digit" variable
	if(BUTTON_RIGHT) { 
		digit++;
		if(digit == 17){ // If the cursor is outside the display on the right, then we save the name enter by the user. 
			*ir_name = *nameadd;
			lcdClear();
			currentwrite = READY; // We show to the user that the process is ready
			while(*currentwrite) lcdWriteChar(*currentwrite++);
			digit = 0;	 // reset the digit for the next time
		 }
		lcdSetCursor(LINE2,digit);
		_delay_ms(200);
	}
	else if(BUTTON_LEFT) {
		digit--;
		if(digit == -1){ // if the cursor is outside the display on the left, then we come back to the main menu
			lcdClear();
			line = 1;
			menu_start();
			digit = 0; // reset the digit for the next time
		}
		else{
			lcdSetCursor(LINE2,digit);
			_delay_ms(200);
		}
	}
	// allow the selction of the letters
	else if(BUTTON_UP){ // increase the value
		if(nameadd[digit] == ' ') nameadd[digit] = 'A'; 
		else if(nameadd[digit] == 'Z') nameadd[digit] = 'a';
		else if(nameadd[digit] == 'z') nameadd[digit] = ' ';
		else nameadd[digit]++;
		lcdSetCursor(LINE2,digit);
		lcdWriteChar(nameadd[digit]);
		_delay_ms(200);
		}
		else if(BUTTON_DOWN){ // decrease the value
			if(nameadd[digit] == ' ') nameadd[digit] = 'z';
			else if(nameadd[digit] == 'a') nameadd[digit] = 'Z';
			else if(nameadd[digit] == 'A') nameadd[digit] = ' ';
			else nameadd[digit]--;
			lcdSetCursor(LINE2,digit);
			lcdWriteChar(nameadd[digit]);
			_delay_ms(200);
		}
}

void menu_selection_start(void){
	// Show 3 words (rec, rep, del)
	/// Position :
	/// REC => 	pos R = (0,1)  REPL => pos R = (0,6)  DEL => pos D = (0,12) 
	///	            pos E = (0,2)                   pos E = (0,7)                 pos E = (0,13)
	///	            pos C = (0,3)                   pos P = (0,8)                 pos L = (0,14)
	///	  
	// navigation in the main menu
	int8_t direction=0;
	
	if((BUTTON_RIGHT || BUTTON_LEFT)&&(line == 1)){ // check which button is pressed and if we are in the main menu, that means first line 
		if(BUTTON_RIGHT) direction=1; else direction=0; // RIGHT = 1, LEFT = 0
		_delay_ms(200);	
		if(direction){
		// go from an option to another, on the right
			cursor = cursor + 6;
			if(cursor >= 17) cursor = POS_CURSOR_REC; // if the cursor go further the display on the right, then the cursor go back to the first option
			lcdSetCursor(LINE1,cursor);	
		}
		else {
		// go from an option to another, on the right
		cursor = cursor - 6;
		if(cursor <= 0) cursor = POS_CURSOR_DEL; // if the cursor go further the display on the right, then the cursor go back to the first option
		lcdSetCursor(LINE1,cursor);	
		}
	}
}

uint8_t menu_sub_selection(uint8_t cursor_pos){
	uint8_t numb_menu;
	// navigation in the sub menu
	// Show only the useful information, depeding of the choice of the user
	switch(cursor_pos){
				case POS_CURSOR_REC:
					lcdClear();
					currentwrite = REC;
					while(*currentwrite) lcdWriteChar(*currentwrite++);
					numb_menu = 1; // 1 = REC
					break;
				case POS_CURSOR_REPL:
					lcdClear();
					currentwrite = REPL;
					while(*currentwrite) lcdWriteChar(*currentwrite++);
					numb_menu = 2; // 2 = REPL
					break;
				case POS_CURSOR_DEL:
					lcdClear();
					currentwrite = DEL;
					while(*currentwrite) lcdWriteChar(*currentwrite++);
					numb_menu = 3; // 3 = DEL
					break;
				}
		lcdSetCursor(LINE2,0); 
		return numb_menu;
}


