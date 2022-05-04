/*
 * main.c
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
 * 
 */


#include "common.h"

/** global variable instances, see common.h for documentation */
uint16_t  ir_timings[MAX_IR_EDGES];
char ir_name[MAX_NAME_LEN];

/// currently working index (rec/replay/del)
uint8_t current_index = 0; //currently selected index
uint8_t ret_uint = 0; //general return value variable, type uint8

//TBD: add additional global variables if necessary.

int main(void)
{
	//call all setup methods.
	uart_init(115200);
	eeprom_init(); //TODO: implement error code handling here!
	ui_init();
	
	///TBD: add additional init stuff
	
	
	while(1)
	{
		//call UI part & wait for a selection/command
		switch(ui_get_selection(&current_index, ir_name))
		{
			//TBD: please use #defines here & define them in common.h!!!
			
			case 0: //record
				///at this point, ui_get_selection provided a valid name in ir_name
				/// and a valid index in current_index.
				///so we can call now IR record function to record the ir_timings
				ret_uint = ir_record_command(ir_timings);
				
				//TBD: implement error handling here!
			
				///if ir_record_command was successful (returnvalue == 0)
				/// we can store the command now
				ret_uint = eeprom_store_command(current_index, ir_name, ir_timings);
				
				
				//TBD: implement error handling here!
				
				break;
			case 1: //replay
				///at this point, ui_get_selection provided a valid current index
				///so we can call now EEPROM load to have valid ir timings
				
				ret_uint = eeprom_load_command(current_index, ir_timings);
				
				//TBD: implement error handling here!
			
				///if the load was successful, we have everything to replay the command.
				ret_uint = ir_play_command(ir_timings);
				
				
				//TBD: implement error handling here!
				
				break;
			case 2: //delete
				///at this point, ui_get_selection provided a valid current index
				///so we can call now EEPROM delete.
				ret_uint = eeprom_delete_command(current_index);
				
				//TBD: implement error handling here!
				
				break;
			default:
				uart_sendstring("Unknown return code ui_get_selection\r\n");
				break;
		}
	}
}
