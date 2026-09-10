# Troubleshooting Guide, Exercise Session 2

## Project generated as C when I wanted C++ (or the other way around)

**Symptom:** You want to check which compiler built your project, but the build
terminal disappears too quickly to read.

### Fix

1. Check the file extension of your source file: `.c` means the C compiler, `.cpp`
   means the C++ compiler. The `add_executable(...)` line in `CMakeLists.txt` shows
   the same thing.
2. For hard proof (optional): open `build/compile_commands.json` and press **Ctrl+F**
   to search for your file name. The very first word of that line is the compiler:

```
...\arm-none-eabi-gcc.exe  ->  C project
...\arm-none-eabi-g++.exe  ->  C++ project
```

(The final `Linking CXX executable` line does not tell you anything: the SDK adds a
small C++ file to every project, so the link step always says `CXX`.)

### Fix (if it is the wrong language)

1. If you just created the project: delete it and create it again. The project
   generator has a **Generate C++ code** checkbox on the **C/C++** screen. Ticked = C++,
   unticked = C.
2. Or convert the existing project manually: rename `c_versions.c` to `c_versions.cpp`
   and update the `add_executable(...)` line in `CMakeLists.txt` accordingly. The
   generated CMakeLists already enables both languages, so no other change is needed.

## `arm-none-eabi-objdump` not found

**Symptom:** Running `arm-none-eabi-objdump` in a terminal gives:

```
'arm-none-eabi-objdump' is not recognized as an internal or external command
```

### Fix

1. You usually do not need it at all: every build automatically writes
   `build/<name>.dis` in your project folder: a full disassembly of the program,
   generated with objdump by the build itself. See
   [Exercise 2: What did the compiler generate?](instructions.md#exercise-2-what-did-the-compiler-generate).
2. If you want the real tool: it is bundled with the toolchain that the extension
   downloaded. Run it with the full path:
   - Windows: `C:\Users\<user>\.pico-sdk\toolchain\<version>\bin\arm-none-eabi-objdump.exe`
   - Linux: `~/.pico-sdk/toolchain/<version>/bin/arm-none-eabi-objdump`

   (`<version>` is a folder like `13_2_1`; look inside `.pico-sdk/toolchain/`.)

## Debug session does not start

**Symptom:** Pressing F5 fails with something like:

```
Error: unable to find a matching CMSIS-DAP device
```

or GDB fails to launch altogether.

### Fix

Check, in order:

1. The **Debug Probe** is connected to the Pico's debug header with all 3 wires
   (see "SES-PHCE - Lecture 1 - Introduction.pdf" slide 35, or the
   [Getting Started guide](https://pip.raspberrypi.com/documents/RP-008276-DS),
   section A.1.1 "Debug Probe wiring").
2. The Pico is powered (its own USB cable is in).
3. The project was created with **Debugger: DebugProbe (CMSIS-DAP)**, the default
   selection in the project generator.
4. If GDB itself is reported missing, check the `gdbPath` in `.vscode/launch.json`.

## Run USB fails after switching to run-from-RAM

**Symptom:** After adding `pico_set_binary_type(temp_blink no_flash)` to
`CMakeLists.txt`, **Run Pico Project (USB)** fails and there is no `.uf2` file in
`build/` anymore.

This is expected: a `no_flash` build does not produce a `.uf2`, because the program is
meant to be loaded into SRAM by the debugger over SWD.

### Fix

- Use the **Debug** session (F5) to load and run the program; that is the whole point
  of the run-from-RAM exercise.
- To go back to normal: remove the `pico_set_binary_type(... no_flash)` line, rebuild,
  and **Run Pico Project (USB)** works again.

## Breakpoints not hit / variables show `<optimized out>` / `not an lvalue` / `in register $rN`

**Symptom:** Any of:
- A breakpoint is set on a line, but execution never stops there (the breakpoint icon
  may be grayed out).
- A variable in the **Watch** pane shows `<optimized out>`.
- `print &var` in the Debug Console fails with `Can't take address of "var" which
  isn't an lvalue` or `... which is in register $rN`.

### Fix

You are debugging an optimized build. This can happen two separate ways, check both:

1. Look at the CMake build variant in the bottom status bar. The project generator can
   leave it on **Release** instead of **Debug**, which builds with `-O3 -DNDEBUG` even
   though `CMakeLists.txt` does not mention it. Switch it to **Debug**. Switching the
   variant alone does not update an already-built project: delete the `build` folder
   (or run CMake Tools' **Clean Reconfigure**) and rebuild.
2. Even on **Debug**, the SDK's default is `-Og`, not `-O0`. `-Og` still allows the
   compiler to keep short-lived local variables in a register instead of giving them a
   stack address, which is exactly what causes the "not an lvalue" / "in register"
   errors. Force `-O0` for the target:
   ```cmake
   target_compile_options(<name> PRIVATE -O0)
   ```
   (or configure the project with `PICO_DEOPTIMIZED_DEBUG=1`)
3. Delete `build/`, rebuild, and debug again. Breakpoints and variable addresses now
   line up with the source.

Exercise 3's final step asks you to compare debugging an optimized build; if you see
`<optimized out>` there, you have found the difference.

## Serial Monitor not outputting

1. Check the Debug Probe wiring: the 3 SWD wires on the debug header **and** the UART
   pair to the Pico's UART0 TX/RX pins (GPIO 0/1). The Serial Monitor listens on the
   probe, so without the UART wires there is no output.
   ([Getting Started guide](https://pip.raspberrypi.com/documents/RP-008276-DS),
   section A.1.1 "Debug Probe wiring").
2. Check that the project was created with **Console over UART** ticked, or that
   `CMakeLists.txt` contains `pico_enable_stdio_uart(<name> 1)`.
3. Check that `stdio_init_all();` is the first line inside `main()`.

## `printf("%.2f", ...)` prints nothing or garbage

**Symptom:** `printf("temp = %.2f C\n", temp_c);` compiles fine, but the Serial Monitor
shows a blank, `?`, or garbage where the number should be.

**Fix:** The SDK's printf (newlib-nano) drops float support by default to save Flash.
Add this to `CMakeLists.txt`, after `target_link_libraries(temp_blink ...)`:

```cmake
target_link_options(temp_blink PRIVATE "-Wl,-u,_printf_float")
```

Rebuild and reflash.

## `Failed to launch GDB: could not connect (error 138): ... joined drive`

**Symptom:** Pressing F5 gives an error like:

```
Failed to launch GDB: could not connect (error 138): The system tried to join a drive
to a directory on a joined drive.
```

The text is nonsense, it is Windows mangling a raw error code. In practice it means GDB
could not reach the OpenOCD server, usually because something from a previous session
is still holding the port.

### Fix

1. Kill leftover processes from a previous debug session (Task Manager > Details > end
   `openocd.exe` and `arm-none-eabi-gdb.exe`), or in a terminal:
   ```powershell
   Get-Process openocd,arm-none-eabi-gdb -ErrorAction SilentlyContinue | Stop-Process -Force
   ```
2. Unplug and replug the Debug Probe.
3. Make sure only one debug session is attached to the probe at a time.
4. If it still fails, restart VS Code.

## `Failed to generated gdb commands: Could not start gdb, no response from gdb`

**Symptom:** GDB itself never starts (different from the connection error above).

### Fix

1. Test GDB directly in a terminal: `arm-none-eabi-gdb --version`. If that fails, GDB
   itself is broken (wrong path, missing file, blocked by antivirus), not the debug
   config.
2. Check `gdbPath` in `.vscode/launch.json` points at the real
   `arm-none-eabi-gdb.exe` inside the toolchain `bin` folder.
3. Check Windows Security > Protection history for a quarantined `gdb.exe`.
4. Kill leftover processes, same as the error above.

## Which debug configuration to pick

If the Run and Debug dropdown offers a choice between **Cortex-Debug** and
**Cortex-Debug with External OpenOCD** (naming varies), pick the plain **Cortex-Debug**
one. It starts and manages OpenOCD for you automatically. "External OpenOCD" expects
you to already have OpenOCD running yourself in a separate terminal, which nothing in
this course does.

## Debug Console: `-exec` prefix and `printf` vs `print`

**Symptom:**
```
-exec print &temp_c
Undefined MI command: exec (from exec print &temp_c)
```
or
```
printf &temp_c
Bad format string, missing '"'.
```

**Fix:** Cortex-Debug's Debug Console sends what you type straight to GDB, it does not
use the `-exec` prefix that Microsoft's `cppdbg` adapter needs. And `printf` is GDB's
own formatted-print command (it wants a quoted format string first), not the way to
print a value. Just use:
```
print &temp_c
```

## `No symbol "temp_c" in current context`

**Symptom:** `print &temp_c` (or any local variable) fails with this message, even
though the variable is declared earlier in the same function.

**Fix:** Usually one of:
- The loaded program is stale, you edited the source after the last build. Rebuild,
  then restart the debug session.
- You paused mid-execution (for example hit Pause instead of letting a breakpoint stop
  you) while control was inside an SDK function like `sleep_ms`, a different stack
  frame that does not have your variable in scope. Either resume with **F5** until your
  own breakpoint is hit, or use `up` in the Debug Console to select the `main()` frame.

## Compile error: hardware/adc.h: No such file or directory

**Symptom:** The `temp_blink` build fails with:

```
fatal error: hardware/adc.h: No such file or directory
```

**Fix:** The ADC code lives in its own SDK library. Add `hardware_adc` to the
`target_link_libraries(temp_blink ...)` block in `CMakeLists.txt` (see Exercise 3,
step 2).

## Other issue?

Send the full error and what you tried to svgot23@student.sdu.dk :)
