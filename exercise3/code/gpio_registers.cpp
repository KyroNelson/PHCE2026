#include <cstdint>

#include "pico/stdlib.h"

// Three peripherals, three jobs:
//   IO_BANK0    picks which internal signal ("function") drives a pin - GPIO, UART, PWM...
//   PADS_BANK0  sets a pin's electrical behavior - pull-up/down, input buffer, output enable
//   SIO         the actual GPIO register block: reads and writes the pin's 0/1 level
// Base addresses below, from the RP2040 datasheet (SS2.3.1, SS2.19.6.2, SS2.3.1.2).
//
// Named *_ADDR, not IO_BANK0_BASE etc., because those exact names are already macros in
// hardware/regs/addressmap.h (pulled in via pico/stdlib.h) - reusing them makes the
// preprocessor mangle these lines before the compiler even sees them.
constexpr uint32_t IO_BANK0_BASE_ADDR = 0x40014000u;
constexpr uint32_t PADS_BANK0_BASE_ADDR = 0x4001c000u;
constexpr uint32_t SIO_BASE_ADDR = 0xd0000000u;
constexpr uint32_t LED_PIN = 7u;
constexpr uint32_t BUTTON_PIN = 10u;

// The offsets and bit positions below (0x04, 5u, 1u << 7, ...) are raw magic numbers
// on purpose: you are welcome to try and match each one against the datasheet. In real
// code you'd name these too (constexpr/#define)
static inline volatile uint32_t& reg(uint32_t address)
{
    return *reinterpret_cast<volatile uint32_t*>(address);
}

int main()
{
    // LED (GPIO7): SIO function, output, start low.
    // Each pin gets its own 32-bit pad register in PADS_BANK0. Register 0 (offset 0x00)
    // is shared config for the whole bank, not one pin, so pin 0's own register starts
    // at 0x04, and pin N's is at 0x04 + 4*N. Bit 7 of it is OD (Output Disable);
    // clearing that bit lets the pin actually drive a value instead of floating.
    reg(PADS_BANK0_BASE_ADDR + 0x04u + 4u * LED_PIN) &= ~(1u << 7);  // OD = 0

    // Each pin also gets a control register in IO_BANK0 that picks its "function" - which
    // internal peripheral drives it. Each pin's block is 8 bytes (a STATUS register at
    // +0, then this CTRL register at +4), so pin N's CTRL register is at 0x04 + 8*N.
    // Writing 5 there selects FUNCSEL = SIO, "plain software-controlled GPIO".
    reg(IO_BANK0_BASE_ADDR + 0x04u + 8u * LED_PIN) = 5u;              // FUNCSEL = SIO

    reg(SIO_BASE_ADDR + 0x018u) = 1u << LED_PIN;                      // GPIO_OUT_CLR: LED off
    reg(SIO_BASE_ADDR + 0x024u) = 1u << LED_PIN;                      // GPIO_OE_SET: LED_PIN is now an output
    // SET/CLR registers let you flip one pin's bit without touching the others.

    // Button (GPIO10): SIO function, input, pull-down.
    reg(IO_BANK0_BASE_ADDR + 0x04u + 8u * BUTTON_PIN) = 5u;           // FUNCSEL = SIO
    volatile uint32_t& button_pad = reg(PADS_BANK0_BASE_ADDR + 0x04u + 4u * BUTTON_PIN);
    button_pad |= 1u << 6;                                       // IE = 1 (Input Enable): reads see the real pin level
    button_pad &= ~(1u << 3);                                    // PUE = 0 (Pull-Up Enable): off
    button_pad |= 1u << 2;                                       // PDE = 1 (Pull-Down Enable): on

    while (true)
    {
        // GPIO_IN has one bit per pin, telling you its current level. With the
        // pull-down enabled above, an unpressed button reads LOW and a pressed one HIGH.
        bool button_pressed = reg(SIO_BASE_ADDR + 0x004u) & (1u << BUTTON_PIN);

        if (button_pressed)
        {
            reg(SIO_BASE_ADDR + 0x014u) = 1u << LED_PIN;  // GPIO_OUT_SET: LED on
        }
        else
        {
            reg(SIO_BASE_ADDR + 0x018u) = 1u << LED_PIN;  // GPIO_OUT_CLR: LED off
        }

        sleep_ms(10);  // polling interval, doubles as simple debounce
    }
}
