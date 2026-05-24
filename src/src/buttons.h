#ifndef BUTTONS_H
#define BUTTONS_H

#include <stdint.h>

typedef enum {
    BUTTON_NONE = 0,
    BUTTON_UP,
    BUTTON_DOWN,
    BUTTON_OK,
    BUTTON_CHANGE
} button_t;

void buttons_init(void);
button_t button_read(void);
uint8_t button_is_pressed(button_t btn);

#endif