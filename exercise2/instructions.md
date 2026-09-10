# Exercise Session 2

Work through the exercises in order. Write your answers directly in this file, in the
`_Answer:_` blocks under each question, and commit it together with any `.c` or `.cpp`
files you are asked to attach, to your own copy of the repository.

---

## Exercise 1: Same firmware in C and C++

**Goal:** Write the same small program in C11 and in C++17, and see what the modern
C++ features buy you: `#define` → `constexpr`, `enum` → `enum class`, type safety.

### Instructions

1. Open **Visual Studio Code** and open the **Raspberry Pi Pico** extension from the sidebar.
2. Create a new project:
   - Select **New Pico Project** → **C/C++**.
   - Name it `c_versions`, select **Pico W** as the target board.
   - Tick **Console over UART** (the output then travels through the Debug Probe).
   - **Do NOT tick the wireless option** (`Pico W onboard LED`). Step 4 explains why.
   - Leave **Generate C++ code** unticked.
   - Choose a folder for the project and open it in VS Code.
3. Find the starter file
   [`code/c_versions/main.c`](code/c_versions/main.c) and copy its contents into the
   generated `c_versions.c`, replacing everything that was there.
4. Read the code. The comments in the starter explain each part. Note the C constructs
   you are about to convert: `#define`, `enum`, and `struct blinker` with the free
   function `blink_once()`. The LED is the **PicoBricks LED**, wired straight to
   RP2040 **GPIO 7** and driven with plain `gpio_put()`.

   The Pico W's onboard LED sits behind the WiFi chip and needs a large driver that
   would bloat the firmware and shift every later exercise onto that driver instead of
   your code, so this exercise uses the PicoBricks LED instead.
5. Compile the project and run it. The PicoBricks LED should blink at 250 ms intervals.
6. Create a second project exactly the same way (same board, **Console over UART**),
   but name it `cpp_versions` and tick **Generate C++ code** this time.
7. Rewrite the program in C++17 using the mapping table below. When you get to the LED
   lines, try this first: inside `blink_once()`, pass the new `LedState` value straight
   to `gpio_put()` and compile.

   ```cpp
   gpio_put(b->pin, LedState::On);   // and the same for LedState::Off
   ```

   It will **not compile**. Read the error message, then read the explanation below. It
   tells you what the compiler is trying to protect you from, and how the fix works.
   Keep the overall structure the same in both versions: `struct blinker` with the free
   function `blink_once()` stays as it is. We will turn it into a proper object later in
   the course.

   **What just happened, and what does the fix mean?**

   `gpio_put()` expects a `bool`. In C, an `enum` is just an integer, so `LED_ON`
   converts to `true` automatically. `enum class` refuses that conversion on purpose:
   a `LedState` is its own type, not a number, so swapped or nonsense values get caught
   when you compile instead of showing up as a bug on the board later.

   Fix it with an explicit cast. It costs nothing at runtime and vanishes from the
   machine code you'll look at in Exercise 2:

   ```cpp
   gpio_put(b->pin, static_cast<bool>(LedState::On));
   gpio_put(b->pin, static_cast<bool>(LedState::Off));
   ```

   The `: bool` in `enum class LedState : bool { Off = false, On = true };` is what
   makes `On` and `Off` real booleans, so the cast has something to hand over.

   Read more: Beginning C++17, Chapter 2 (casts) and Chapter 3 (enumerations).
8. Build and run `cpp_versions`. The behavior must match the C version.
9. Compare the two builds:
   - In the Explorer, note the source file names: `c_versions.c` ends in `.c`,
     `cpp_versions.cpp` ends in `.cpp`. The extension is what tells the build system
     which compiler to use: `.c` files go to the C compiler (`gcc`), `.cpp` files to
     the C++ compiler (`g++`).
   - Open both `CMakeLists.txt` files and find the lines that set the language
     standards (`CMAKE_C_STANDARD 11` in one, `CMAKE_CXX_STANDARD 17` in the other).
10. **Meet the cross-compiler.** Open `C:\Users\<user>\.pico-sdk\toolchain\<version>\bin\`
    (Linux: `~/.pico-sdk/toolchain/<version>/bin/`) and find `arm-none-eabi-gcc` and
    `arm-none-eabi-g++`. The name says it all: `arm` is the target CPU, `none` means no
    operating system, `eabi` is the embedded calling convention. This folder also holds
    `gdb`, `objdump`, `size` and `c++filt`, used in later exercises.
11. **Measure properly:** in a terminal inside VS Code (the extension adds the
    toolchain folder to the PATH of its own terminals), run `arm-none-eabi-size` on
    both firmware files. If the command is not found, step 10 shows where the tools
    live:

    ```
    arm-none-eabi-size build/c_versions.elf
    arm-none-eabi-size build/cpp_versions.elf
    ```

    Fill in the table below. `text` is the code in Flash, `data` is initialized variables
    (Flash copy + RAM), `bss` is zero-initialized variables (RAM only). The two rows
    should be identical or nearly identical: any *difference* between them is the cost
    of the language choice.

### C11 → C++17 mapping

| C11 | C++17 |
|---|---|
| `#define LED_PIN 7` | `constexpr uint led_pin = 7;` |
| `#define BLINK_DELAY_MS 250` | `constexpr uint blink_delay_ms = 250;` |
| `enum led_state { LED_OFF = 0, LED_ON = 1 };` | `enum class LedState : bool { Off = false, On = true };` |
| `enum` values are integers and convert to anything silently | `enum class` is its own type and refuses. Why, and how to fix it, is explained in step 7 |

### Results

| Version | `text` | `data` | `bss` | Total Flash (`text`+`data`) | Total RAM (`data`+`bss`) |
|---|---|---|---|---|---|
| `c_versions` (C11) | | | | | |
| `cpp_versions` (C++17) | | | | | |

### Checklist

- [ ] `c_versions` project created (C, **Console over UART**, no wireless option) and builds
- [ ] `c_versions` runs and the PicoBricks LED blinks
- [ ] `cpp_versions` project created with the same options and **Generate C++ code** ticked
- [ ] `gpio_put(..., LedState::On)` tried without a cast, and the compile error read
- [ ] The program is rewritten in C++17 (with the cast) and behaves identically
- [ ] The `CMAKE_C_STANDARD` / `CMAKE_CXX_STANDARD` lines found in both `CMakeLists.txt`
- [ ] `arm-none-eabi-gcc` and `arm-none-eabi-g++` found in the toolchain folder
- [ ] Sizes table above filled in with `arm-none-eabi-size`

**The compiler runs on your Windows/Linux PC. What CPU architecture will the produced
instructions run on? What does the `arm-none-eabi` part of the compiler's name tell you?
Why can the `.elf` file produced on your laptop not simply be executed by Windows?**

> _Answer:_
>

**Why did `enum class` require an explicit conversion where the C `enum` did not? What
went wrong when you passed `LedState::On` to `gpio_put()` without a cast?**

> _Answer:_
>

**Compare the `text`, `data` and `bss` sizes of your C and C++ versions. Did the C++
abstractions used here introduce a measurable runtime or memory cost?**

> _Answer:_
>

**Attached file(s):**

> _Filename:_
>

---

## Exercise 2: What did the compiler generate?

**Goal:** See what the compiler actually does with your code: how the `-O` optimization
levels trade size against speed, and what happens to your functions in the machine code.

### Instructions

1. Open the `cpp_versions` project. In `CMakeLists.txt`, add the following line after
   `pico_add_extra_outputs(...)`:
   ```cmake
   target_compile_options(cpp_versions PRIVATE -O0)
   ```
2. Compile. Then run `arm-none-eabi-size build/cpp_versions.elf` and fill in a row of
   the table below. The size command only reads the compiled file on your PC. The board
   does not need to be connected for this part.
3. Flash the program (**Run Pico Project (USB)**, or **F5** over SWD) and check that it
   still blinks correctly. Then repeat steps 2 and 3 for `-O2` and `-Os` (edit the one
   line, recompile, flash).

   `-O0` = no optimization, `-O2` = a normal release build, `-Os` = optimized for
   *size* instead of speed. (Two more flags exist but you won't build them here: `-Og`,
   debug-friendly optimization, used later in Exercise 3; and `-O3`, aggressive speed
   optimization that can make firmware bigger. Real-Time C++ §6.1 covers all of them.)
4. **The disappearing delay loop.** Add this silly function and call it in
   `blink_once()` between the LED on and off calls:
   ```cpp
   static void waste_time() {
       for (uint32_t i = 0; i < 1000000; ++i) {
           // do nothing
       }
   }
   ```
   Build with `-O0`: the blinker visibly slows down. Build with `-O2`: the delay is
   gone. What happened?
5. **Disassembly.** Every build automatically writes
   `build/cpp_versions.dis`, a full disassembly of your program. Open it and press
   **Ctrl+F** to search for `main` and `blink_once`. Copy the file aside (e.g.
   `cpp_versions_O0.dis`) after the `-O0` build, and again as `cpp_versions_O2.dis`
   after the `-O2` build. In VS Code, right-click one file → **Select for Compare**,
   then right-click the other → **Compare with Selected**.
   - Can you still find `blink_once()` as a separate function at `-O2`?
   - Find one source statement whose generated assembly changed significantly between
     `-O0` and `-O2`. What did the compiler do differently?
   - **Bonus:** the C++ symbol for `blink_once` is mangled (e.g.
     `_Z10blink_onceP7blinker`). Run `arm-none-eabi-c++filt _Z10blink_onceP7blinker` and
     see what comes out. This is what makes C++ overloading work (Real-Time C++ §6.6).

### Results

| Flag | `text` | `data` | `bss` | Total Flash (`text`+`data`) | Total RAM (`data`+`bss`) |
|---|---|---|---|---|---|
| `-O0` | | | | | |
| `-O2` | | | | | |
| `-Os` | | | | | |

### Checklist

- [ ] Project builds with `-O0`, `-O2` and `-Os`, table above filled in
- [ ] `waste_time()` added, and the difference between `-O0` and `-O2` observed
- [ ] `blink_once` found in the `.dis` file (and its fate at `-O2` determined)
- [ ] One changed source statement identified and explained
- [ ] (bonus) `c++filt` demangled the symbol

**Why is the size of the `.elf` file on disk not the same thing as the amount of Flash
used by the program?**

> _Answer:_
>

**Why was the compiler allowed to remove the entire `waste_time()` loop? Why is a
software delay loop therefore a bad way to create timing in an embedded program, and
why does `sleep_ms()` not disappear in the same way?**

> _Answer:_
>

**What happened to `blink_once()` between `-O0` and `-O2`?**

> _Answer:_
>

**Why can optimized firmware contain fewer instructions even though the C++ source code
is exactly the same?**

> _Answer:_
>

**What is `-Os` optimizing for, compared to `-O2`?**

> _Answer:_
>

---

## Exercise 3: Debug a temperature-controlled blinker

**Goal:** Write a small program yourself: a blinker whose speed follows the RP2040's
internal temperature. Then debug it with the Debug Probe: breakpoints, watches, memory,
Flash vs. RAM, and what optimization does to your debugging experience.

### Instructions

1. In the **Raspberry Pi Pico** extension: **New Pico Project** → **C/C++**.
   - Name: `temp_blink`, board: **Pico W**.
   - Tick **Generate C++ code** and **Console over UART**.
2. Replace the contents of the generated `temp_blink.cpp` with the skeleton below and
   fill in the four TODOs yourself. They are all things you did in Exercise 1. The
   temperature sensor parts are provided (ADC is a later session):

   ```cpp
   #include <stdio.h>
   #include "pico/stdlib.h"
   #include "hardware/adc.h"
   #include "hardware/gpio.h"

   int main() {
       // TODO 1: initialize stdio so printf reaches the Serial Monitor

       // TODO 2: initialize the PicoBricks LED (GPIO 7) as an output

       printf("BUILD: FLASH\n");  // Provided: flip to RAM in the Flash vs. RAM step

       // Temperature sensor initialization is provided. ADC itself is a later session.
       adc_init();
       adc_set_temp_sensor_enabled(true);
       adc_select_input(4);

       while (true) {
           // Provided: read the sensor and convert to a temperature.
           uint16_t raw = adc_read();                                    // 12-bit
           const float voltage = raw * 3.3f / (1 << 12);                 // full scale 3.3 V
           const float temp_c = 27.0f - (voltage - 0.706f) / 0.001721f;  // RP2040 datasheet

           // Provided: blink delay from the temperature (hotter = faster).
           int delay_ms = static_cast<int>(1000 - 50 * (temp_c - 20));
           if (delay_ms < 100) delay_ms = 100;
           if (delay_ms > 950) delay_ms = 950;

           sleep_ms(500);  // Provided: keep the Serial Monitor readable

           // TODO 3: print the temperature and the raw value

           // TODO 4: blink the LED (on, wait, off, wait)
       }
       return 0;
   }
   ```

   The skeleton uses `hardware/adc.h`, which lives in its own SDK library. A fresh
   project only links `pico_stdlib`, added by the generator in its own
   `target_link_libraries(temp_blink ...)` block. Below it, the generator leaves a second,
   empty `target_link_libraries(temp_blink ...)` block commented "Add any user requested
   libraries". Open `CMakeLists.txt` and add `hardware_adc` there, so it looks like this:

   ```cmake
   # Add any user requested libraries
   target_link_libraries(temp_blink
           hardware_adc
   )
   ```

3. Compile and run with **Run Pico Project (USB)**. The **Serial Monitor** (over the
   Debug Probe) shows a new `temp = ... C` line roughly every 2 seconds and the
   **PicoBricks LED** blinks. Put a finger on the black chip. The temperature rises and
   the blinking speeds up. Note the values of `raw` and `temp_c` before and after
   warming it. You will need them for the questions below.
4. Now debug it. Keep the Pico's own USB cable connected for power and make sure the
   **Debug Probe** is connected with all its wires: the 3 SWD wires on the debug header plus the
   UART pair. The Serial Monitor output travels through the probe.

   **Check the build type first.** In the bottom status bar, find the CMake build
   variant selector and make sure it says **Debug**, not Release/MinSizeRel/RelWithDebInfo.
   The project generator can leave this on Release, which builds with `-O3 -DNDEBUG` even
   though nothing in `CMakeLists.txt` mentions it. A Release build makes later steps fail
   in confusing ways (e.g. `Can't take address of "temp_c" which isn't an lvalue.`, because
   the compiler kept it in a register instead of on the stack). If it was on Release,
   switching the variant alone is not enough: delete the `build` folder (or run CMake
   Tools' **Clean Reconfigure**), then rebuild.

   Even on **Debug**, the SDK builds with `-Og`, not `-O0`. `-Og` still allows the
   compiler to keep short-lived locals in a register instead of on the stack, which
   causes the exact same "not an lvalue" / "which is in register $rN" error in step 7.
   Force `-O0` for this target to guarantee every local gets a real stack address. Add
   this line to `CMakeLists.txt`, after `add_executable(temp_blink temp_blink.cpp)`:
   ```cmake
   target_compile_options(temp_blink PRIVATE -O0)
   ```
   Delete `build/` and rebuild again after adding it.

   In the left sidebar, open the **Run and Debug** view, make sure the **Cortex Debug**
   configuration is selected, and press **F5**. The build starts, the program is loaded
   over SWD, and execution stops at the first line of `main`. The yellow arrow shows
   where you are.
5. **Breakpoints:** click in the gutter to the left of the line numbers, next to the
   `printf` line. A red dot appears. Press **F5** (Continue): the program runs until it
   hits the breakpoint and stops on that line.
6. **Watch:** right-click `raw` and choose **Add to Watch**, then do the same for
   `temp_c`. The WATCH panel appears in the **Run and Debug** sidebar. Step over with
   **F10** and watch the values change each loop iteration.
7. **Dump memory**: in the **Debug Console**, run:
   ```
   print &temp_c
   ```
   Copy the address into the **MEMORY** pane (in the CORTEX-DEBUG view) and set the
   length to 8 bytes. Decode the value: `float` is stored in IEEE-754 little-endian, e.g.
   25.0 °C = `00 00 C8 41` bytes.
8. **Flash vs. RAM:**
   1. With the breakpoint hit, look at the instruction address in the call stack or
      disassembly view: it lies in the `0x1000xxxx` region, code executing from Flash (XIP).
      Write down the address of `main()`. For comparison, the address of a variable
      (e.g. `temp_c` from step 7) is in the `0x2000xxxx` region: SRAM.
   2. Change `BUILD: FLASH` to `BUILD: RAM`, then add `pico_set_binary_type(temp_blink
      no_flash)` to `CMakeLists.txt` (e.g. right after `pico_add_extra_outputs(...)`).
      Rebuild and start debugging again: the program now loads into SRAM (write down the
      new address of `main()`), and the Serial Monitor should print `BUILD: RAM`. A
      `no_flash` build produces no `.uf2` file, so **Run Pico Project (USB)** no longer
      works; use the Debug session instead.
   3. Unplug the board and plug it back in. Which line does the Serial Monitor print now,
      `FLASH` or `RAM`? Is the program still there? Why (not)?
9. **The `-O2` experiment.** Remove the `no_flash` line, add
   `target_compile_options(temp_blink PRIVATE -O2)` to `CMakeLists.txt` (like in
   Exercise 2), rebuild, and start the debugger again. Can you still step through every
   source line and inspect every local variable exactly as before? Note at least one
   concrete difference (e.g. a variable showing `<optimized out>`, or a breakpoint
   landing on a different line).

### Checklist

- [ ] `temp_blink` project created (C++, **Console over UART**)
- [ ] All 4 TODOs filled in by hand. Program runs: temperature printed, LED blinks, finger test works
- [ ] Debug session starts and stops at `main`
- [ ] Breakpoint on the `printf` line hit; `raw` and `temp_c` watched while stepping
- [ ] `&temp_c` dumped and decoded in the MEMORY pane (or skipped if behind schedule)
- [ ] Flash run: address of `main()` in `0x1000xxxx` recorded
- [ ] RAM run (`no_flash`): address of `main()` in `0x2000xxxx` recorded, `BUILD:` printf added, power-cycle behavior explained
- [ ] `-O2` build debugged: at least one concrete difference noted

**What were the values of `raw` and `temp_c` before and after warming the RP2040 with
your finger?**

> _Answer:_
>

**What address did you observe for `main()` when executing from Flash, and what address
when executing from SRAM? What do the `0x1000....` and `0x2000....` address ranges tell
you?**

> _Answer:_
>

**Power-cycle the board after the `no_flash` build. Which `BUILD:` line did the Serial
Monitor print, and why? Why does a RAM-loaded program disappear while the Flash version
remains? (And why would you ever want to run from RAM?)**

> _Answer:_
>

**What difference did you notice when debugging the optimized (`-O2`) build compared to
the unoptimized one?**

> _Answer:_
>

**Attached file(s):**

> _Filename:_
>