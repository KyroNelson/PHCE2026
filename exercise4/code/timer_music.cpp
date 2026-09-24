#include <array>
#include <cstdint>
#include <iostream>

#include "hardware/gpio.h"
#include "pico/stdlib.h"

constexpr uint LED_PIN {7};     // uint: the SDK's pin type
constexpr uint BUZZER_PIN {20};
constexpr std::int32_t BLINK_MS {250};  // signed: the minus sign below means something
constexpr std::uint32_t NOTE_GAP_MS {30};  // short silence between notes, so repeated notes don't blur

// Everything the timer callback needs to know about one pin it toggles.
struct Toggler
{
    uint pin;
    bool level;
    volatile unsigned int toggles;  // changed by the timer interrupt, read by main()
};

// Internal linkage: only this translation unit (this .cpp and the headers it includes)
// can see these names. static on each one does the same.
namespace
{
    // The timers keep a pointer to these for as long as they run, so they have to exist
    // for the whole program, not just while some function is running.
    Toggler led {LED_PIN, false, 0};
    Toggler buzzer {BUZZER_PIN, false, 0};
}

// Called from the timer interrupt every time the timer fires. The SDK passes back
// the same pointer we gave add_repeating_timer_*() as user_data, in t->user_data.
bool toggle_callback(repeating_timer_t* t)
{
    // TODO 1: t->user_data is a void*. Turn it back into a Toggler* with static_cast.

    // TODO 2: through that pointer: flip toggler->level, write it to the pin,
    //         and add 1 to toggler->toggles.
    //         (toggler->level is short for (*toggler).level.)

    return true;  // true = keep repeating, false = stop this timer
}

// A note is a frequency and a length. freq_hz = 0 is a rest (silence).
struct Note
{
    std::uint32_t freq_hz;
    std::uint32_t ms;
};

// Note frequencies in Hz, rounded (4th octave, C4 is "middle C").
constexpr std::uint32_t C4 {262};
constexpr std::uint32_t D4 {294};
constexpr std::uint32_t E4 {330};
constexpr std::uint32_t F4 {349};
constexpr std::uint32_t G4 {392};
constexpr std::uint32_t REST {0};

constexpr std::uint32_t Q {300};  // a quarter note, in ms

// std::array: fixed size, known at compile time.
// The start of Beethoven's "Ode to Joy".
constexpr std::array<Note, 16> MELODY {{
    {E4, Q}, {E4, Q}, {F4, Q}, {G4, Q},
    {G4, Q}, {F4, Q}, {E4, Q}, {D4, Q},
    {C4, Q}, {C4, Q}, {D4, Q}, {E4, Q},
    {E4, Q * 3 / 2}, {D4, Q / 2}, {D4, Q * 2},
    {REST, Q * 2},
}};

// Plays one note: a timer toggles the speaker's pin fast enough to make a tone, this
// function waits for the note's length, then stops the timer again.
// speaker is passed by reference: there is no "no speaker" case to allow for. The C SDK
// takes pointers only, so it gets &speaker.
void play_note(const Note& note, Toggler& speaker)
{
    if (note.freq_hz == 0)
    {
        sleep_ms(note.ms);
        return;
    }

    // One period of the tone is one HIGH half and one LOW half, so the pin has to
    // toggle twice per period.
    const std::int64_t half_period_us {1'000'000 / (2 * note.freq_hz)};

    repeating_timer_t timer {};
    add_repeating_timer_us(-half_period_us, toggle_callback, &speaker, &timer);
    sleep_ms(note.ms);
    cancel_repeating_timer(&timer);

    gpio_put(speaker.pin, false);
    sleep_ms(NOTE_GAP_MS);
}

int main()
{
    stdio_init_all();

    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    gpio_init(BUZZER_PIN);
    gpio_set_dir(BUZZER_PIN, GPIO_OUT);

    // Negative delay: "every 250 ms, counted from when the previous call started",
    // so the time the callback itself takes doesn't add up and slow the blinking down.
    repeating_timer_t led_timer {};
    add_repeating_timer_ms(-BLINK_MS, toggle_callback, &led, &led_timer);

    std::cout << "timer_music started." << std::endl;

    while (true)
    {
        // TODO 3 (step 5): play every note in MELODY, in order, with play_note().
        //                  A range-based for loop fits: for (const Note& note : MELODY)

        std::cout << "LED toggled " << led.toggles << " times, buzzer toggled "
                  << buzzer.toggles << " times" << std::endl;
        sleep_ms(1000);
    }
}
