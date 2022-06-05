/*
 * eeprom.h
 * 
 * This module is responsible for the storage part.
 * 
 * TBD: extend this file header with infos & name
 */

#include "common.h"
#include "eeprom.h"
#include "i2c.h"



/** @brief Init EEPROM
 * 
 * Initialize I2C interface & EEPROM.
 * This function checks for a valid memory too.
 * 
 * @note EEPROM memory has an "empty" value of 0xFF!
 * @return 0 on success, error code otherwise
 * TBD: implement & document error codes
 */
uint8_t eeprom_init()
{
	uart_sendstring("Initializing EEPROM...\r\n");

	twi_init();
	// clock to output in master mode
	DDRC |= (1 << PC6);

	// check if EEPROM was initialized
	uint8_t stored_magic_number = 0;
	eeprom_read_bytes(MAGIC_NUMBER_ADDRESS, &stored_magic_number, 1);

	/*
	// for debugging
	uart_sendstring("Stored magic number: ");
	uart_sendstring(i16tos(stored_magic_number));
	uart_sendstring("\r\n");*/

	if(stored_magic_number != MAGIC_NUMBER){
		// initalize EEPROM
		uart_sendstring("Initializing new EEPROM...\r\n");

		// set metadata
		eeprom_write_byte(MAGIC_NUMBER_ADDRESS, MAGIC_NUMBER);
		_delay_ms(10);

		// set the first byte of every command to 0, denoting an empty slot
		for(uint8_t i = 0; i < MAX_COMMANDS; i++){
			eeprom_write_byte(510 * i, 0);
			_delay_ms(10);
		}
	}

	
	// see list of existsing commands - for debugging
	uint8_t slot;
	char name2[10];
	for(uint8_t i = 0; i < MAX_COMMANDS; i++){
		eeprom_read_bytes(510 * i, &slot, 1);
		if(slot != 0){
			eeprom_get_command_name(i, name2);
			uart_sendstring("Command name: ");
			uart_sendstring(name2);
			uart_sendstring("\r\n");
		}
	}

	/*
	// see initial memory - for debugging
	uint8_t buffer[500];
	eeprom_read_bytes(0, buffer, 500);
	for(uint8_t i = 0; i < 20; i++){
		uart_sendstring(i16tos(buffer[i]));
		uart_sendstring(", ");
	}
	uart_sendstring("\r\n");*/

	// TODO: check for valid memory?

	uart_sendstring("EEPROM is ready\r\n");

	return 0;
}

int8_t eeprom_get_prev_command(int8_t* current_index, char* name)
{
	uint8_t slot;

	// if current index is -1 ("start from the beggining"), set it to 1
	*current_index = *current_index == -1 ? 1 : *current_index;

	for(int8_t i = (*current_index - 1 + MAX_COMMANDS) % MAX_COMMANDS; 
		i < MAX_COMMANDS + *current_index; 
		i = (i - 1 + MAX_COMMANDS) % MAX_COMMANDS){
		eeprom_read_bytes(510 * i, &slot, 1);
		if(slot != 0){
			eeprom_get_command_name(i, name);
			*current_index = i;
			/*uart_sendstring("Command name: ");
			uart_sendstring(name);
			uart_sendstring("\r\n");*/
			return 0;
		}
	}

	return -1;
}

int8_t eeprom_get_next_command(int8_t* current_index, char* name)
{
	uint8_t slot;

	for(uint8_t i = (*current_index + 1) % MAX_COMMANDS; 
		i < MAX_COMMANDS + *current_index; 
		i = (i + 1) % MAX_COMMANDS){
		eeprom_read_bytes(510 * i, &slot, 1);
		if(slot != 0){
			eeprom_get_command_name(i, name);
			*current_index = i;
			/*uart_sendstring("Command name: ");
			uart_sendstring(name);
			uart_sendstring("\r\n");*/
			return 0;
		}
	}

	return -1;
}


/** @brief Get number of stored IR commands
 * 
 * Returns the number of stored & valid IR commands.
 * If there are no commands -> 0 returned
 * 
 * @return number of commands (count)
 */
uint16_t eeprom_get_command_count()
{
	uint16_t counter = 0;

	uint8_t slot;
	for(uint8_t i = 0; i < MAX_COMMANDS; i++){
		eeprom_read_bytes(510 * i, &slot, 1);
		if(slot != 0){
			counter++;
		}
	}
	
	return counter;
}

/** @brief Get index for a name
 * 
 * This function returns a command index (starting with 0) for
 * a given name. It searches through all commands if this name is used.
 * If yes, the index is returned, if no -1 is returned.
 * 
 * @param name Name of the command to search for
 * @return -1 if not found, index otherwise
 */
int8_t eeprom_get_command_index(char * name)
{
	uart_sendstring("Getting index of command name ");
	uart_sendstring(name);
	uart_sendstring(" ...\r\n");

	char command_name[10];

	for(uint8_t i = 0; i < MAX_COMMANDS; i++){
		eeprom_read_bytes(510 * i, (uint8_t*)command_name, 10);
		if(str_equal(name, command_name)){
			return i;
		}
	}
	
	return -1;
}

/** @brief Get name for index
 * 
 * This function returns the name of command (stored in the pointer name)
 * for a given index.
 * 
 * @param name (out) -> Pointer where the name is stored
 * @param index Index of the command, of which the name should be returned
 * @return Length of the name string, 0 when no valid command at the index.
 */
uint8_t eeprom_get_command_name(int8_t index, char * name)
{
	uart_sendstring("Getting command name for index ");
	uart_sendstring(i16tos(index));
	uart_sendstring("...\r\n");

	uint16_t start_address_name = index * 510;
	// QQ: why wrong sideness for a byte?
	eeprom_read_bytes(start_address_name, (uint8_t*)name, 10);
	
	return 0;
}

/** @brief Store a command on a given index
 * 
 * This function is called when a command is recorded successfully.
 * It stores an IR command (ir edges / name) to the given EEPROM address
 * (calculated with the index).
 * TODO: add description about index=null
 * 
 * @param ir Pointer to array of recorded edge timings
 * @param name Pointer to name of command
 * @param index Where to store this command
 * @return 0 when successful, error code otherwise
 * TBD: implement & document error codes
 */
uint8_t eeprom_store_command(int8_t index, char * name, uint16_t * ir)
{
	uart_sendstring("Storing command with name ");
	uart_sendstring(name);
	uart_sendstring("...\r\n");

	if(index == -1) {
		// find first empty slot
		uint8_t slot;
		for(uint8_t i = 0; i < MAX_COMMANDS; i++){
			eeprom_read_bytes(510 * i, &slot, 1);
			/*uart_sendstring("Slot ");
			uart_sendstring(i16tos(i));
			uart_sendstring(" is ");
			uart_sendstring(i16tos(slot));
			uart_sendstring("...\r\n");*/
			if(slot == 0){
				index = i;
				break;
			}
		}
	}

	uint16_t start_address_name = index * 510;
	uint16_t start_address_command = start_address_name + 10;
	
	eeprom_write_page(start_address_name, (uint8_t*)name);
	_delay_ms(10);

	uint8_t buffer[MAX_IR_EDGES * 2];
	for(uint16_t i = 0; i < MAX_IR_EDGES * 2; i += 2){
		buffer[i] = *ir & 0xff;
		buffer[i + 1] = *ir >> 8;
		ir++;
	}
	for(uint8_t i = 0; i < MAX_IR_EDGES / PAGE_SIZE; i++){
		eeprom_write_page(start_address_command + i * PAGE_SIZE, buffer);
		_delay_ms(10);
	}

	uart_sendstring("Command stored\r\n");
	
	return 0;
}

/** @brief Load a command (only IR timings) from given Index
 * 
 * This function loads the edge timings for a given index into the given
 * pointer to the array.
 * @param ir Pointer to array where the timings will be loaded
 * @param index Where to load the command from.
 * @return 0 when successful, error code otherwise
 * TBD: implement & document error codes
 */
uint8_t eeprom_load_command(int8_t index, uint16_t * ir)
{
	uart_sendstring("Loading command at index ");
	uart_sendstring(i16tos(index));
	uart_sendstring("...\r\n");

	uint16_t start_address_command = index * 510 + 10;
	uint8_t buffer[MAX_IR_EDGES * 2];
	eeprom_read_bytes(start_address_command, buffer, MAX_IR_EDGES * 2);

	for(uint8_t i = 0; i < MAX_IR_EDGES; i++){
		ir[i] = buffer[i * 2];
		ir[i] |= (buffer[i * 2 + 1] << 8);
		uart_sendstring(i16tos(buffer[i * 2]));
		uart_sendstring(", ");
		uart_sendstring(i16tos(buffer[i * 2 + 1]));
		uart_sendstring(" -> ");
		uart_sendstring(i16tos(ir[i]));
		uart_sendstring("; ");
	}
	uart_sendstring("\r\n");

	uart_sendstring("Command loaded\r\n");
	
	return 0;
} 


/** @brief Delete IR command on given index
 * 
 * This function deletes the command on the given index.
 * 
 * @param index Which command to delete
 * @return 0 when successful, error code otherwise
 * TBD: implement & document error codes
 */
uint8_t eeprom_delete_command(int8_t index)
{
	uart_sendstring("Deleting command at index ");
	uart_sendstring(i16tos(index));
	uart_sendstring("...\r\n");

	eeprom_write_byte(510 * index, 0);
	_delay_ms(10);

	uart_sendstring("Command deleted\r\n");
	
	return 0;
}

