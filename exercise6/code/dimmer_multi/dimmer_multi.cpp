#include <cstdint>
#include <iostream>

#include "application.h"
#include "pico/stdlib.h"
#include "potentiometer.h"
#include "pwm_led.h"

constexpr std::uint32_t LED_PIN {7};
constexpr std::uint32_t POT_PIN {26};

int main()
{
    stdio_init_all();

    std::cout << "dimmer_multi started. Turn the knob." << std::endl;

    hw::PwmLed led {LED_PIN};
    hw::Potentiometer pot {POT_PIN};
    app::Application application {pot, led};

    application.run();
}
