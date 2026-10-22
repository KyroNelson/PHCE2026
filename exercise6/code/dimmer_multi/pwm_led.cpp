#include "pwm_led.h"

#include <cstdint>

#include "hardware/pwm.h"
#include "pico/stdlib.h"

hw::PwmLed::PwmLed(std::uint32_t pin) : pin_ {pin}, level_ {0}
{
    gpio_set_function(pin_, GPIO_FUNC_PWM);
    const std::uint32_t slice {pwm_gpio_to_slice_num(pin_)};
    pwm_set_wrap(slice, MAX_LEVEL);
    pwm_set_gpio_level(pin_, level_);
    pwm_set_enabled(slice, true);
}

hw::PwmLed::~PwmLed()
{
    pwm_set_gpio_level(pin_, 0);
}

void hw::PwmLed::set_level(std::uint16_t level)
{
    level_ = level;

    std::uint16_t pin_level {level_};
    // at MAX_LEVEL the pin is still low for one count per period
    if (pin_level == MAX_LEVEL)
    {
        pin_level = MAX_LEVEL + 1;
    }
    pwm_set_gpio_level(pin_, pin_level);
}

std::uint16_t hw::PwmLed::level() const
{
    return level_;
}
