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
	uart_sendstring("Initializing UI\r\n");
	
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
 * TBD: clarify the command return by using macros
 */
int8_t ui_get_selection() // allow the link between the display part and the main part
{
	// Show 3 words (rec, rep, del)
	// Position :
	// REC => 	pos R = (0,1)  REPL => pos R = (0,6)  DEL => pos D = (0,12) 
	//	        pos E = (0,2)          pos E = (0,7)         pos E = (0,13)
	//	        pos C = (0,3)          pos P = (0,8)         pos L = (0,14)

	uart_sendstring("Waiting for menu selection...\r\n");

	int8_t direction = 0;

	// until the button down aka "confirm selection button" is pressed, allow navigation
	while(!(BUTTON_DOWN && (line == 1))) {
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
	line = 2;

	// wait till button is released to avoid confusion in further processing
	while(SOME_BUTTON_PRESSED) {};
	
	// detect which command was selected and return the code
	return menu_sub_selection(cursor);
}

void menu_start(void){
	lcdClear();
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
	line = 1;
}

uint8_t load_name(int8_t* index){
	char *NO = "NO COMMAND";
	char ir_name[MAX_NAME_LEN];
	int8_t result;

	result = eeprom_get_next_command(index, ir_name);
	lcdSetCursor(LINE2,0);
	if (result == -1) {
		currentwrite = NO;
		while(*currentwrite) lcdWriteChar(*currentwrite++);
	} else {
		currentwrite = ir_name;
		while(*currentwrite) lcdWriteChar(*currentwrite++);
	}

	// allow list navigation until selection is confirmed by pressing button right
	while(!BUTTON_RIGHT) {
		// allow the navigation between all the command saved
		if(BUTTON_DOWN){  // show the next command
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
		if(BUTTON_UP){ // show the previous command
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
			return 1; // code of exit
		}
	}
	
	return 0; // success
}

int8_t add_name(char* ir_name){
	char *READY = "READY";
	uint8_t finished = 0;

	// set every character to initial value
	for(uint8_t i = 0; i < MAX_NAME_LEN; i++){
		ir_name[i] = 0;
	}

	// allow the navigation between each letter of the name thanks to the "digit" variable
	while(!finished) {
		if(BUTTON_RIGHT) { 
			digit++;
			if(digit == 17){ // if the cursor is outside the display on the right, then we save the name enter by the user. 
				finished = 1;
			}
			lcdSetCursor(LINE2,digit);
			_delay_ms(200);
		}
		else if(BUTTON_LEFT) {
			digit--;
			if(digit == -1){ // if the cursor is outside the display on the left, then we come back to the main menu
				lcdClear();
				line = 1;
				digit = 0; // reset the digit for the next time
				return 1; // code of exit
			}
			else{
				lcdSetCursor(LINE2,digit);
				_delay_ms(200);
			}
		}
		// allow the selction of the letters
		else if(BUTTON_UP){ // increase the value
			if(ir_name[digit] == 0) ir_name[digit] = 'A'; 
			else if(ir_name[digit] == 'Z') ir_name[digit] = 'a';
			else if(ir_name[digit] == 'z') ir_name[digit] = 0;
			else ir_name[digit]++;
			lcdSetCursor(LINE2,digit);
			lcdWriteChar(ir_name[digit]);
			_delay_ms(200);
		}
		else if(BUTTON_DOWN){ // decrease the value
			if(ir_name[digit] == 0) ir_name[digit] = 'z';
			else if(ir_name[digit] == 'a') ir_name[digit] = 'Z';
			else if(ir_name[digit] == 'A') ir_name[digit] = 0;
			else ir_name[digit]--;
			lcdSetCursor(LINE2,digit);
			lcdWriteChar(ir_name[digit]);
			_delay_ms(200);
		}
	}

	lcdClear();
	currentwrite = READY; // We show to the user that the process is ready
	while(*currentwrite) lcdWriteChar(*currentwrite++);
	digit = 0;	 // reset the digit for the next time

	return 0;
}

int8_t menu_sub_selection(uint8_t cursor_pos){
	uint8_t numb_menu = -1;
	// navigation in the sub menu
	// Show only the useful information, depeding of the choice of the user
	switch(cursor_pos){
		case POS_CURSOR_REC:
			lcdClear();
			currentwrite = REC;
			while(*currentwrite) lcdWriteChar(*currentwrite++);
			numb_menu = COMMAND_RECORD;
			break;
		case POS_CURSOR_REPL:
			lcdClear();
			currentwrite = REPL;
			while(*currentwrite) lcdWriteChar(*currentwrite++);
			numb_menu = COMMAND_REPLAY;
			break;
		case POS_CURSOR_DEL:
			lcdClear();
			currentwrite = DEL;
			while(*currentwrite) lcdWriteChar(*currentwrite++);
			numb_menu = COMMAND_DELETE;
			break;
	}
	lcdSetCursor(LINE2,0);
	return numb_menu;
}


