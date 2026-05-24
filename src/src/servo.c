#include "config.h"
#include "servo.h"
#include <avr/io.h>

#define SERVO_TOP       40000
#define SERVO_MIN_PULSE 1000
#define SERVO_MAX_PULSE 5000

void servo_init(void) {
    SERVO_DDR |= (1 << SERVO_PIN);

    TCCR1A = (1 << COM1A1) | (1 << WGM11);
    TCCR1B = (1 << WGM13) | (1 << WGM12) | (1 << CS11);

    ICR1 = SERVO_TOP;
    OCR1A = SERVO_MIN_PULSE;
}

void servo_set_angle(uint8_t angle) {
    if (angle > 180) angle = 180;

    uint16_t pulse = SERVO_MIN_PULSE +
                     ((uint32_t)(SERVO_MAX_PULSE - SERVO_MIN_PULSE) * angle) / 180;
    OCR1A = pulse;
}