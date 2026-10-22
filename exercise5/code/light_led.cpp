#include <array>
#include <cstdint>
#include <iostream>

#include "hardware/adc.h"
#include "hardware/gpio.h"
#include "pico/status_led.h"
#include "pico/stdlib.h"

constexpr uint LED_PIN {7};
constexpr uint LIGHT_PIN {27};        // GP27 is ADC input 1
constexpr uint LIGHT_ADC_INPUT {1};
constexpr std::uint16_t DARK_THRESHOLD {700};  // above this is dark
constexpr std::uint16_t OFFSET {50};           // gap on each side of the threshold

// Colours for the RGB LED on GP6.
constexpr std::array<std::uint32_t, 4> COLORS {
    PICO_COLORED_STATUS_LED_COLOR_FROM_RGB(40, 0, 0),   // red
    PICO_COLORED_STATUS_LED_COLOR_FROM_RGB(0, 40, 0),   // green
    PICO_COLORED_STATUS_LED_COLOR_FROM_RGB(0, 0, 40),   // blue
    PICO_COLORED_STATUS_LED_COLOR_FROM_RGB(40, 0, 40),  // purple
};

int main()
{
    stdio_init_all();

    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    gpio_put(LED_PIN, false);

    status_led_init();  // the RGB LED on GP6

    adc_init();
    adc_gpio_init(LIGHT_PIN);
    adc_select_input(LIGHT_ADC_INPUT);

    std::cout << "light_led started. Cover the sensor." << std::endl;

    bool is_dark {false};  // true: the LEDs are on

    while (true)
    {
        const std::uint16_t raw {adc_read()};  // higher when darker

        // TODO 1: set is_dark with two thresholds, using if and else if.
        //         Above DARK_THRESHOLD + OFFSET: is_dark becomes true.
        //         Below DARK_THRESHOLD - OFFSET: is_dark becomes false.

        gpio_put(LED_PIN, is_dark);

        // TODO 2: the RGB LED. If is_dark, turn it on in blue:
        //           colored_status_led_set_on_with_color(COLORS[2]);
        //         Else, turn it off:
        //           colored_status_led_set_state(false);

        // TODO 3: disco. Change TODO 2 so every pass shows the next colour in COLORS.
        //         After purple, start again at red.

        std::cout << "raw " << raw << "  LED " << (is_dark ? "ON" : "OFF") << std::endl;

        sleep_ms(200);
    }
}
