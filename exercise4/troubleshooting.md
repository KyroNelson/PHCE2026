# Troubleshooting Guide, Exercise Session 4

## Serial Monitor not outputting

1. Check the Debug Probe wiring: the 3 SWD wires on the debug header **and** the UART
   pair to the Pico's UART0 TX/RX pins (GPIO 0/1).
2. Check that the project was created with **Console over UART** ticked, or that
   `CMakeLists.txt` contains `pico_enable_stdio_uart(<name> 1)`.
3. Check that `stdio_init_all();` is the first line inside `main()`. This applies to
   `std::cout` just as much as to `printf`.
4. If `std::cout` still prints nothing (a few older SDK/toolchain combinations don't
   wire up `iostream` cleanly, same as in Session 3), swap those lines for `printf()` and
   carry on. Note in your answer that you had to do this.

## `polling_led_toggle` (Exercise 1): nothing happens at all

**Symptom:** No output, and the LED never changes.

**Fix:** Work out which half is broken, in this order.

1. Reset the board with the Serial Monitor already open. If the startup line
   (`polling_led_toggle started.`) appears, serial works and the problem is the button or
   the wiring. If it doesn't, the program never printed to where you're looking: see
   "Serial Monitor not outputting" above.
2. Check each half on its own. For the LED, add `gpio_put(LED_PIN, true);` right after
   `gpio_set_dir()` and rebuild: if it doesn't light, the problem is GPIO 7 or the board,
   not your loop. For the button, print `gpio_get(BUTTON_PIN)` in the loop and hold the
   button down: if it never goes from 0 to 1, the problem is GPIO 10 or the wiring.
3. Check that the project was created for Pico W.

## `irq_led_toggle` (Exercise 1): the ISR never runs

**Symptom:** The LED doesn't react and `Presses this cycle` never appears.

**Fix:**

1. Check TODO 2 is filled in and runs **before** the `while (true)` loop. Code after an
   endless loop never runs.
2. Check the event mask is `GPIO_IRQ_EDGE_RISE`. With the pull-down, a press is a
   LOW → HIGH change, a rising edge. The check at the top of `button_isr()` ignores any
   other kind of event.
3. Check you passed the function itself, `button_isr` or `&button_isr`, and did not
   call it (`button_isr()`).
4. Confirm the wiring with `polling_led_toggle` first. If that doesn't work either, see
   Session 3's troubleshooting guide.

## `irq_led_toggle` (Exercise 1): the program freezes after adding TODO 3

**Fix:** Every `save_and_disable_interrupts()` needs a matching
`restore_interrupts(saved_state)`, with the value the first call returned. If the
restore is missing, or sits after the `return`, interrupts stay off for good: the
button stops working, along with anything else that runs from an interrupt.

## `timer_music` (Exercise 2): the LED doesn't blink

**Fix:**

1. Check TODO 1: `static_cast<Toggler*>(t->user_data)`, and that you use *that* pointer
   in TODO 2, not `LED_PIN` or the global `led`. Otherwise the buzzer timer toggles the
   LED too.
2. Check `toggle_callback()` still ends with `return true;`. With `false` the timer runs
   once and stops.

## `timer_music` (Exercise 2): the melody is silent

**Fix:**

1. Check TODO 3 calls `play_note(note, buzzer)`. `play_note()` takes a reference, so
   no `&` is needed.
2. Check the buzzer module on your board is labeled GP20. If it's connected to a
   different pin, change `BUZZER_PIN`.
3. If the LED blinks and the toggle count for the buzzer goes up, but there's still no
   sound, the buzzer itself is the problem. Ask during the session.

## `timer_music` (Exercise 2): the tune sounds wrong or crackles

**Fix:** Some crackling between notes is normal, since each note starts and stops its
own timer. If the notes are the wrong pitch, check `half_period_us` hasn't been changed:
the pin has to toggle *twice* per period, so it's `1'000'000 / (2 * note.freq_hz)`.

## `reaction_game` (Exercise 3): stuck at "Press the button to start"

**Fix:** TODO 1 isn't filled in yet, or it doesn't set `round_state.pressed = true`.
The start screen waits for exactly that flag.

## `reaction_game` (Exercise 3): a reaction time of about 4294967 ms

**Fix:** That's an early press before TODO 2 is in place: the stop time is earlier than
the start time, and subtracting them in an unsigned type wraps around. This is step 4 of
the exercise, so answer the question about it and then add TODO 2.

## `reaction_game` (Exercise 3): `best` shows about 4293967 ms

**Fix:** You pressed early while TODO 2 was still missing, that round's wrapped time was
lower than "no best time yet" (`best_us` starts at the largest possible `std::uint32_t`),
and it became the best. Reset the board to clear it.

## `smart_pointers` (Exercise 4): no output at all

**Fix:** The program prints only once, right at the start. Open the Serial Monitor
first, then reset the board.

## `smart_pointers` (Exercise 4): `raw is still set`

**Fix:** `delete raw;` frees the memory but doesn't change `raw` itself. You also need
`raw = nullptr;`.

## `smart_pointers` (Exercise 4): `first is full, second is empty`

**Fix:** TODO 2 is missing. It's one line: `second = std::move(first);`.

## `smart_pointers` (Exercise 4): step 4 counts up forever and never panics

**Fix:** Keep `leak` in the `std::cout` line. If the pointer is never used, the compiler
is allowed to skip the `new` entirely, and then nothing leaks.

## `references` (Exercise 5): the LED doesn't blink after step 3 or 4

**Fix:**

1. Check you changed the call in `main()`, not only the function body.
2. In `toggle_by_pointer()`, flip the value it points at: `*led_on = !*led_on;`.
3. If `ON`/`OFF` alternates on the Serial Monitor but the LED stays dark, check the
   wiring with `polling_led_toggle` from Exercise 1.

## Other issue?

Send the full error and what you tried to svgot23@student.sdu.dk :)
