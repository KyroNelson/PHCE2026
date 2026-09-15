# Exercise Session 3, Homework

Do these after Exercises 1 and 2 (in [`instructions.md`](instructions.md)). Write your
answers directly in this file, in the `_Answer:_` blocks under each question, and commit
it together with any `.cpp` files you are asked to attach, to your own copy of the
repository.

---

## Exercise 3: Detecting button presses and releases

**Goal:** Catch the moment it changes state (pressed or released), print that to the Serial
Monitor, and ignore the rapid flicker a real switch produces on every press ("bounce") -
unfiltered, that flicker would report one physical press as several presses to your code.

### Instructions

1. Create a new Pico project `button_edges` (C++, **Console over UART**), same wiring as
   Exercise 2.
2. Start from this skeleton and fill in the four TODOs. The raw-edge print (right after
   `reading` is read) is already there for you, it's not one of the TODOs: it logs every
   raw level change on the pin, bounce included, so you have something to watch on the
   Serial Monitor before the debounce logic exists at all.

   ```cpp
   #include <cstdio>
   #include "pico/stdlib.h"
   #include "hardware/gpio.h"

   constexpr uint LED_PIN = 7;
   constexpr uint BUTTON_PIN = 10;
   constexpr uint32_t DEBOUNCE_MS = 20;

   int main()
   {
       stdio_init_all();
       gpio_init(LED_PIN);
       gpio_set_dir(LED_PIN, GPIO_OUT);
       gpio_init(BUTTON_PIN);
       gpio_set_dir(BUTTON_PIN, GPIO_IN);
       gpio_pull_down(BUTTON_PIN);

       bool stable_state = false;
       bool last_reading = false;
       bool last_raw = false;
       uint32_t last_change_ms = to_ms_since_boot(get_absolute_time());

       while (true)
       {
           bool reading = gpio_get(BUTTON_PIN);
           uint32_t now_ms = to_ms_since_boot(get_absolute_time());

           // Given: raw, un-debounced edge on the pin, printed as it happens.
           if (reading != last_raw)
           {
               printf("raw   %s at %u ms\n", reading ? "rising (0->1)" : "falling (1->0)", now_ms);
               last_raw = reading;
           }

           // TODO 1: if `reading` differs from `last_reading`, update `last_reading`
           //         and reset `last_change_ms` to `now_ms`.

           // TODO 2: if the reading has been stable for at least DEBOUNCE_MS, and it
           //         differs from `stable_state`, update `stable_state`.

           // TODO 3: whenever `stable_state` just changed, print "pressed" or
           //         "released" to the Serial Monitor.

           // TODO 4: drive the LED to match `stable_state`.

           sleep_ms(1);
       }
   }
   ```
3. Build and run. Press and release the button a few times. For one physical press,
   `pressed`/`released` should appear exactly once each, never more - but the number of
   `raw ...` lines per press can vary. A clean press might produce just one `raw` line
   each way, same as `pressed`/`released`; a bouncier one will show several as the
   contacts chatter before settling. Either is fine - the `raw` lines are your
   visualization of the actual signal on the pin, however many there happen to be;
   `pressed`/`released` is what debouncing turns it into either way.
4. Set `DEBOUNCE_MS` to `0`, rebuild, and try again. Compare what you see.

### Checklist

- [ ] `button_edges` builds and runs
- [ ] All 4 TODOs filled in
- [ ] One press produces exactly one "pressed" and one "released" line (any number of
      `raw` lines)
- [ ] `DEBOUNCE_MS = 0` tried and compared

**How many `raw` lines did one press produce, versus "pressed"/"released" lines? What's
the difference between `reading` and `stable_state` in your program, and why do you need
both?**

> _Answer:_
>

**What happened with `DEBOUNCE_MS = 0`, and why?**

> _Answer:_
>

**Attached file(s):**

> _Filename:_
>

---

## Exercise 4: Mixing C and C++

**Goal:** Call a plain C driver function from a C++ class, and see what actually breaks
if the C/C++ boundary isn't declared correctly.

### Instructions

1. Create a new Pico project `c_interop` (C++, no **Console over UART** needed - this
   one's LED-only), same wiring as Exercise 2.
2. Add a plain C file, `gpio_driver.c`:

   ```c
   #include "hardware/gpio.h"

   void led_set(unsigned pin, int value)
   {
       gpio_put(pin, value);
   }
   ```
   Add it to `add_executable(...)` in `CMakeLists.txt`, next to your `.cpp` file.
3. In your `.cpp` file:

   ```cpp
   #include "hardware/gpio.h"
   #include "pico/stdlib.h"

   constexpr unsigned LED_PIN = 7;

   extern "C" void led_set(unsigned pin, int value);

   class LedController
   {
   public:
       explicit LedController(unsigned pin) : pin_(pin) {}

       void set(bool on) { led_set(pin_, on ? 1 : 0); }

   private:
       unsigned pin_;
   };

   int main()
   {
       gpio_init(LED_PIN);
       gpio_set_dir(LED_PIN, GPIO_OUT);

       LedController led(LED_PIN);
       led.set(true);

       while (true)
       {
           sleep_ms(1000);
       }
   }
   ```
   Build and confirm the LED turns on, now through `LedController` → `led_set()` (C++) →
   `gpio_put()` (C).
4. Remove the `extern "C"` (keep just the plain declaration, still outside the class)
   and rebuild. Read the linker error, then put `extern "C"` back. Note that
   `LedController` itself never changes: the `extern "C"` boundary sits entirely on the
   free declaration of `led_set()`, not inside the class.

### Checklist

- [ ] `c_interop` builds and the LED turns on
- [ ] Linker error seen after removing `extern "C"`, then fixed again

**What did the linker error say when `extern "C"` was removed? What does `extern "C"`
actually tell the compiler to do, and why does `LedController` itself never need to know
about it?**

> _Answer:_
>

**Attached file(s):**

> _Filename:_
>

