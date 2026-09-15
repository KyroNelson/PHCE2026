#include <cstdint>

#include "hardware/structs/io_bank0.h"
#include "hardware/structs/padsbank0.h"
#include "hardware/structs/sio.h"
#include "pico/stdlib.h"

constexpr uint32_t LED_PIN = 7u;
constexpr uint32_t BUTTON_PIN = 10u;

int main()
{
    // Same three peripherals as gpio_registers.cpp, reached by name instead of a
    // hand-computed address:
    //   pads_bank0_hw->io[N]     pin N's pad register (electrical config: pull, output enable)
    //   io_bank0_hw->io[N].ctrl  pin N's control register (which peripheral function drives it)
    //   sio_hw->...              SIO's registers, one field per register (the actual 0/1 level)
    // The *_BITS constants are just named versions of the bit masks gpio_registers.cpp
    // writes by hand (e.g. ..._OD_BITS is bit 7, Output Disable).

    // LED (GPIO7): SIO function, output, start low.
    pads_bank0_hw->io[LED_PIN] &= ~PADS_BANK0_GPIO0_OD_BITS;   // OD = 0: pin allowed to drive
    io_bank0_hw->io[LED_PIN].ctrl = GPIO_FUNC_SIO;             // FUNCSEL = SIO
    sio_hw->gpio_clr = 1u << LED_PIN;                          // same as GPIO_OUT_CLR: LED off
    sio_hw->gpio_oe_set = 1u << LED_PIN;                       // same as GPIO_OE_SET: LED_PIN is now an output

    // Button (GPIO10): SIO function, input, pull-down.
    io_bank0_hw->io[BUTTON_PIN].ctrl = GPIO_FUNC_SIO;          // FUNCSEL = SIO
    pads_bank0_hw->io[BUTTON_PIN] |= PADS_BANK0_GPIO0_IE_BITS;   // IE = 1 (Input Enable): reads see the real pin level
    pads_bank0_hw->io[BUTTON_PIN] &= ~PADS_BANK0_GPIO0_PUE_BITS; // PUE = 0 (Pull-Up Enable): off
    pads_bank0_hw->io[BUTTON_PIN] |= PADS_BANK0_GPIO0_PDE_BITS;  // PDE = 1 (Pull-Down Enable): on

    while (true)
    {
        // With the pull-down enabled above, an unpressed button reads LOW and a pressed one HIGH.
        bool button_pressed = sio_hw->gpio_in & (1u << BUTTON_PIN);

        if (button_pressed)
        {
            sio_hw->gpio_set = 1u << LED_PIN;  // same as GPIO_OUT_SET: LED on
        }
        else
        {
            sio_hw->gpio_clr = 1u << LED_PIN;  // same as GPIO_OUT_CLR: LED off
        }

        sleep_ms(10);  // polling interval, doubles as simple debounce
    }
}
