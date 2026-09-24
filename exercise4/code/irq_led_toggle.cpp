#include <cstdint>
#include <iostream>

#include "hardware/gpio.h"
#include "hardware/sync.h"
#include "pico/stdlib.h"

constexpr uint LED_PIN {7};     // uint: the SDK's pin type
constexpr uint BUTTON_PIN {10};
// uint32_t: matches the SDK's time functions.
// 32-bit: this CPU's natural size, so no extra work.
constexpr std::uint32_t WORK_MS {500};    // same "busy" main loop as the polling version
constexpr std::uint32_t DEBOUNCE_US {20000};  // lockout debounce: 20 ms, in microseconds

// Internal linkage: only this translation unit (this .cpp and the headers it includes)
// can see these names. static on each one does the same.
namespace
{
    // volatile: the ISR changes these, so never reuse a copy held in a register.
    volatile bool led_on {false};
    volatile unsigned int pending_presses {0};  // counted by the ISR, not yet handled by main()
    volatile std::uint32_t last_press_us {0};
}

// The interrupt service routine (ISR): runs on a rising edge, whatever main() is doing.
// Keep it short: time-critical work only.
void button_isr(uint gpio, std::uint32_t events)
{
    // events is a bit mask, one bit per edge type.
    if (gpio != BUTTON_PIN || (events & GPIO_IRQ_EDGE_RISE) == 0u)
    {
        return;
    }

    const std::uint32_t now_us {time_us_32()};  // microseconds since boot, from the hardware timer

    // Debounce (see Session 3's homework): one press can bounce and make several rising
    // edges within a few ms, and every one of them runs this ISR. Accept the first edge,
    // then ignore any edge that comes less than DEBOUNCE_US after it.
    // Unsigned subtraction wraps, so this stays correct when the timer wraps too.
    if (now_us - last_press_us < DEBOUNCE_US)
    {
        return;
    }
    last_press_us = now_us;

    // TODO 1: flip led_on, write it to LED_PIN, and add 1 to pending_presses.
}

// Hands main() the presses the ISR has counted so far, and starts the count again at 0.
// Read + reset must not be interrupted: a critical section.
unsigned int take_pending_presses()
{
    // TODO 3 (step 5): the ISR can run between the two lines that read and reset
    //                  pending_presses. Turn them into a critical section: call
    //                  save_and_disable_interrupts() before them, keep what it returns,
    //                  and hand that to restore_interrupts() after them.
    const unsigned int presses {pending_presses};
    pending_presses = 0;

    return presses;
}

int main()
{
    stdio_init_all();

    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    gpio_put(LED_PIN, false);

    gpio_init(BUTTON_PIN);
    gpio_set_dir(BUTTON_PIN, GPIO_IN);
    gpio_pull_down(BUTTON_PIN);

    // TODO 2: register button_isr() for a rising edge on BUTTON_PIN with
    //         gpio_set_irq_enabled_with_callback(pin, event mask, enabled, callback).
    //         The event mask for a rising edge is GPIO_IRQ_EDGE_RISE.

    std::cout << "irq_led_toggle started. Press the button." << std::endl;

    unsigned int total_presses {0};

    while (true)
    {
        // Deferred work: the ISR counted the press, the slow printing happens here.
        const unsigned int presses {take_pending_presses()};
        if (presses > 0)
        {
            total_presses += presses;
            std::cout << "Presses this cycle: " << presses << ", total: " << total_presses
                      << std::endl;
        }

        sleep_ms(WORK_MS);  // pretend the main loop is busy with other work
    }
}
