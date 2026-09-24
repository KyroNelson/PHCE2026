#include <cstdint>
#include <iostream>
#include <limits>
#include <random>

#include "hardware/gpio.h"
#include "pico/stdlib.h"

constexpr uint LED_PIN {7};     // uint: the SDK's pin type
constexpr uint BUTTON_PIN {10};
// uint32_t: matches the SDK's time functions.
// 32-bit: this CPU's natural size, so no extra work.
constexpr std::uint32_t POLL_MS {50};  // how long main() is "busy" between checks for a press

// State of the current round, shared between button_isr() and main().
// Every field fits the CPU's 32-bit word, so each is read and written in one go:
// no torn reads, so the ISR can't leave one half-written.
struct Round
{
    std::uint32_t start_us;  // when the LED lit up (written by main)
    std::uint32_t stop_us;   // when the button was pressed (written by the ISR)
    bool pressed;            // set by the ISR, cleared by main at the start of every round
};

// Internal linkage: only this translation unit (this .cpp and the headers it includes)
// can see these names. static on each one does the same.
namespace
{
    // volatile: button_isr() changes it behind main()'s back.
    volatile Round round_state {};
}

// The ISR's only job, and the time-critical one: note *when* the press happened.
// Everything else (maths, printing) is deferred to main().
void button_isr(uint gpio, std::uint32_t events)
{
    if (gpio != BUTTON_PIN || (events & GPIO_IRQ_EDGE_RISE) == 0u)
    {
        return;
    }

    // TODO 1: if round_state.pressed is still false, store the current time
    //         (time_us_32()) in round_state.stop_us and set round_state.pressed.
    //         Only the first press of a round counts, so bounce and extra presses
    //         can't overwrite it.
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
    gpio_set_irq_enabled_with_callback(BUTTON_PIN, GPIO_IRQ_EDGE_RISE, true, &button_isr);

    std::cout << "\nReaction time game. Press the button to start." << std::endl;
    while (!round_state.pressed)
    {
        sleep_ms(10);
    }

    // <random>: an engine makes the numbers, a distribution fits them to a range.
    // The microsecond you pressed "start" is unpredictable, so it makes a good seed.
    std::minstd_rand random_engine {round_state.stop_us};
    std::uniform_int_distribution<std::uint32_t> wait_ms {1000, 5000};  // 1-5 s, so you can't count along

    std::uint32_t best_us {std::numeric_limits<std::uint32_t>::max()};  // max = "no best time yet"

    while (true)
    {
        std::cout << "\nNext round in 1 second..." << std::endl;
        sleep_ms(1000);

        round_state.pressed = false;
        std::cout << "Get ready..." << std::endl;
        sleep_ms(wait_ms(random_engine));

        // TODO 2 (step 4): if round_state.pressed is already true here, the player
        //                  pressed before the LED lit. Print "Too early!" and
        //                  `continue` to the next round.

        round_state.start_us = time_us_32();
        gpio_put(LED_PIN, true);

        while (!round_state.pressed)
        {
            sleep_ms(POLL_MS);
        }
        const std::uint32_t noticed_us {time_us_32()};
        gpio_put(LED_PIN, false);

        // Reaction time: when the ISR saw the press, minus when the LED lit.
        // Unsigned subtraction wraps like the timer does, so this stays correct.
        const std::uint32_t reaction_us {round_state.stop_us - round_state.start_us};

        // Keep the fastest round so far.
        if (reaction_us < best_us)
        {
            best_us = reaction_us;
        }

        std::cout << "Your reaction time: " << reaction_us / 1000 << " ms (best: "
                  << best_us / 1000 << " ms)" << std::endl;
        std::cout << "main() noticed the press " << (noticed_us - round_state.stop_us) / 1000
                  << " ms after it happened" << std::endl;
    }
}
