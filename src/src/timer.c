#include "config.h"
#include "timer.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

static volatile uint32_t timer_millis = 0;

ISR(TIMER0_COMPA_vect) {
    timer_millis++;
}

void timer_init(void) {
    TCCR0A = (1 << WGM01);
    TCCR0B = (1 << CS01) | (1 << CS00);
    OCR0A = 249;
    TIMSK0 = (1 << OCIE0A);
    sei();
}

uint32_t millis(void) {
    uint32_t m;
    uint8_t sreg = SREG;
    cli();
    m = timer_millis;
    SREG = sreg;
    return m;
}

void delay_ms(uint16_t ms) {
    while (ms--) {
        _delay_ms(1);
    }
}