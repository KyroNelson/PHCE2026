# Troubleshooting Guide, Exercise Session 5

## Serial Monitor shows nothing

1. Check the Debug Probe wires. You need 3 SWD wires. You also need UART on GP0/GP1.
2. Tick **Console over UART** in the new project. Or check `CMakeLists.txt` for
   `pico_enable_stdio_uart(<name> 1)`.
3. `stdio_init_all();` goes first in `main()`.
4. The homework prints once, at start. Open the Serial Monitor first. Then reset the board.

## `hardware/adc.h: No such file or directory`

Add `hardware_adc` to `target_link_libraries` in `CMakeLists.txt`.

## `raw` is always 0 or always 4095

1. Check `adc_gpio_init()` gets the right pin. Pot is 26. Light sensor is 27.
2. Check `adc_select_input()` matches the pin. GP26 is input 0. GP27 is input 1.
3. Check the module sits firmly.

## `raw_to_volts()` always prints 0

TODO 1 is still `return 0.0f;`. Replace that line, don't add below it.

## `light_led`: the LED is always on

Check the numbers from step 3. The threshold must sit between them. Covered should read
higher than normal light. If the room got darker since step 3, measure again: in a dim
room the sensor reads high even when uncovered.

## `light_led`: the LED never turns on

Covered doesn't reach `DARK_THRESHOLD + 50`. Cover the sensor fully with a finger, check
the covered number, and set `DARK_THRESHOLD` lower.

## `light_led`: the LED is on when it is bright

Your sensor reads lower when dark. In TODO 1, swap the `<` and `>`, and swap the `+ 50`
and `- 50`. In `macros` (step 9), flip
`raw > THRESHOLD` to `raw < THRESHOLD`.

## `light_led`: `pico/status_led.h` not found

Add `pico_status_led` to `target_link_libraries` (step 1).

## `light_led`: the RGB LED stays dark

1. Check the `target_compile_definitions(...)` line from step 1. Without it, nothing
   lights.
2. Check the line says `light_led`, your project's name.

## `light_led`: the disco stays red

The colour position is declared inside the loop, so it starts at 0 on every pass. Declare
it before `while (true)`.

## `macros`: `pico/cyw43_arch.h: No such file or directory`

Add `pico_cyw43_arch_none` to `target_link_libraries`.

## `macros`: `#error "EXERCISE must be 1 or 2"`

That is on purpose in steps 10 and 11. The `was not declared in this scope` errors after it
come from the same cause: without a valid `EXERCISE`, `SENSOR_PIN` and friends never get
defined. Set `EXERCISE` back to `1` or `2`.

## `recursion`: the board freezes or prints garbage in step 6

That is the point of step 6. Put the base case back and re-flash.

## `recursion`: step 6 counts down forever and never freezes

Check the `back in` line is still after `countdown(n - 1);`. Without it, the compiler turns
the recursion into a plain loop that uses no extra memory per call.
