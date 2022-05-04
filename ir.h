/*
 * ir.h
 * 
 * This module is responsible for the infrared record / replay
 * 
 * TBD: extend this file header with infos & name
 */

#ifndef _IR_H_
#define _IR_H_

/** @brief Record an IR command
 * 
 * This function records an IR command to the given uint16 array pointer.
 * It returns when the record is finished (either OK or with error).
 * 
 * @param ir Pointer to array, where the timings should be stored
 * @return 0 on success, error code otherwise
 * 
 * TBD: implement error codes (example: no IR command recorded)
 */
uint8_t ir_record_command(uint16_t * ir);


/** @brief Replay an IR command
 * 
 * This function replays a command with the given timings from ir
 * array.
 * 
 * @param ir Pointer to array, where the timings are.
 * @return 0 on success, error code otherwise
 * 
 * TBD: implement error codes
 */
uint8_t ir_play_command(uint16_t * ir);

	
#endif /* _IR_H_ */
