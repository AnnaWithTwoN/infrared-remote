/*
 * eeprom.h
 * 
 * This module is responsible for the storage part.
 * 
 * TBD: extend this file header with infos & name
 */

#ifndef _EEPROM_H_
#define _EEPROM_H_
#define MEMORY_SIZE 32768 // 32KB // (32 * 1024)

#define MAX_IR_EDGES 250
#define MAX_NAME_LEN 10
#define MAX_COMMANDS (MEMORY_SIZE / (MAX_IR_EDGES * 2 + MAX_NAME_LEN))

#define MAGIC_NUMBER 123
#define MAGIC_NUMBER_ADDRESS (MEMORY_SIZE - 8)

uint16_t ir_timings[MAX_IR_EDGES];

/** @brief Init EEPROM
 * 
 * Initialize I2C interface & EEPROM.
 * This function checks for a valid memory too.
 * 
 * @note EEPROM memory has an "empty" value of 0xFF!
 * @return 0 on success, error code otherwise
 * TBD: implement & document error codes
 */
uint8_t eeprom_init ();  


/** @brief Get number of stored IR commands
 * 
 * Returns the number of stored & valid IR commands.
 * If there are no commands -> 0 returned
 * 
 * @return number of commands (count)
 */
uint16_t eeprom_get_command_count ();  

int8_t eeprom_get_prev_command(int8_t* current_index, char* name);
int8_t eeprom_get_next_command(int8_t* current_index, char* name);

/** @brief Get index for a name
 * 
 * This function returns a command index (starting with 0) for
 * a given name. It searches through all commands if this name is used.
 * If yes, the index is returned, if no -1 is returned.
 * 
 * @param name Name of the command to search for
 * @return -1 if not found, index otherwise
 */
int8_t eeprom_get_command_index (char * name);  

/** @brief Get name for index
 * 
 * This function returns the name of command (stored in the pointer name)
 * for a given index.
 * 
 * @param name (out) -> Pointer where the name is stored
 * @param index Index of the command, of which the name should be returned
 * @return Length of the name string, 0 when no valid command at the index.
 */
uint8_t eeprom_get_command_name (int8_t index, char * name);  

/** @brief Store a command on a given index
 * 
 * This function is called when a command is recorded successfully.
 * It stores an IR command (ir edges / name) to the given EEPROM address
 * (calculated with the index).
 * 
 * @param ir Pointer to array of recorded edge timings
 * @param name Pointer to name of command
 * @param index Where to store this command
 * @return 0 when successful, error code otherwise
 * TBD: implement & document error codes
 */
uint8_t eeprom_store_command (int8_t index, char * name, uint16_t * ir);  

/** @brief Load a command (only IR timings) from given Index
 * 
 * This function loads the edge timings for a given index into the given
 * pointer to the array.
 * @param ir Pointer to array where the timings will be loaded
 * @param index Where to load the command from.
 * @return 0 when successful, error code otherwise
 * TBD: implement & document error codes
 */
uint8_t eeprom_load_command (int8_t index, uint16_t * ir);


/** @brief Delete IR command on given index
 * 
 * This function deletes the command on the given index.
 * 
 * @param index Which command to delete
 * @return 0 when successful, error code otherwise
 * TBD: implement & document error codes
 */
uint8_t eeprom_delete_command (int8_t index);  


#endif /* _EEPROM_H_ */
