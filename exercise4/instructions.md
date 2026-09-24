# Exercise Session 4

Work through the exercises in order. Write your answers directly in this file, in the
`_Answer:_` blocks under each question, and commit it together with any `.cpp`
files you are asked to attach, to your own copy of the repository.

Exercises 1-3 (below) are done during the exercise session. Exercises 4
and 5 are homework - see [`homework.md`](homework.md).

Create every
project with **New Pico Project** → **C/C++**, board **Pico W**, **Generate C++ code**
and **Console over UART** ticked.

---

## Exercise 1: Polling vs. interrupts

**Goal:** Find out what a button press costs you when the program only looks at the pin
now and then (polling), and how an interrupt fixes that by running your code the moment
the pin changes. Then protect the data the interrupt shares with `main()`.

Every program so far has used polling: `main()` reads the pin with `gpio_get()`, does
something, sleeps, and reads it again. It only knows what the pin looks like at the
moments it checks. If the main loop is busy with something else for 500 ms, a press
that starts and ends inside those 500 ms never happened as far as the program can tell.

An **interrupt** turns that around. You tell the hardware "when GPIO 10 goes from LOW to
HIGH, stop whatever you're doing and call this function". That function
is the **interrupt service routine (ISR)**. The CPU pauses `main()` wherever it is, runs
the ISR, and then carries on in `main()` from exactly where it was. `main()` doesn't have
to check anything.


### Instructions

1. Create a project `polling_led_toggle` and copy in the program below. It toggles the LED
   on every press and counts the presses, but the main loop spends `WORK_MS` "busy"
   between each check of the button. Recall,
   `button && !last_button` is the same idea as `reading != last_raw` from Session 3's
   homework.

   ```cpp
   #include <cstdint>
   #include <iostream>

   #include "hardware/gpio.h"
   #include "pico/stdlib.h"

   constexpr uint LED_PIN {7};
   constexpr uint BUTTON_PIN {10};
   constexpr std::uint32_t WORK_MS {500};  // pretend the main loop is busy with other work

   int main()
   {
       stdio_init_all();

       gpio_init(LED_PIN);
       gpio_set_dir(LED_PIN, GPIO_OUT);
       gpio_put(LED_PIN, false);

       gpio_init(BUTTON_PIN);
       gpio_set_dir(BUTTON_PIN, GPIO_IN);
       gpio_pull_down(BUTTON_PIN);

       bool led_on {false};
       bool last_button {false};
       unsigned int total_presses {0};

       std::cout << "polling_led_toggle started. Press the button." << std::endl;

       while (true)
       {
           const bool button {gpio_get(BUTTON_PIN)};

           if (button && !last_button)
           {
               led_on = !led_on;
               gpio_put(LED_PIN, led_on);
               ++total_presses;
               std::cout << "Presses counted: " << total_presses << std::endl;
           }
           last_button = button;

           sleep_ms(WORK_MS);
       }
   }
   ```
2. Build, open the **Serial Monitor**, then flash (or reset after starting **Serial Monitor**).
   The startup line must appear before anything else makes sense: no line means the serial
   path is the problem, not your program (see [`troubleshooting.md`](troubleshooting.md)).
   Then press the button 10 times, about once a second, and fill in the first row of the
   table below. Most of those presses will not be counted. Finish by holding one press down
   for a full second and watch what changes.
3. Create a second project `irq_led_toggle` and copy in
   [`code/irq_led_toggle.cpp`](code/irq_led_toggle.cpp). The main loop is just as busy
   (`WORK_MS` is still 500), but the LED toggling and the counting have moved into
   `button_isr()`. Fill in TODO 1 (the ISR body) and TODO 2 (registering the ISR). Leave
   TODO 3 for later.

   `button_isr()` has the signature the SDK expects for a GPIO interrupt callback:
   `void (uint gpio, uint32_t events)`. `gpio` tells you which pin fired, `events` which
   kind of change it was (rising edge, falling edge, ...). The ISR checks both first,
   so it only reacts to what it was written for. The debounce from Session 3's homework
   is already in the ISR: read its comment.
4. Build and run, and repeat the 10-press test from step 2. `main()` still only looks every
   500 ms, so several presses can land in one "cycle": use the `total` for the table.
5. Now look at `take_pending_presses()`. It reads `pending_presses` and then sets it to
   0. Those are two separate steps for the CPU, and the ISR can run between them.
   Programming Embedded Systems (Sect. 8.3.1) walks through exactly this bug: if a press
   lands in that gap, the ISR's `++pending_presses` is overwritten with 0 and the press
   is lost. Such a piece of code has to run in one go, without interruption: a
   **critical section**. The book's fix is to save the current interrupt state, disable
   interrupts, do the work, and restore the saved state. The Pico SDK has functions for
   exactly that. Fill in TODO 3 with them and check that the program still works.

   You won't be able to make the bug happen by hand, since the gap is only a few CPU
   instructions wide. That's what makes race conditions hard to find: they show up
   rarely, and never while you're debugging.

| Program | Presses made | Presses counted | LED reacted right away? |
|---|---|---|---|
| `polling_led_toggle` | 10 | | |
| `irq_led_toggle` | 10 | | |

### Checklist

- [ ] `polling_led_toggle` builds and runs; 10-press test recorded
- [ ] `irq_led_toggle` TODO 1 and 2 filled in; 10-press test recorded
- [ ] TODO 3 filled in; the program still counts correctly

**Why did `polling_led_toggle` miss presses? How long does a press have to last to be sure the
polling version sees it?**

> _Answer:_
>

**In `irq_led_toggle`, the LED reacts at once, but the printed count still shows up late.
Why?**

> _Answer:_
>

**Why is printing kept out of `button_isr()`? What could go wrong if the ISR took, say,
100 ms?**

> _Answer:_
>

**`pending_presses` is declared `volatile`. What could the compiler do with the loop in
`main()` if it wasn't?**

> _Answer:_
>

**Describe, step by step, how a press gets lost in `take_pending_presses()` without TODO
3. Why does the book restore the *saved* interrupt state at the end, instead of simply
switching interrupts back on?**

> _Answer:_
>

**Give one example, outside this exercise, where polling is the better choice, and say
why.**

> _Answer:_
>

**Give one example where an interrupt is the only workable choice, and polling would be
unsafe or useless.**

> _Answer:_
>

**Attached file(s):**

> _Filename:_
>

*Read more (optional): Programming Embedded Systems, Chapter 8 (Sect. 8.3, "Interrupt
Service Routine", and 8.3.1, "Shared Data and Race Conditions"), and Real-Time C++,
Sect. 9.2 ("Programming Interrupts in C++") and 6.15 ("Minimize the Interrupt Frame").
*

---

## Exercise 2: Hardware timers and the buzzer

**Goal:** Blink the LED from a hardware timer instead of `sleep_ms()`, then use the same
timer mechanism to play a melody on the buzzer. Along the way, pass a struct to the
timer through a pointer.

The RP2040 has a hardware timer that counts microseconds from the moment the chip starts
(`time_us_32()` in Exercise 1 reads it). You can set an **alarm** on it: "when the count
reaches X, raise an interrupt". The SDK builds a **repeating timer** on top of that:
`add_repeating_timer_ms()` calls your callback every N milliseconds, from an interrupt,
while `main()` gets on with something else. That freedom is Programming Embedded Systems'
argument for timers over delay loops.

**Pitch is frequency.** How high a note sounds is just how many times per second the air
vibrates. That count is the frequency, measured in hertz: more vibrations per second, a
higher note. 440 vibrations a second is 440 Hz, which musicians call A4.

Hear it before you build it: open the [online tone
generator](https://www.szynalski.com/tone-generator/), set 440 Hz, switch the waveform to
**square** (the shape the Pico makes), then double it to 880 Hz. Same sound, one octave up.

**The buzzer vibrates because you flip the pin.** It is a passive buzzer, a tiny speaker
with no electronics inside: it pushes out on HIGH and pulls back on LOW. One push and pull
is one vibration, which takes two flips. So A4 needs 880 flips a second, one every 1136 µs.
Keep it up for longer and you get a longer note, not a different one.

`struct Note` is exactly those two numbers: `freq_hz` and `ms`. You write them, and
`play_note()` does the rest. It turns the frequency into a gap between flips
(`half_period_us`), runs a timer for `ms` milliseconds, then stops it. You never work out
a microsecond value yourself.

![The pin flips between HIGH and LOW. For the note A4 the gap between flips is 1136
microseconds, so a push and a pull together take 2272 microseconds and 440 of them fit in
a second.](img/square-wave.svg)

### Instructions

1. Create a project `timer_music` and copy in [`code/timer_music.cpp`](code/timer_music.cpp).
2. Read `main()` and `toggle_callback()`. `led` is a `Toggler` struct, and
   `add_repeating_timer_ms()` gets a pointer to it (`&led`). The SDK hands that pointer
   back to your callback as `t->user_data`, typed `void*`, so you cast it back to the
   right type before using it. Then `->` reaches the struct's fields: `toggler->pin`.
   Fill in TODO 1 and TODO 2 in `toggle_callback()`. Beginning C++17 suggests `auto*` for
   a variable that holds a pointer, so it's obvious from the declaration:
   `auto* toggler {static_cast<Toggler*>(t->user_data)};`
3. Build and run. The LED should blink, even though the main loop only prints and
   sleeps. The printed toggle count should grow by 4 every second (250 ms per toggle).
4. Read `play_note()`. It starts a second repeating timer, this time on the buzzer pin,
   with the **same** `toggle_callback()` and a pointer to a different struct, `buzzer`.
   Note that `play_note()` takes the `Toggler` as a reference, and only turns it into a
   pointer (`&speaker`) at the point where the C API needs one. Fill in TODO 3 in `main()`,
   build and run. You should hear the melody while the LED keeps blinking.

### Checklist

- [ ] TODO 1 and 2 filled in; LED blinks from the timer
- [ ] TODO 3 filled in; melody plays while the LED blinks

**The LED and the buzzer use the same `toggle_callback()`. How does it know which pin to
toggle each time it's called?**

> _Answer:_
>

**Why does `add_repeating_timer_ms()` take a `void*` and not a `Toggler*`? What would
happen if you gave it a pointer to something that isn't a `Toggler`?**

> _Answer:_
>

**The button in Exercise 1 interrupts the CPU when a pin changes. Nothing changes on a
pin here, so what triggers the interrupt instead, and what is doing the triggering?**

> _Answer:_
>

**For the note E4 (330 Hz), what is `half_period_us`, and how many times per second does
`toggle_callback()` run?**

> _Answer:_
>

**Attached file(s):**

> _Filename:_
>

*Read more (optional): Programming Embedded Systems, Sect. 8.4 ("The Improved Blinking
LED Program"), Real-Time C++, Sect. 9.3 ("Implementing a System-Tick"), and Beginning
C++17, Chapter 6 (pointers, the `->` operator) and Chapter 5 (`std::array` and the
range-based `for` loop).*

*Active vs. passive buzzers, with pictures of what is inside one:
<https://www.makerguides.com/active-and-passive-piezo-buzzers-with-arduino/>*

---

## Exercise 3: Tommy's Reaction time game

**Goal:** Put interrupts and the hardware timer together in a small game, and see why the
time-critical part of a measurement belongs in the ISR.

The game waits a random 1-5 seconds, lights the LED, and measures how long it takes you
to press the button. The ISR does exactly one thing: it notes the time of the press. The
arithmetic and the printing happen afterwards in `main()`, where it doesn't matter if
they take a while.

`main()` is deliberately lazy. It checks whether the button has been pressed only every
`POLL_MS` milliseconds. The program prints how late `main()` noticed each press, so you
can see what it would have cost to take the stop time there instead.

This game is based on Tommy Nielsen's reaction time game from last year's course.

### Instructions

1. Create a project `reaction_game` and copy in
   [`code/reaction_game.cpp`](code/reaction_game.cpp). Read `struct Round` and how
   `main()` uses `round_state.start_us` and `round_state.pressed`.
2. Fill in TODO 1 (the ISR). Read how `main()` then works out the reaction time and the
   best time from what the ISR stored.
3. Build and run. Press the button to start, then play a few rounds. Wait for the LED!
4. Now press the button **before** the LED lights up, on purpose. Note what is printed.
   Then fill in TODO 2, rebuild, and check that an early press prints `Too early!`.
5. Set `POLL_MS` to `500`, rebuild, and play a few more rounds. Compare the reaction
   times and the "main() noticed the press ... after it happened" lines with the ones at
   `POLL_MS = 50`.
6. Set `POLL_MS` back to `50`. Each of you plays 10 rounds, notes your best time in the
   table, and submits it, with any feedback on the exercises so far, in the
   [highscore and feedback form](https://forms.cloud.microsoft/e/Ry5VEYCUz0). The fastest
   time in the room wins.

| Player | Best of 10 (ms) |
|---|---|
| | |
| | |

### Checklist

- [ ] TODO 1 filled in; the game runs
- [ ] Early press tried before and after TODO 2
- [ ] `POLL_MS = 500` tried and compared
- [ ] Best times recorded and submitted to the form

**What was printed for an early press before you added TODO 2? Why that number? (Hint:
Session 3, `number = 256`.)**

> _Answer:_
>

**Did your reaction times change when `POLL_MS` went from 50 to 500? Did the "noticed"
lag change? If the stop time were taken in `main()` right after the
`while (!round_state.pressed)` loop instead of in the ISR, how much error could that add
at `POLL_MS = 50`, and at `500`?**

> _Answer:_
>

**Exercise 1 needed a critical section for `pending_presses`, but `main()` reads
`round_state` here without one. What is different? (Look at who writes each field, and
when.)**

> _Answer:_
>

**Name one thing that still makes the measured time differ from your real reaction time,
even with the ISR.**

> _Answer:_
>

**Attached file(s):**

> _Filename:_
>

*Read more (optional): Real-Time C++, Sect. 3.14 ("atomic_load() and atomic_store()")
and 6.10 ("Use Native Integer Types").*

Exercises 4 and 5 continue in [`homework.md`](homework.md).
