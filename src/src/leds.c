#include "config.h"
#include "leds.h"
#include <avr/io.h>

void leds_init(void) {
    LED_DDR |= (1 << LED_RED) | (1 << LED_GREEN);
    leds_off();
}

void led_red(uint8_t on) {
    if (on) {
        LED_PORT |= (1 << LED_RED);
    } else {
        LED_PORT &= ~(1 << LED_RED);
    }
}

void led_green(uint8_t on) {
    if (on) {
        LED_PORT |= (1 << LED_GREEN);
    } else {
        LED_PORT &= ~(1 << LED_GREEN);
    }
}

void leds_off(void) {
    LED_PORT &= ~((1 << LED_RED) | (1 << LED_GREEN));
}