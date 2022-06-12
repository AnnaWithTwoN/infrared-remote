/*
 * i2c.c
 * 
 * This module holds implementations of functions for working with i2c.
 * 
 * Authors: Anna Sidorova, FH Technikum Wien
 */
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <util/twi.h>
#include "i2c.h"

void twi_init() {
	//Set I2C clock rate to 400kHz
	//adjust the TWBR according to 16MHz clock!
	TWSR = 0x00;
	TWBR = 0x0C;
	//enable the TWI
	TWCR = (1 << TWEN);
}

//send START condition
void twi_start() {
	TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
	while ((TWCR & (1 << TWINT)) == 0);
}

//send STOP condition
void twi_stop() {
	TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);
}

//read one byte, send ACK
uint8_t twi_read_ACK() {
	TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWEA);
	while ((TWCR & (1 << TWINT)) == 0);
	return TWDR;
}
//read one byte, send NACK
uint8_t twi_read_NACK() {
	TWCR = (1 << TWINT) | (1 << TWEN);
	while ((TWCR & (1 << TWINT)) == 0);
	return TWDR;
}

//write one byte
void twi_write(uint8_t u8data) {
	TWDR = u8data;
	TWCR = (1 << TWINT ) | (1 << TWEN);
	while ((TWCR & (1 << TWINT)) == 0);
}

//get status
uint8_t twi_getStatus() {
	return TWSR & 0xF8; //remove unused bits by the mask
}

// write byte to I2C EEPROM (MTM)
uint8_t eeprom_write_byte(uint16_t addr, uint8_t value) {
	twi_start();
	twi_write(CONTROL_BYTE_WRITE);
	
	// write address high byte
	twi_write(addr >> 8);
	// write address low byte
	twi_write(addr & 0xff);

	// write data
	twi_write(value);

	twi_stop();
	return 0;
}

// read multiple bytes from I2C EEPROM (MRM)
uint8_t eeprom_read_bytes(uint16_t addr, uint8_t *values, uint16_t size) {
	twi_start();
	twi_write(CONTROL_BYTE_WRITE);
	
	// write address high byte
	twi_write(addr >> 8);
	// write address low byte
	twi_write(addr & 0xff);

	twi_start();
	twi_write(CONTROL_BYTE_READ);

	// read sequential data
	while (--size) {
		*values = twi_read_ACK();
		values++;
	}
	*values = twi_read_NACK();

	twi_stop();
	return 0;
}
