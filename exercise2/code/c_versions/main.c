/*
 * C11 blinker starter for Exercise Session 2, Exercise 1.
 *
 * Copy this file into the generated .c file of a project created with the
 * Raspberry Pi Pico VS Code extension (C version, i.e. "Generate C++ code"
 * NOT ticked). See exercise2/instructions.md for the full exercise.
 *
 * The LED is the PicoBricks LED on GPIO 7. The Pico W's own tiny LED sits
 * behind the WiFi chip: driving it links in a large driver and makes the
 * firmware about 20 times bigger. That is why this exercise does not use it.
 *
 * Exercise 1 asks you to rewrite this file in C++17. The constructs to
 * convert are marked below.
 */
#include <stdio.h>
#include "pico/stdlib.h"

/* #define = text substitution. Before the compiler even runs, every LED_PIN
 * in the file is replaced by the number 7. */
#define LED_PIN 7          // PicoBricks LED
#define BLINK_DELAY_MS 250 // how long each on/off phase lasts

/* enum = names for integers. LED_OFF is 0 and LED_ON is 1, nothing more.
 * The compiler will happily pass them anywhere a number is expected. */
enum led_state { LED_OFF = 0, LED_ON = 1 };

/* struct = a small bundle of data that belongs together. A blinker needs to
 * know two things: which pin its LED is on, and how fast to blink.
 * (uint is the SDK's shorthand for unsigned int, a 32-bit number that
 * cannot be negative.) */
struct blinker {
    uint pin;      // which GPIO pin the LED is on
    uint delay_ms; // how long each on/off phase lasts
};

/* A free function: it takes a pointer to a blinker and does one full blink
 * (on, wait, off, wait). static just means "used only in this file". */
static void blink_once(struct blinker* b) {
    gpio_put(b->pin, LED_ON);
    sleep_ms(b->delay_ms);
    gpio_put(b->pin, LED_OFF);
    sleep_ms(b->delay_ms);
}

int main() {
    stdio_init_all(); // start printf/puts output (Serial Monitor over the Debug Probe)

    // Make the PicoBricks LED pin an output.
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    // Create one blinker and tell it which pin to use and how fast to blink.
    struct blinker b = { .pin = LED_PIN, .delay_ms = BLINK_DELAY_MS };
    puts("C11 blinker started");

    while (true) { // runs forever, like every embedded program
        blink_once(&b);
    }
    return 0;
}
