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
int8_t cursor = 1;
int8_t line = 1;

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

void menu_start(void){
  
	lcdSetCursor(LINE1,1);
	currentwrite = REC;
	while(*currentwrite) lcdWriteChar(*currentwrite++);
	lcdSetCursor(LINE1,6);
	currentwrite = REPL;
	while(*currentwrite) lcdWriteChar(*currentwrite++);
	lcdSetCursor(LINE1,12);
	currentwrite = DEL;
	while(*currentwrite) lcdWriteChar(*currentwrite++);
	lcdSetCursor(LINE1,POS_CURSOR_REC);		
}

void add_name(void){
	char *READY = "READY";
	
	if(BUTTON_RIGHT) {
		digit++;
		if(digit == 17){ //save
			lcdClear();
			currentwrite = READY;
			while(*currentwrite) lcdWriteChar(*currentwrite++);
			digit = 0;
		 }
		lcdSetCursor(LINE2,digit);
		_delay_ms(200);
	}
	else if(BUTTON_LEFT) {
		digit--;
		if(digit == -1){
			//nameadd[17] = "                ";
			lcdClear();
			line = 1;
			menu_start();
			digit = 0;
		}
		else{
			lcdSetCursor(LINE2,digit);
			_delay_ms(200);
		}
	}
	else if(BUTTON_UP){
		if(nameadd[digit] == ' ') nameadd[digit] = 'A';
		else if(nameadd[digit] == 'Z') nameadd[digit] = 'a';
		else if(nameadd[digit] == 'z') nameadd[digit] = ' ';
		else nameadd[digit]++;
		lcdSetCursor(LINE2,digit);
		lcdWriteChar(nameadd[digit]);
		_delay_ms(200);
		}
		else if(BUTTON_DOWN){
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
	int8_t direction=0;
			if((BUTTON_RIGHT || BUTTON_LEFT)&&(line == 1)){
			if(BUTTON_RIGHT) direction=1; else direction=0;
			_delay_ms(200);	
			if(direction){
				cursor = cursor + 6;
				if(cursor == 19) cursor = POS_CURSOR_REC;
				lcdSetCursor(LINE1,cursor);	
			}
			else {
				cursor = cursor - 6;
				if(cursor == -5) cursor = POS_CURSOR_DEL;
				lcdSetCursor(LINE1,cursor);	
			}
		}
}

uint8_t menu_sub_selection(uint8_t cursor_pos){
	uint8_t numb_menu;
	switch(cursor_pos){
				case POS_CURSOR_REC:
					lcdClear();
					currentwrite = REC;
					while(*currentwrite) lcdWriteChar(*currentwrite++);
					numb_menu = 1;
					break;
				case POS_CURSOR_REPL:
					lcdClear();
					currentwrite = REPL;
					while(*currentwrite) lcdWriteChar(*currentwrite++);
					numb_menu = 2;
					break;
				case POS_CURSOR_DEL:
					lcdClear();
					currentwrite = DEL;
					while(*currentwrite) lcdWriteChar(*currentwrite++);
					numb_menu = 3;
					break;
				}
				lcdSetCursor(LINE2,0);
				return numb_menu;
			}


int main(void){
	
// Set buttons PD2, PD3, PD4, PD5

	DDRD &= ~((1<<PD2)|(1<<PD3)|(1<<PD4)|(1<<PD5));
	PORTD |= (1<<PD2)|(1<<PD3)|(1<<PD4)|(1<<PD5);
	ui_init();
	int8_t menu = 0;
	
// Show 3 words (rec, rep, del)
  /// REC => pos R = (0,1)  REPL => pos R = (0,6)  DEL => pos D = (0,12) 
  ///	             pos E = (0,2)                   pos E = (0,7)                 pos E = (0,13)
  ///	             pos C = (0,3)                   pos P = (0,8)                 pos L = (0,14)
  ///	         			                                   pos L = (0,9)
  	
	menu_start();

	while(1){

		menu_selection_start();
		
		if(BUTTON_DOWN && (line == 1)){
			line = 2;		
			menu = menu_sub_selection(cursor);
		}	
		if((line == 2) && (menu == 1)){
			add_name();
		}
		if((line == 2) && (menu == 2));
		
		if((line == 2) && (menu == 3));
	
	}

}
