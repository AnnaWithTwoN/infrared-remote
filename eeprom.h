/*
 * eeprom.h
 * 
 * This module is responsible for the storage part.
 * 
 * Author: Anna Sidorova
 */

#ifndef _EEPROM_H_
#define _EEPROM_H_
#define MEMORY_SIZE 32768 // 32KB // (32 * 1024)

#define MAX_IR_EDGES 250
#define MAX_NAME_LEN 10
#define MAX_COMMANDS (MEMORY_SIZE / (MAX_IR_EDGES * 2 + MAX_NAME_LEN))

#define MAGIC_NUMBER 123
#define MAGIC_NUMBER_ADDRESS (MEMORY_SIZE - 8)

// all doc commens can be found in .c file
// strategic solution - in order not to recompile headers when comments change
// and keep documentation & implementation together

uint8_t eeprom_init();  
uint16_t eeprom_get_command_count();
int8_t eeprom_get_prev_command(int8_t* current_index, char* name);
int8_t eeprom_get_next_command(int8_t* current_index, char* name);
int8_t eeprom_get_command_index (char * name);  
uint8_t eeprom_get_command_name (uint8_t index, char * name);
uint8_t eeprom_store_command (int8_t index, char * name, uint16_t * ir);  
uint8_t eeprom_load_command (int8_t index, uint16_t * ir);
uint8_t eeprom_delete_command (int8_t index);

#define MEM_SUCCESS 0
#define MEM_NO_COMMANDS_FOUND -1
#define MEM_COMMAND_NOT_FOUND -1
#define MEM_INDEX_OUT_OF_RANGE 1
#define MEM_OUT_OF_MEMORY 2


#endif /* _EEPROM_H_ */
