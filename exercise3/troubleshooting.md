# Troubleshooting Guide, Exercise Session 3

## Serial Monitor not outputting

1. Check the Debug Probe wiring: the 3 SWD wires on the debug header **and** the UART
   pair to the Pico's UART0 TX/RX pins (GPIO 0/1).
2. Check that the project was created with **Console over UART** ticked, or that
   `CMakeLists.txt` contains `pico_enable_stdio_uart(<name> 1)`.
3. Check that `stdio_init_all();` is the first line inside `main()`. This applies to
   `std::cout` in Exercise 1 just as much as to `printf`.
4. If `std::cout` in `data_types` still prints nothing (a few older SDK/toolchain
   combinations don't wire up `iostream` cleanly), swap those lines for
   `printf("...\n", ...)` and carry on. Note in your answer that you had to do this.

## `gpio_registers`/`gpio_hal` builds fine, but the LED never lights (or is stuck on)

**Symptom:** No compile or link error, but the button/LED behavior doesn't happen, or
doesn't match `gpio_sdk`.

**Fix:**

1. First confirm the wiring and board are fine by running `gpio_sdk`. If that also
   fails, it's a hardware/wiring problem, see Session 1 and 2's troubleshooting guides.
2. If `gpio_sdk` works but `gpio_registers` or `gpio_hal` does not, you likely have a
   wrong address, struct field, or bit index somewhere. This won't throw an error, it
   just silently touches the wrong register. Recheck each line against the comment next
   to it, and against the other two implementations.
3. Double-check `LED_PIN`/`BUTTON_PIN` match the SDK version (7 and 10) in all three
   projects.

## `gpio_hal` doesn't build: unknown struct or field name

**Fix:** `hardware/structs/sio.h`, `io_bank0.h` and `padsbank0.h` come from the Pico SDK,
not the SDK's public `hardware/gpio.h` API, so autocomplete may not suggest them. Make
sure all three `#include`s from [`code/gpio_hal.cpp`](code/gpio_hal.cpp) are present, and
that field names are spelled exactly as in that file (`sio_hw->gpio_set`, not
`sio_hw->gpio_out_set`; `pads_bank0_hw`, not `padsbank0_hw`, though on recent SDK
versions both names work).

## `button_edges` (Exercise 3) prints nothing at all, not even `raw` lines

**Fix:** The `raw` print is given code, not a TODO, so if you see nothing, it's a wiring
or `stdio_init_all()` problem (see "Serial Monitor not outputting" above), not a logic
bug. Confirm `gpio_sdk` from Exercise 2 still works on the same wiring first.

## `button_edges` (Exercise 3) only shows one `raw` line per press, not several

**Fix:** Nothing to fix - this is expected and fine. The PicoBricks button isn't
especially prone to visible bounce - most presses give you just one `raw` line each way,
exactly matching `pressed`/`released`. It does happen, though: confirmed on this exact
hardware, occasionally a press or release shows 2-3 `raw` lines in a row (e.g. falling,
then rising again a few ms later, then falling again) before settling. Either way,
`pressed`/`released` should still appear exactly once per press - that's what to check,
not the `raw` count. If you want to try for visible bounce, a faster or sloppier press
helps, or set `DEBOUNCE_MS = 0` per step 4 - that removes the filter entirely, so any
bounce that does occur shows up directly as extra `pressed`/`released` lines instead.

## `button_edges` (Exercise 3) prints several "pressed"/"released" lines per tap

**Fix:**

1. Check `last_change_ms` resets on *every* raw change (TODO 1), not only once the
   debounce window has already elapsed.
2. Check you're comparing elapsed time (`now_ms - last_change_ms >= DEBOUNCE_MS`), not
   `DEBOUNCE_MS` directly.
3. Some buttons genuinely bounce longer than others. If bumping `DEBOUNCE_MS` to `50`
   fixes it, that's a fine finding to note in your answer.

## Linker error after removing `extern "C"` (Exercise 4)

**Symptom:**
```
undefined reference to `led_set(unsigned int, int)'
```

**Fix:** This is expected, it's the point of the exercise. Without `extern "C"`, the
C++ compiler looks for a C++-mangled version of `led_set` that doesn't exist, because
`gpio_driver.c` was compiled as plain C, which never mangles names. Restore:
```cpp
extern "C" void led_set(unsigned pin, int value);
```
This declaration stays outside `LedController`; only the free function `led_set()`
needs it, since that's the only symbol whose name is actually decided by C vs. C++
compilation. `LedController::set()` itself is always compiled as ordinary C++.

## `c_interop` (Exercise 4): `undefined reference to led_set` even with `extern "C"` in place

**Fix:** `gpio_driver.c` was never compiled into the program. Check it's listed in
`add_executable(...)` in `CMakeLists.txt`, next to your `.cpp` file:
```cmake
add_executable(c_interop
    main.cpp
    gpio_driver.c
)
```

## `c_interop` (Exercise 4): builds fine, but the LED never lights

**Fix:** Check `gpio_init(LED_PIN)`/`gpio_set_dir(LED_PIN, GPIO_OUT)` are called before
`LedController` is used - `LedController::set()` only wraps `gpio_put()`, it doesn't
configure the pin itself. Without that setup, `gpio_put()` has no defined effect on the
pin, and this won't show up as a build or link error - only as the LED not responding.

## Other issue?

Send the full error and what you tried to svgot23@student.sdu.dk :)