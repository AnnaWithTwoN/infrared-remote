/*
 * eeprom.c
 * 
 * This module is responsible for the storage part.
 * 
 * Author: Anna Sidorova
 */

#include "common.h"
#include "eeprom.h"
#include "i2c.h"


/** @brief Init EEPROM
 * 
 * Initialize I2C interface & EEPROM.
 * 
 * @note EEPROM memory has an "empty" value of 0xFF!
 * @return 0 on success,
 */
uint8_t eeprom_init()
{
	#if INFO_LOGS
	uart_sendstring("Initializing EEPROM...\r\n");
	#endif

	twi_init();
	// clock to output in master mode
	DDRC |= (1 << PC6);

	// check if EEPROM was initialized
	uint8_t stored_magic_number = 0;
	eeprom_read_bytes(MAGIC_NUMBER_ADDRESS, &stored_magic_number, 1);

	#if DEBUG_LOGS
	uart_sendstring("Stored magic number: ");
	uart_sendstring(i16tos(stored_magic_number));
	uart_sendstring("\r\n");
	#endif

	if(stored_magic_number != MAGIC_NUMBER){
		// initalize EEPROM
		uart_sendstring("Initializing new EEPROM...\r\n");

		// set metadata
		eeprom_write_byte(MAGIC_NUMBER_ADDRESS, MAGIC_NUMBER);
		_delay_ms(10);

		// set the first byte of every command to 0, denoting an empty slot
		for(uint8_t i = 0; i < MAX_COMMANDS; i++){
			eeprom_write_byte(FULL_COMMAND_ARR_LENGTH * i, 0);
			_delay_ms(10);
		}
	}

	#if INFO_LOGS	
	// see list of existsing commands
	uint8_t slot;
	char name2[MAX_NAME_LEN];
	for(uint8_t i = 0; i < MAX_COMMANDS; i++){
		eeprom_read_bytes(FULL_COMMAND_ARR_LENGTH * i, &slot, 1);
		if(slot != 0){
			eeprom_get_command_name(i, name2);
			uart_sendstring("Command name: ");
			uart_sendstring(name2);
			uart_sendstring("\r\n");
		}
	}
	#endif

	#if DEBUG_LOGS
	// see first few bytes of memory - for debugging
	uint8_t buffer[20];
	eeprom_read_bytes(0, buffer, 20);
	for(uint8_t i = 0; i < 20; i++){
		uart_sendstring(i16tos(buffer[i]));
		uart_sendstring(", ");
	}
	uart_sendstring("\r\n");
	#endif

	#if INFO_LOGS
	uart_sendstring("EEPROM is ready\r\n");
	#endif

	return MEM_SUCCESS;
}

/**
 * @brief Get name & index of next command with respect to current index
 * 
 * Sets variables send as pointers in params to the index and name of a command,
 * found after current_index. If current_index has value of -1,
 * the search starts from index 0 (to compare, if current_index has value of 0,
 * command at index 0 is not taken into account). The search makes full
 * circle, meaning if there are only two commands with indexes n and n + k, n,k >= 0,
 * next command after n + k will be at index n.
 * 
 * @param current_index pointer to index where to start search from or -1 to start from 0
 * @param name pointer to char array where the name will be stored
 * @return 0 on success, -1 if no any commands found
 * 
 * Error codes
 * -1 MEM_NO_COMMANDS_FOUND could not find any stored commands
 */
int8_t eeprom_get_next_command(int8_t* current_index, char* name)
{
	uint8_t slot;

	for(uint8_t i = (*current_index + 1) % MAX_COMMANDS; 
		i < MAX_COMMANDS + *current_index; 
		i = (i + 1) % MAX_COMMANDS){
		eeprom_read_bytes(FULL_COMMAND_ARR_LENGTH * i, &slot, 1);
		if(slot != 0){
			eeprom_get_command_name(i, name);
			*current_index = i;

			#if DEBUG_LOGS
			uart_sendstring("Command name: ");
			uart_sendstring(name);
			uart_sendstring("\r\n");
			#endif

			return MEM_SUCCESS;
		}
	}

	return MEM_NO_COMMANDS_FOUND;
}

/**
 * @brief Get name & index of previous command with respect to current index
 * 
 * Sets variables send as pointers in params to the index and name of a command,
 * found before current_index. If current_index has value of -1,
 * the search starts from index 0 (to compare, if current_index has value of 0,
 * command at index 0 is not taken into account). The search makes full
 * circle, meaning if there are only two commands with indexes n and n + k, n,k >= 0,
 * prev command before n will be at index n + k.
 * 
 * @param current_index pointer to index where to start search from or -1 to start from 0
 * @param name pointer to char array where the name will be stored
 * @return 0 on success, -1 if no any commands found
 * 
 *  Error codes
 * -1 MEM_NO_COMMANDS_FOUND could not find any stored commands
 */
int8_t eeprom_get_prev_command(int8_t* current_index, char* name)
{
	uint8_t slot;

	// if current index is -1 ("start from the beginning"), set it to 1
	*current_index = *current_index == -1 ? 1 : *current_index;

	for(int8_t i = (*current_index - 1 + MAX_COMMANDS) % MAX_COMMANDS; 
		i < MAX_COMMANDS + *current_index; 
		i = (i - 1 + MAX_COMMANDS) % MAX_COMMANDS){
		eeprom_read_bytes(FULL_COMMAND_ARR_LENGTH * i, &slot, 1);
		if(slot != 0){
			eeprom_get_command_name(i, name);
			*current_index = i;

			#if DEBUG_LOGS
			uart_sendstring("Command name: ");
			uart_sendstring(name);
			uart_sendstring("\r\n");
			#endif

			return MEM_SUCCESS;
		}
	}

	return MEM_NO_COMMANDS_FOUND;
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
		eeprom_read_bytes(FULL_COMMAND_ARR_LENGTH * i, &slot, 1);
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
 * 
 * Error codes
 * -1 MEM_COMMAND_NOT_FOUND command not found
 */
int8_t eeprom_get_command_index(char * name)
{
	#if INFO_LOGS
	uart_sendstring("Getting index of command name ");
	uart_sendstring(name);
	uart_sendstring(" ...\r\n");
	#endif

	char command_name[MAX_NAME_LEN];

	for(uint8_t i = 0; i < MAX_COMMANDS; i++){
		eeprom_read_bytes(FULL_COMMAND_ARR_LENGTH * i, (uint8_t*)command_name, MAX_NAME_LEN);
		if(str_equal(name, command_name)){
			return i;
		}
	}
	
	return MEM_COMMAND_NOT_FOUND;
}

/** @brief Get name for index
 * 
 * This function returns the name of command (stored in the pointer name)
 * for a given index.
 * 
 * @param name (out) -> Pointer where the name is stored
 * @param index Index of the command, of which the name should be returned
 * @return 0 on success, error code otherwise
 * 
 * Error codes
 * 1 MEM_INDEX_OUT_OF_RANGE passed index does not exits
 */
uint8_t eeprom_get_command_name(uint8_t index, char * name)
{
	#if INFO_LOGS
	uart_sendstring("Getting command name for index ");
	uart_sendstring(i16tos(index));
	uart_sendstring("...\r\n");
	#endif

	if(index < 0 || index >= MAX_COMMANDS) {
		return MEM_INDEX_OUT_OF_RANGE;
	}

	uint16_t start_address_name = index * FULL_COMMAND_ARR_LENGTH;
	eeprom_read_bytes(start_address_name, (uint8_t*)name, MAX_NAME_LEN);
	
	return MEM_SUCCESS;
}

/** @brief Store a command
 * 
 * This function is called when a command is recorded successfully.
 * It stores an IR command (ir edges / name) to the given EEPROM address
 * (calculated with the index). If -1 send as index, first empty slot
 * for a command will be found.
 * 
 * @param ir Pointer to array of recorded edge timings
 * @param name Pointer to name of command
 * @param index Where to store this command
 * @return 0 when successful, error code otherwise
 * 
 * Error codes
 * 1 MEM_INDEX_OUT_OF_RANGE passed index does not exits
 * 2 MEM_OUT_OF_MEMORY      eeprom does not have any empty slot for storing command 
 */
uint8_t eeprom_store_command(int8_t index, char * name, uint16_t * ir)
{
	#if INFO_LOGS
	uart_sendstring("Storing command with name ");
	uart_sendstring(name);
	uart_sendstring("...\r\n");
	#endif

	if(index < -1 || index >= MAX_COMMANDS) {
		return MEM_INDEX_OUT_OF_RANGE;
	}

	if(index == -1) {
		// find first empty slot
		uint8_t slot;
		for(uint8_t i = 0; i < MAX_COMMANDS; i++){
			eeprom_read_bytes(FULL_COMMAND_ARR_LENGTH * i, &slot, 1);

			#if DEBUG_LOGS
			uart_sendstring("Slot ");
			uart_sendstring(i16tos(i));
			uart_sendstring(" is ");
			uart_sendstring(i16tos(slot));
			uart_sendstring("...\r\n");
			#endif

			if(slot == 0){
				index = i;
				break;
			}
		}
	}

	// no empty slots were found
	if(index == -1) {
		return MEM_OUT_OF_MEMORY;
	}

	uint16_t start_address_name = index * FULL_COMMAND_ARR_LENGTH;
	uint16_t start_address_command = start_address_name + MAX_NAME_LEN;
	while(*name) {
		eeprom_write_byte(start_address_name++, *name);
		_delay_ms(10);
		name++;
	}
	eeprom_write_byte(start_address_name++, 0);
	uint8_t counter = MAX_IR_EDGES;
	while(counter--) {
		eeprom_write_byte(start_address_command++, *ir & 0xff);
		_delay_ms(10);
		eeprom_write_byte(start_address_command++, *ir >> 8);
		_delay_ms(10);
		ir++;
	}

	#if INFO_LOGS
	uart_sendstring("Command stored\r\n");
	#endif
	
	return MEM_SUCCESS;
}

/** @brief Load a command (only IR timings) from given index
 * 
 * This function loads the edge timings for a given index into the given
 * pointer to the array.
 * 
 * @param ir Pointer to array where the timings will be loaded
 * @param index Where to load the command from.
 * @return 0 when successful, error code otherwise
 * 
 * Error codes
 * 1 MEM_INDEX_OUT_OF_RANGE passed index does not exits
 */
uint8_t eeprom_load_command(int8_t index, uint16_t * ir)
{
	#if INFO_LOGS
	uart_sendstring("Loading command at index ");
	uart_sendstring(i16tos(index));
	uart_sendstring("...\r\n");
	#endif

	if(index < 0 || index >= MAX_COMMANDS) {
		return MEM_INDEX_OUT_OF_RANGE;
	}

	uint16_t start_address_command = index * FULL_COMMAND_ARR_LENGTH + MAX_NAME_LEN;
	uint8_t buffer[IR_EDGES_ARR_LENGTH];
	eeprom_read_bytes(start_address_command, buffer, IR_EDGES_ARR_LENGTH);
	
	for(uint8_t i = 0; i < MAX_IR_EDGES; i++){
		if(!buffer[i*2]) break;
		else
		{
			ir[i] = buffer[i * 2];
			ir[i] |= (buffer[i * 2 + 1] << 8);
		}
		
		#if DEBUG_LOGS
		uart_sendstring(i16tos(buffer[i * 2]));
		uart_sendstring(", ");
		uart_sendstring(i16tos(buffer[i * 2 + 1]));
		uart_sendstring(" -> ");
		uart_sendstring(i16tos(ir[i]));
		uart_sendstring(";\r\n");
		#endif
	}

	#if INFO_LOGS
	uart_sendstring("Command loaded\r\n");
	#endif
	
	return MEM_SUCCESS;
} 


/** @brief Delete IR command on given index
 * 
 * This function deletes the command on the given index.
 * 
 * @param index Which command to delete
 * @return 0 when successful, error code otherwise
 * 
 * Error codes
 * 1 MEM_INDEX_OUT_OF_RANGE passed index does not exits
 */
uint8_t eeprom_delete_command(int8_t index)
{
	#if INFO_LOGS
	uart_sendstring("Deleting command at index ");
	uart_sendstring(i16tos(index));
	uart_sendstring("...\r\n");
	#endif

	if(index < 0 || index >= MAX_COMMANDS) {
		return MEM_INDEX_OUT_OF_RANGE;
	}

	eeprom_write_byte(FULL_COMMAND_ARR_LENGTH * index, 0);
	_delay_ms(10);

	#if INFO_LOGS
	uart_sendstring("Command deleted\r\n");
	#endif

	return MEM_SUCCESS;
}

