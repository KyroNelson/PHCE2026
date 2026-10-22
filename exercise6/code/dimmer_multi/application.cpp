#include "application.h"

#include <cstdint>
#include <iostream>

#include "pico/stdlib.h"

namespace
{
    constexpr std::uint32_t ADC_TO_PWM_SHIFT {4};  // 12-bit pot value to 8-bit LED level
    constexpr std::uint32_t UPDATE_MS {100};
}

app::Application::Application(const hw::Potentiometer& pot, hw::PwmLed& led)
    : pot_ {pot}, led_ {led}
{
}

void app::Application::run()
{
    while (true)
    {
        // TODO 2: read pot_ into raw. Turn raw into a level, like in Exercise 1,
        //         and set it on led_.
        const std::uint16_t raw {0};

        std::cout << "raw " << raw << "  level " << led_.level() << std::endl;

        sleep_ms(UPDATE_MS);
    }
}
