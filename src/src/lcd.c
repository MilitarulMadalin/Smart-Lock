#include "config.h"
#include "lcd.h"
#include "i2c.h"
#include "timer.h"

#include <util/delay.h>
#include <stdio.h>

#define LCD_RS    0x01
#define LCD_RW    0x02
#define LCD_EN    0x04
#define LCD_BL    0x08

#define LCD_CLEAR           0x01
#define LCD_HOME            0x02
#define LCD_ENTRY_MODE      0x06
#define LCD_DISPLAY_ON      0x0C
#define LCD_FUNCTION_SET_4B 0x28

static uint8_t backlight_state = LCD_BL;

static void lcd_write_nibble(uint8_t nibble, uint8_t rs) {
    uint8_t data = (nibble & 0xF0) | backlight_state | (rs ? LCD_RS : 0);

    i2c_write_byte(LCD_I2C_ADDR, data | LCD_EN);
    _delay_us(1);
    i2c_write_byte(LCD_I2C_ADDR, data & ~LCD_EN);
    _delay_us(50);
}

static void lcd_send(uint8_t value, uint8_t rs) {
    lcd_write_nibble(value & 0xF0, rs);
    lcd_write_nibble((value << 4) & 0xF0, rs);
}

static void lcd_command(uint8_t cmd) {
    lcd_send(cmd, 0);
    if (cmd == LCD_CLEAR || cmd == LCD_HOME) {
        _delay_ms(2);
    }
}

void lcd_init(void) {
    _delay_ms(50);

    lcd_write_nibble(0x30, 0);
    _delay_ms(5);
    lcd_write_nibble(0x30, 0);
    _delay_us(150);
    lcd_write_nibble(0x30, 0);
    _delay_us(150);
    lcd_write_nibble(0x20, 0);
    _delay_us(150);

    lcd_command(LCD_FUNCTION_SET_4B);
    lcd_command(LCD_DISPLAY_ON);
    lcd_command(LCD_CLEAR);
    lcd_command(LCD_ENTRY_MODE);
}

void lcd_clear(void) {
    lcd_command(LCD_CLEAR);
}

void lcd_home(void) {
    lcd_command(LCD_HOME);
}

void lcd_set_cursor(uint8_t col, uint8_t row) {
    uint8_t addr = (row == 0) ? 0x00 : 0x40;
    addr += col;
    lcd_command(0x80 | addr);
}

void lcd_print_char(char c) {
    lcd_send((uint8_t)c, 1);
}

void lcd_print(const char *str) {
    while (*str) {
        lcd_print_char(*str++);
    }
}

void lcd_print_int(int value) {
    char buf[8];
    snprintf(buf, sizeof(buf), "%d", value);
    lcd_print(buf);
}

void lcd_backlight(uint8_t on) {
    backlight_state = on ? LCD_BL : 0;
    i2c_write_byte(LCD_I2C_ADDR, backlight_state);
}