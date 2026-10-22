#include "potentiometer.h"

#include <cstdint>

#include "hardware/adc.h"

namespace
{
    constexpr std::uint32_t ADC_FIRST_PIN {26};  // GP26 is ADC input 0, so input = pin - 26
}

hw::Potentiometer::Potentiometer(std::uint32_t pin) : pin_ {pin}, adc_input_ {pin - ADC_FIRST_PIN}
{
    adc_init();
    adc_gpio_init(pin_);
}

std::uint16_t hw::Potentiometer::read() const
{
    // TODO 1: the Pico has one ADC for several pins. Switch it to adc_input_, then read it
    //         and return the value.
    return 0;
}
