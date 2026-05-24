#ifndef LEDS_H
#define LEDS_H

#include <stdint.h>

void leds_init(void);
void led_red(uint8_t on);
void led_green(uint8_t on);
void leds_off(void);

#endif