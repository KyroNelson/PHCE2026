# Troubleshooting Guide, Exercise Session 6

## Serial Monitor shows nothing

1. Check the Debug Probe wires. You need 3 SWD wires. You also need UART on GP0/GP1.
2. Tick **Console over UART** in the new project. Or check `CMakeLists.txt` for
   `pico_enable_stdio_uart(<name> 1)`.
3. `stdio_init_all();` goes first in `main()`.

## `hardware/pwm.h: No such file or directory`

Add `hardware_pwm` to `target_link_libraries` in `CMakeLists.txt`. Same for
`hardware/adc.h` and `hardware_adc`.

## The LED never lights, but `level` changes

TODO 1 is missing. Without `gpio_set_function(..., GPIO_FUNC_PWM)`, the PWM hardware does
not drive the pin.

## The LED barely glows, even with the knob fully right

TODO 2 is missing. Then `wrap` stays at its start value, 65535. A `level` of 255 is then
under 1% duty cycle.

## The LED is fully on most of the way

TODO 3 shifts left (`<<`). Shift right (`>>`). A `level` above `wrap` keeps the pin high
all the time.

## `level` is always 0

TODO 3 is still `{0}`. Replace the `0`, don't add a new line below it.

## `pwm_led`: `narrowing conversion ... from 'int' to 'uint16_t'`

`raw >> ADC_TO_PWM_SHIFT` gives an `int`. Braces `{}` don't allow a silent `int` to
`std::uint16_t` conversion. Cast it: `{static_cast<std::uint16_t>(raw >> ADC_TO_PWM_SHIFT)}`.

## `dimmer_class`: `print_level()` prints nothing

TODO 3 is still empty.

## `dimmer_multi`: `undefined reference to hw::PwmLed::...`

The `.cpp` file is not built. Add `pwm_led.cpp`, `potentiometer.cpp` and
`application.cpp` to `add_executable(...)` (step 2).

## `dimmer_multi`: `pwm_led.h: No such file or directory`

The headers must sit next to `dimmer_multi.cpp`. Copy all files from
`code/dimmer_multi/`.

## `dimmer_multi`: `'PwmLed' was not declared in this scope`

That is on purpose in step 6. Put `hw::` back in front of `PwmLed`.

## `dimmer_multi`: `raw` and `level` are always 0

TODO 2 is still `{0}`. Replace the `0` with a read of `pot_`, then set the level on `led_`.

## draw.io: the PNG is missing a page

**Export as** → **PNG** exports only the page you have open. Switch to the other page at
the bottom and export again.

## `led_class`: `conversion from 'const uint32_t' {aka 'const long unsigned int'} to non-scalar type 'Led' requested`

You wrote `Led led = LED_PIN;`. The constructor is `explicit`, so write
`Led led {LED_PIN};`.
