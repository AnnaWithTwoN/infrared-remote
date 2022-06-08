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
#include "ir.h"
#include <stdio.h>

/// currently working index (rec/replay/del)
int8_t current_index = 0; // currently selected index
uint8_t ret_uint = 0;     // general return value variable, type uint8

// TBD: add additional global variables if necessary.
uint8_t menu = 0;
int8_t cursor = 1;
uint8_t line = 1;

int main(void) {
  // call all setup methods
  uart_init(115200);
  eeprom_init(); // TODO: implement error code handling here!
  ui_init();
  sei();

  DDRD &= ~((1 << PD2) | (1 << PD3) | (1 << PD4) | (1 << PD5));
  PORTD |= (1 << PD2) | (1 << PD3) | (1 << PD4) | (1 << PD5);

  uint16_t ir_timings[MAX_IR_EDGES];
  char ir_name[MAX_NAME_LEN];

  while (1) {
    menu_start(); // show main menu
    switch (ui_get_selection()) {
    case COMMAND_RECORD:
      // request name input
      if (add_name(ir_name) != 0) {
        // return to the main menu was requested
        break;
      }

      // start ir recording
      clear_array(ir_timings, MAX_IR_EDGES);
      ret_uint = ir_record_command(ir_timings);
      if (ret_uint != IR_RECORDING_SUCCESSFUL) {
        uart_sendstring("IR recording failed. Error code: ");
        uart_sendstring(i16tos(ret_uint));
        uart_sendstring("\r\n");
        break;
      }


      // uint16_t sum = 0;
      // char debug_string[50];
      // for (uint8_t i = 0; i < MAX_IR_EDGES; i++) {
        
      //   sprintf(debug_string,"The %d. timestamp is %d\r\n",i,ir_timings[i]);
      //   uart_sendstring(debug_string);
      //   sum += *(ir_timings + i);
      // }
      // sprintf(debug_string, "The total command time is %d ticks.\r\n", sum);
      // uart_sendstring(debug_string);

      // TBD: implement error handling here!

      // if ir_record_command was successful (returnvalue == 0)
      // we can store the command now
      ret_uint = eeprom_store_command(-1, ir_name, ir_timings);

      // TBD: implement error handling here!

      break;
    case COMMAND_REPLAY:
      // get the user selection of a command in current_index
      current_index =
          -1; // initialize to -1 to start searching from the beginning
      if (load_name(&current_index) != 0) {
        // return to the main menu was requested
        break;
      }
      clear_array(ir_timings, MAX_IR_EDGES);
      
      ret_uint = eeprom_load_command(current_index, ir_timings);

      // TBD: implement error handling here!

      //if the load was successful, we have everything to replay the command.
      // sum = 0;
      // for (uint8_t i = 0; i < MAX_IR_EDGES; i++) {
      //   sprintf(debug_string,"The %d. timestamp is %d\r\n",i,ir_timings[i]);
      //   uart_sendstring(debug_string);
      //   sum += *(ir_timings + i);
      // }
      // sprintf(debug_string, "The total command time is %d ticks.\r\n", sum);
      // uart_sendstring(debug_string);
      ret_uint = ir_play_command(ir_timings);

      // TBD: implement error handling here!

      break;
    case COMMAND_DELETE: // delete
      // get the user selection of a command in current_index
      current_index =
          -1; // initialize to -1 to start searching from the beginning
      if (load_name(&current_index) != 0) {
        // return to the main menu was requested
        break;
      }

      ret_uint = eeprom_delete_command(current_index);

      // TBD: implement error handling here!

      break;
    default:
      uart_sendstring("Unknown return code ui_get_selection\r\n");
      break;
    }
  }
}
