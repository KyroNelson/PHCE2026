#include <cstdint>
#include <iostream>

#include "hardware/adc.h"
#include "hardware/pwm.h"
#include "pico/stdlib.h"

constexpr std::uint32_t LED_PIN {7};
constexpr std::uint32_t POT_PIN {26};             // the potentiometer; GP26 is ADC input 0
constexpr std::uint32_t POT_ADC_INPUT {0};
// the PWM counter counts 0 to 255, then starts again
constexpr std::uint16_t PWM_WRAP {255};
// like raw >> 3 in Session 5, Exercise 1: drop the 4 lowest bits,
// so 0-4095 (12 bits) becomes 0-255 (8 bits)
constexpr std::uint32_t ADC_TO_PWM_SHIFT {4};
// The pin is high while the counter is below level. The counter stops at PWM_WRAP,
// so PWM_WRAP + 1 keeps the pin high all the time: fully on.
constexpr std::uint16_t FULL_ON_LEVEL {PWM_WRAP + 1};
constexpr std::uint32_t UPDATE_MS {100};

int main()
{
    stdio_init_all();

    adc_init();
    adc_gpio_init(POT_PIN);
    adc_select_input(POT_ADC_INPUT);

    // TODO 1: let the PWM hardware drive LED_PIN, instead of normal on/off output.

    const std::uint32_t slice {pwm_gpio_to_slice_num(LED_PIN)};  // the PWM slice that drives LED_PIN
    // TODO 2: make the counter of slice count from 0 to PWM_WRAP (255),
    //         then start again at 0.
    pwm_set_enabled(slice, true);

    std::cout << "pwm_led started. Turn the knob." << std::endl;

    while (true)
    {
        const std::uint16_t raw {adc_read()};  // 0 to 4095
        // TODO 3: turn raw (0 to 4095) into level (0 to 255). Use ADC_TO_PWM_SHIFT.
        //         The shift gives an int: static_cast it to std::uint16_t.
        std::uint16_t level {0};

        if (level == PWM_WRAP)  // at PWM_WRAP the pin is still low for one count per period
        {
            level = FULL_ON_LEVEL;
        }

        // the pin is high while the counter is below level
        pwm_set_gpio_level(LED_PIN, level);

        std::cout << "raw " << raw << "  level " << level << std::endl;

        sleep_ms(UPDATE_MS);
    }
}
