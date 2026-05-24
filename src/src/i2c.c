#include "config.h"
#include "i2c.h"
#include <avr/io.h>
#include <util/twi.h>

#define TWBR_VALUE 72

void i2c_init(void) {
    TWSR = 0x00;
    TWBR = TWBR_VALUE;
    TWCR = (1 << TWEN);
}

void i2c_start(void) {
    TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
    while (!(TWCR & (1 << TWINT)));
}

void i2c_stop(void) {
    TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);
    while (TWCR & (1 << TWSTO));
}

void i2c_write(uint8_t data) {
    TWDR = data;
    TWCR = (1 << TWINT) | (1 << TWEN);
    while (!(TWCR & (1 << TWINT)));
}

void i2c_write_byte(uint8_t addr, uint8_t data) {
    i2c_start();
    i2c_write(addr << 1);
    i2c_write(data);
    i2c_stop();
}