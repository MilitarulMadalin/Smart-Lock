#include "config.h"
#include "timer.h"
#include "buttons.h"
#include "leds.h"
#include "servo.h"
#include "i2c.h"
#include "lcd.h"

#include <avr/io.h>
#include <stdint.h>

static int8_t correctPIN[PIN_LENGTH] = {1, 2, 3, 4};
static int8_t trollPIN[PIN_LENGTH]   = {6, 7, 6, 7};
static int8_t enteredPIN[PIN_LENGTH] = {0, 0, 0, 0};

static uint8_t currentDigit = 0;
static uint8_t wrongAttempts = 0;
static uint8_t systemLocked = 0;
static uint32_t lastButtonPress = 0;

static void reset_system(void);
static void wake_system(void);
static void update_lcd(void);
static void check_pin(void);
static uint8_t verify_old_pin(void);
static void change_password(void);
static void open_door(void);
static void lock_system(uint8_t seconds);
static void sos_morse(void);

static void setup(void) {
    timer_init();
    buttons_init();
    leds_init();
    servo_init();
    servo_set_angle(0);

    i2c_init();
    lcd_init();
    lcd_backlight(1);

    reset_system();
}

int main(void) {
    setup();

    while (1) {
        if (systemLocked) continue;

        if (millis() - lastButtonPress > BACKLIGHT_TIMEOUT) {
            lcd_backlight(0);
            leds_off();
        }

        button_t btn = button_read();
        if (btn == BUTTON_NONE) continue;

        wake_system();

        switch (btn) {
            case BUTTON_CHANGE:
                delay_ms(300);
                if (verify_old_pin()) {
                    change_password();
                } else {
                    lcd_clear();
                    lcd_print("ACCES REFUZAT!");
                    led_red(1);
                    delay_ms(2000);
                    reset_system();
                }
                break;

            case BUTTON_UP:
                enteredPIN[currentDigit]++;
                if (enteredPIN[currentDigit] > 9) enteredPIN[currentDigit] = 0;
                update_lcd();
                while (button_is_pressed(BUTTON_UP));
                break;

            case BUTTON_DOWN:
                enteredPIN[currentDigit]--;
                if (enteredPIN[currentDigit] < 0) enteredPIN[currentDigit] = 9;
                update_lcd();
                while (button_is_pressed(BUTTON_DOWN));
                break;

            case BUTTON_OK:
                while (button_is_pressed(BUTTON_OK));
                currentDigit++;
                if (currentDigit >= PIN_LENGTH) {
                    check_pin();
                    currentDigit = 0;
                } else {
                    update_lcd();
                }
                break;

            default:
                break;
        }
    }
}

static void reset_system(void) {
    for (uint8_t i = 0; i < PIN_LENGTH; i++) enteredPIN[i] = 0;
    currentDigit = 0;
    leds_off();
    lcd_clear();
    lcd_print("INTRODU PIN:");
    update_lcd();
}

static void wake_system(void) {
    lastButtonPress = millis();
    lcd_backlight(1);
}

static void update_lcd(void) {
    lcd_set_cursor(0, 1);
    for (uint8_t i = 0; i < PIN_LENGTH; i++) {
        if (i < currentDigit) {
            lcd_print(" * ");
        } else if (i == currentDigit) {
            lcd_print_char('[');
            lcd_print_int(enteredPIN[i]);
            lcd_print_char(']');
        } else {
            lcd_print(" _ ");
        }
    }
}

static void check_pin(void) {
    uint8_t correct = 1, troll = 1, reversed = 1;

    for (uint8_t i = 0; i < PIN_LENGTH; i++) {
        if (enteredPIN[i] != correctPIN[i])       correct = 0;
        if (enteredPIN[i] != trollPIN[i])         troll = 0;
        if (enteredPIN[i] != correctPIN[3 - i])   reversed = 0;
    }

    lcd_clear();

    if (troll) {
        lcd_print("AI LUAT TEAPA!");
        lcd_set_cursor(0, 1);
        lcd_print("67 LUBENITE");

        for (uint8_t i = 0; i < 15; i++) {
            led_red(1);
            led_green(0);
            delay_ms(80);
            led_red(0);
            led_green(1);
            delay_ms(80);
        }

        for (uint8_t i = 0; i < 5; i++) {
            led_red(1);
            led_green(1);
            delay_ms(150);
            led_red(0);
            led_green(0);
            delay_ms(150);
        }

        led_red(1);
        led_green(1);
        delay_ms(1500);

        reset_system();
    }
    else if (reversed && !correct) {
        lcd_print("IN PERICOL!");
        lcd_set_cursor(0, 1);
        lcd_print("SUNAM POLITIA...");

        for (uint8_t i = 0; i < 3; i++) {
            sos_morse();
            delay_ms(500);
        }

        lock_system(LOCK_TIME_PANIC);
    }
    else if (correct) {
        lcd_print("COD CORECT");
        led_green(1);
        wrongAttempts = 0;
        delay_ms(1500);
        open_door();
    }
    else {
        wrongAttempts++;
        lcd_print("COD GRESIT!");
        led_red(1);

        if (wrongAttempts >= MAX_WRONG_ATTEMPTS) {
            lock_system(LOCK_TIME_WRONG);
        } else {
            delay_ms(2000);
            reset_system();
        }
    }
}

static uint8_t verify_old_pin(void) {
    lcd_clear();
    lcd_print("PIN VECHI:");

    currentDigit = 0;
    for (uint8_t i = 0; i < PIN_LENGTH; i++) enteredPIN[i] = 0;
    update_lcd();

    while (currentDigit < PIN_LENGTH) {
        button_t btn = button_read();
        if (btn == BUTTON_NONE) continue;

        switch (btn) {
            case BUTTON_UP:
                enteredPIN[currentDigit]++;
                if (enteredPIN[currentDigit] > 9) enteredPIN[currentDigit] = 0;
                update_lcd();
                while (button_is_pressed(BUTTON_UP));
                break;

            case BUTTON_DOWN:
                enteredPIN[currentDigit]--;
                if (enteredPIN[currentDigit] < 0) enteredPIN[currentDigit] = 9;
                update_lcd();
                while (button_is_pressed(BUTTON_DOWN));
                break;

            case BUTTON_OK:
                while (button_is_pressed(BUTTON_OK));
                currentDigit++;
                if (currentDigit < PIN_LENGTH) update_lcd();
                break;

            default:
                break;
        }
    }

    for (uint8_t i = 0; i < PIN_LENGTH; i++) {
        if (enteredPIN[i] != correctPIN[i]) return 0;
    }
    return 1;
}

static void change_password(void) {
    lcd_clear();
    lcd_print("PIN NOU:");

    currentDigit = 0;
    for (uint8_t i = 0; i < PIN_LENGTH; i++) enteredPIN[i] = 0;
    update_lcd();

    while (currentDigit < PIN_LENGTH) {
        button_t btn = button_read();
        if (btn == BUTTON_NONE) continue;

        switch (btn) {
            case BUTTON_UP:
                enteredPIN[currentDigit]++;
                if (enteredPIN[currentDigit] > 9) enteredPIN[currentDigit] = 0;
                update_lcd();
                while (button_is_pressed(BUTTON_UP));
                break;

            case BUTTON_DOWN:
                enteredPIN[currentDigit]--;
                if (enteredPIN[currentDigit] < 0) enteredPIN[currentDigit] = 9;
                update_lcd();
                while (button_is_pressed(BUTTON_DOWN));
                break;

            case BUTTON_OK:
                while (button_is_pressed(BUTTON_OK));
                currentDigit++;
                if (currentDigit < PIN_LENGTH) update_lcd();
                break;

            default:
                break;
        }
    }

    for (uint8_t i = 0; i < PIN_LENGTH; i++) {
        correctPIN[i] = enteredPIN[i];
    }

    lcd_clear();
    lcd_print("SALVAT!");
    led_green(1);
    delay_ms(2000);
    reset_system();
}

static void open_door(void) {
    lcd_clear();
    lcd_print("DESCHIS");

    servo_set_angle(90);
    delay_ms(5000);
    servo_set_angle(0);

    reset_system();
}

static void lock_system(uint8_t seconds) {
    systemLocked = 1;
    led_red(1);

    for (int8_t i = seconds; i > 0; i--) {
        lcd_clear();
        lcd_print("BLOCAT: ");
        lcd_print_int(i);
        lcd_print_char('s');
        delay_ms(1000);
    }

    led_red(0);
    wrongAttempts = 0;
    systemLocked = 0;
    reset_system();
}

static void sos_morse(void) {
    const uint16_t unit = 150;

    for (uint8_t i = 0; i < 3; i++) {
        led_red(1); delay_ms(unit);
        led_red(0); delay_ms(unit);
    }
    delay_ms(unit * 2);

    for (uint8_t i = 0; i < 3; i++) {
        led_red(1); delay_ms(unit * 3);
        led_red(0); delay_ms(unit);
    }
    delay_ms(unit * 2);

    for (uint8_t i = 0; i < 3; i++) {
        led_red(1); delay_ms(unit);
        led_red(0); delay_ms(unit);
    }
}