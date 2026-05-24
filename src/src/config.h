#ifndef CONFIG_H
#define CONFIG_H

#define F_CPU 16000000UL

#define BTN_DDR     DDRD
#define BTN_PORT    PORTD
#define BTN_PIN     PIND

#define BTN_UP      PD2
#define BTN_DOWN    PD3
#define BTN_OK      PD4
#define BTN_CHANGE  PD5

#define SERVO_DDR   DDRB
#define SERVO_PIN   PB1

#define LED_DDR     DDRB
#define LED_PORT    PORTB
#define LED_RED     PB2
#define LED_GREEN   PB3

#define LCD_I2C_ADDR    0x27
#define LCD_COLS        16
#define LCD_ROWS        2

#define PIN_LENGTH          4
#define MAX_WRONG_ATTEMPTS  3
#define LOCK_TIME_WRONG     15
#define LOCK_TIME_PANIC     10
#define BACKLIGHT_TIMEOUT   20000UL

#endif