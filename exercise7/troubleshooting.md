# Troubleshooting Guide, Exercise Session 7

## Serial Monitor shows nothing

1. Check the Debug Probe wires. You need 3 SWD wires. You also need UART on GP0/GP1.
2. Tick **Console over UART** in the new project. Or check `CMakeLists.txt` for
   `pico_enable_stdio_uart(<name> 1)`.
3. `stdio_init_all();` goes first in `main()`.

## The first lines are missing

The Pico prints them before the Serial Monitor is open. Open the Serial Monitor first,
then run. Or press reset on the Pico.

## `include could not find requested file: .../wifi.cmake`

`wifi.cmake` is missing, or not next to `CMakeLists.txt`. Make it like in step 2.

## `wifi.cmake` shows up in `git status`

Add the line `wifi.cmake` to `.gitignore`. If you committed it already, run
`git rm --cached wifi.cmake`, and change the hotspot password.

## `Connecting to WIFI NAME HERE...`

The `include(...)` or `target_compile_definitions(...)` lines from step 2 are missing, or name the wrong target. The target must be
`catfact`, as in `add_executable`. Then run **Clean Reconfigure** in
CMake Tools and build again.

## `Failed to connect`, or it hangs at `Trying to connect`

1. Turn on the hotspot before you run.
2. Set the hotspot to **2.4 GHz** only. Some phones set to "2.4 and 5 GHz" still send only
   5 GHz. On an iPhone, turn on **Maximize Compatibility**.
3. Use **WPA2**, not WPA3 only.
4. Check the name and password. Capitals count.

## `lwipopts.h: No such file or directory`

Copy all files from `code/catfact/` (or `code/webserver/`) next to your `.cpp`. Same for
`mbedtls_config.h`.

## `undefined reference to 'http_client_request_sync(...)'`

1. Add `example_http_client_util.c` to `add_executable(catfact ...)`.
2. Keep the `extern "C" { }` around its `#include`. Without it, step 7 shows this on purpose.

## `undefined reference to 'HTTPServer::...'`

Add `Connection.cpp` to `add_executable(webserver ...)`.

## `Request failed: 3`

The numbers are lwIP's `httpc_result_t`. 3 means the host name was not found. Is there
internet on the hotspot? 2 means no connection. 5 means a timeout. Check the internet on your
phone.

4 means the server closed the connection early. Press the button again.

## `Body:` starts with a number, and ends with `0`

The server sent the body in pieces, with each piece's length in front. It happens now
and then. `extract_fact()` still finds the fact.

## `webserver`: `Could NOT find Python3`

CMake uses Python 3 to turn `content/` into C arrays. Install Python 3, then
**Clean Reconfigure**.

## `http://...local` does not open

1. Your PC must be on the Pico's network, `AP_SSID`.
2. Some phones and PCs don't find `.local` names. Use the IP address from the Serial
   Monitor, `Open in your browser: http://...`.
3. Write `http://`, not `https://`. The Pico has no TLS server.

## Hotspot mode does not start

- `A WiFi called ... is already around`: someone near you has the same `AP_SSID`. Pick
  another name in `wifi.cmake`.
- `static assertion failed`: the message says what is wrong, for example a password under
  8 characters.
- `AP_SSID` not declared, or `Set AP_SSID in wifi.cmake`: `wifi.cmake`, the `include(...)`
  or the `target_compile_definitions(...)` lines from step 2 are missing.
- `dhserver.h: No such file or directory`: the `target_sources` and
  `target_include_directories` lines from step 2 are missing.
- The PC joins, but the page does not open: some PCs leave a network without
  internet by themselves. Join again.

## The page shows `***UNKNOWN TAG temp***`

TODO 1 is missing, or the tag has a typo. The name in `AddSSIHandler` must match the tag
in the page.

## `fraction`: `no match for 'operator*' (operand types are 'int' and 'const Fraction')`

That is on purpose in step 9. Put the non-member `operator*` back.

## `fraction`: `no match for 'operator+' (operand types are 'const Fraction' and 'int')`

That is on purpose in step 10. Remove the `half + 1` line.

## `fraction_unary`: `no 'operator--(int)' declared for postfix '--'`

TODO 3 is missing. The prefix `operator++()` alone does not cover `f++`.

## `fraction_unary`: `no match for 'operator<<' (operand types are 'std::basic_ostream<char>' and 'Fraction')`

TODO 5 is missing, or `operator<<` is inside the class. It must be a non-member.
