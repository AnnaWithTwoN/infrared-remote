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
#include "common.h"
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
	return(0); // success! return no error code

	// TBD: create Start Condition
	// TBD: (optional) check status (0x08: ok)
	// TBD: write EEPROM chip I2C address (MTM: R/W bit = 0)
	// TBD: (optional) check status (0x18: ok)
	// TBD: write memory address (addr high byte)
	// TBD: (optional) check status (0x28: ok)
	// TBD: write memory address (addr low byte)
	// TBD: (optional) check status (0x28: ok)
	// TBD: write data byte (value)
	// TBD: (optional) check status (0x28: ok)
	// TBD: create Stop Condition
}

uint8_t eeprom_write_page(uint16_t addr, uint8_t* arr) {
	twi_start();
	twi_write(CONTROL_BYTE_WRITE);
	
	// write address high byte
	twi_write(addr >> 8);
	// write address low byte
	twi_write(addr & 0xff);

	// write data
	uint8_t counter = PAGE_SIZE;
	// write bytes until page is over
	while(counter--){
		twi_write(*arr);
		_delay_ms(1);
		//uart_sendstring(i16tos(*arr));
		//uart_sendstring("<- in write page\r\n");
		/*if(twi_write(*arr) != 0x28){
			uart_sendstring("Write of number ");
			uart_sendstring(i16tos(*arr));
			uart_sendstring(" failed\r\n");
		}*/
		arr++;
	}

	//uart_sendstring("Trying to create stop condition\r\n");
	twi_stop();
	//uart_sendstring("Created stop condition\r\n");
	return(0); // success! return no error code

	// TBD: create Start Condition
	// TBD: (optional) check status (0x08: ok)
	// TBD: write EEPROM chip I2C address (MTM: R/W bit = 0)
	// TBD: (optional) check status (0x18: ok)
	// TBD: write memory address (addr high byte)
	// TBD: (optional) check status (0x28: ok)
	// TBD: write memory address (addr low byte)
	// TBD: (optional) check status (0x28: ok)
	// TBD: write data byte (value)
	// TBD: (optional) check status (0x28: ok)
	// TBD: create Stop Condition
}

uint8_t eeprom_write_array(uint16_t addr, uint8_t* arr, uint16_t arr_length) {
	for(uint8_t i = 0; i < arr_length / PAGE_SIZE; i++){
		uart_sendstring(i16tos(addr + i * PAGE_SIZE));
		uart_sendstring(" <- address;\r\n");
		uart_sendstring(i16tos(arr + i * PAGE_SIZE));
		uart_sendstring(" <- array pointer\r\n");
		eeprom_write_page(addr + i * PAGE_SIZE, arr + i * PAGE_SIZE);
		_delay_ms(10);
	}
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
	return(0); // success! return no error code

	// TBD: create Start Condition
	// TBD: (optional) check status (0x08: ok)
	// TBD: write EEPROM chip I2C address (MTM: R/W bit = 0)
	// TBD: (optional) check status (0x18: ok)
	// TBD: write memory address (addr high byte)
	// TBD: (optional) check status (0x28: ok)
	// TBD: write memory address (addr low byte)
	// TBD: (optional) check status (0x28: ok)
	// TBD: create another Start Condition (Restart)
	// TBD: (optional) check status (0x10: ok)
	// TBD: write EEPROM chip I2C address (MRM: R/W bit = 1)
	// TBD: (optional) check status (0x40: ok)
	// TBD: loop for size-1 bytes: read data with ACK
	// (optional) check status (0x50: ok)
	// TBD: read data byte with NACK
	// TBD: (optional) check status (0x58: ok)
	// TBD: create Stop Condition
}
