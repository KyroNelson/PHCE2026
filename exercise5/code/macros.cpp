#include <cstdint>
#include <iostream>

#include "hardware/adc.h"
#include "hardware/gpio.h"
#include "pico/stdlib.h"

// ---- Switches. The preprocessor reads these first. ----

// 1 = the Pico W's own LED, 0 = the red LED on GP7.
#define USE_BUILTIN_LED 1

// 1 = potentiometer, 2 = light sensor.
#define EXERCISE 1

// Put // in front of this line to turn logging off.
#define DEBUG_MODE

// ---- Code picked by the switches ----

#if USE_BUILTIN_LED
#include "pico/cyw43_arch.h"
#endif

#ifdef DEBUG_MODE
#define DEBUG_LOG(text, value) std::cout << "[DEBUG] " << text << value << std::endl
#else
#define DEBUG_LOG(text, value)
#endif

#if EXERCISE == 1
constexpr uint SENSOR_PIN {26};  // potentiometer
constexpr uint SENSOR_ADC_INPUT {0};
constexpr std::uint16_t THRESHOLD {2000};  // LED on above this
#elif EXERCISE == 2
constexpr uint SENSOR_PIN {27};  // light sensor
constexpr uint SENSOR_ADC_INPUT {1};
constexpr std::uint16_t THRESHOLD {700};  // your number from Exercise 2
#else
#error "EXERCISE must be 1 or 2"
#endif

// ---- Constants ----

// TODO 1: turn these two into constexpr constants.
#define LED_PIN 7
#define LOOP_MS 200

// TODO 2: after step 5, delete this macro.
//         Write a constexpr function double_value(int x) instead.
#define DOUBLE(x) x * 2

// ---- LED ----

void led_init()
{
#if USE_BUILTIN_LED
    cyw43_arch_init();
#else
    // TODO 3: set up LED_PIN as an output.
#endif
}

void led_set(bool on)
{
#if USE_BUILTIN_LED
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, on);
#else
    // TODO 3: turn LED_PIN on or off.
#endif
}

int main()
{
    stdio_init_all();
    sleep_ms(2000);  // time to open the Serial Monitor
    led_init();

    adc_init();
    adc_gpio_init(SENSOR_PIN);
    adc_select_input(SENSOR_ADC_INPUT);

    std::cout << "macros started. EXERCISE " << EXERCISE << std::endl;
    std::cout << "DOUBLE(3 + 4) = " << DOUBLE(3 + 4) << std::endl;

    while (true)
    {
        const std::uint16_t raw {adc_read()};
        DEBUG_LOG("raw = ", raw);

        led_set(raw > THRESHOLD);
        sleep_ms(LOOP_MS);
    }
}
