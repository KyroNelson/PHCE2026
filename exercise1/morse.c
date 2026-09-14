#include "pico/stdlib.h"

// Pico W devices use a GPIO on the WIFI chip for the LED,
// so when building for Pico W, CYW43_WL_GPIO_LED_PIN will be defined
#ifdef CYW43_WL_GPIO_LED_PIN
#include "pico/cyw43_arch.h"
#endif

#ifndef DOT
#define DOT 200
#endif

#ifndef DASH
#define DASH 600
#endif

#ifndef BREAK_ELEMENT
#define BREAK_ELEMENT 200
#endif

#ifndef BREAK_LETTER
#define BREAK_LETTER 600
#endif

#ifndef BREAK_WORD
#define BREAK_WORD 1400
#endif

// Perform initialisation
int pico_led_init(void) {
#if defined(PICO_DEFAULT_LED_PIN)
    // A device like Pico that uses a GPIO for the LED will define PICO_DEFAULT_LED_PIN
    // so we can use normal GPIO functionality to turn the led on and off
    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
    return PICO_OK;
#elif defined(CYW43_WL_GPIO_LED_PIN)
    // For Pico W devices we need to initialise the driver etc
    return cyw43_arch_init();
#endif
}

// Turn the led on or off
void pico_set_led(int delay) {
#if defined(PICO_DEFAULT_LED_PIN)
    // Just set the GPIO on or off
    gpio_put(PICO_DEFAULT_LED_PIN, true);
    sleep_ms(delay);
    gpio_put(PICO_DEFAULT_LED_PIN, false);
#elif defined(CYW43_WL_GPIO_LED_PIN)
    // Ask the wifi "driver" to set the GPIO on or off
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, true);
    sleep_ms(delay);
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, false);
#endif
}

int main() {
    int rc = pico_led_init();
    hard_assert(rc == PICO_OK);
    
    sleep_ms(BREAK_WORD);

    while (true) {
        pico_set_led(DOT);
        sleep_ms(BREAK_ELEMENT);
        pico_set_led(DOT);
        sleep_ms(BREAK_ELEMENT);    
        pico_set_led(DOT);
        sleep_ms(BREAK_ELEMENT);
        pico_set_led(DOT);

        sleep_ms(BREAK_LETTER);

        pico_set_led(DOT);

        sleep_ms(BREAK_LETTER);

        pico_set_led(DOT);
        sleep_ms(BREAK_ELEMENT);
        pico_set_led(DASH);
        sleep_ms(BREAK_ELEMENT);
        pico_set_led(DOT);
        sleep_ms(BREAK_ELEMENT);
        pico_set_led(DOT);

        sleep_ms(BREAK_LETTER);

        pico_set_led(DOT);
        sleep_ms(BREAK_ELEMENT);
        pico_set_led(DASH);
        sleep_ms(BREAK_ELEMENT);
        pico_set_led(DASH);
        sleep_ms(BREAK_ELEMENT);
        pico_set_led(DOT);

        sleep_ms(BREAK_WORD);
    }
}
