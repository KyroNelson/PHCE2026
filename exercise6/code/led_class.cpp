#include <cstdint>
#include <iostream>

#include "pico/stdlib.h"

constexpr std::uint32_t LED_PIN {7};
constexpr std::uint32_t BLINK_MS {1000};

// TODO 1: define the class Led from the class diagram in homework.md.

int main()
{
    stdio_init_all();

    // TODO 2: create an Led object named led on LED_PIN.

    while (true)
    {
        // TODO 3: toggle the LED. Print "LED is on" or "LED is off", using is_on().
        sleep_ms(BLINK_MS);
    }
}
