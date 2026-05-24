#include "config.h"
#include "buttons.h"
#include "timer.h"
#include <avr/io.h>

#define DEBOUNCE_MS 30

void buttons_init(void) {
    BTN_DDR &= ~((1 << BTN_UP) | (1 << BTN_DOWN) | (1 << BTN_OK) | (1 << BTN_CHANGE));
    BTN_PORT |= (1 << BTN_UP) | (1 << BTN_DOWN) | (1 << BTN_OK) | (1 << BTN_CHANGE);
}

static uint8_t pin_low(uint8_t pin) {
    return !(BTN_PIN & (1 << pin));
}

uint8_t button_is_pressed(button_t btn) {
    switch (btn) {
        case BUTTON_UP:     return pin_low(BTN_UP);
        case BUTTON_DOWN:   return pin_low(BTN_DOWN);
        case BUTTON_OK:     return pin_low(BTN_OK);
        case BUTTON_CHANGE: return pin_low(BTN_CHANGE);
        default: return 0;
    }
}

button_t button_read(void) {
    button_t pressed = BUTTON_NONE;

    if (pin_low(BTN_UP))           pressed = BUTTON_UP;
    else if (pin_low(BTN_DOWN))    pressed = BUTTON_DOWN;
    else if (pin_low(BTN_OK))      pressed = BUTTON_OK;
    else if (pin_low(BTN_CHANGE))  pressed = BUTTON_CHANGE;

    if (pressed == BUTTON_NONE) return BUTTON_NONE;

    delay_ms(DEBOUNCE_MS);

    if (!button_is_pressed(pressed)) return BUTTON_NONE;

    return pressed;
}