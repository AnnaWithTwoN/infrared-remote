/*
 * i2c.h
 * 
 * This module holds functions for working with i2c.
 * 
 * Authors: Anna Sidorova, FH Technikum Wien
 */

#define CONTROL_BYTE_WRITE 0b10100000
#define CONTROL_BYTE_READ 0b10100001

#define PAGE_SIZE 64

void twi_init ();

//send START condition
void twi_start ();

//send STOP condition
void twi_stop ();

//read one byte, send ACK
uint8_t twi_read_ACK ();

//read one byte, send NACK
uint8_t twi_read_NACK ();

//write one byte
void twi_write (uint8_t u8data);

//get status
uint8_t twi_getStatus ();

// write byte to I2C EEPROM (MTM)
uint8_t eeprom_write_byte (uint16_t addr, uint8_t value);

uint8_t eeprom_write_page(uint16_t addr, uint8_t* arr);

// read multiple bytes from I2C EEPROM (MRM)
uint8_t eeprom_read_bytes (uint16_t addr, uint8_t *values, uint16_t size);
