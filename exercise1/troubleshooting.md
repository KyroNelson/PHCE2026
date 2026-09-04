# Troubleshooting Guide

## Build fails: picotool blocked by your organization's Device Guard policy

**Symptom:** The project compiles, but the build fails at the final linking step with:

```
FAILED: [code=4551] blink.elf blink.dis blink.hex blink.bin blink.uf2 ...
'C:\Users\<user>\.pico-sdk\picotool\2.3.0\picotool\picotool.exe' was blocked by your organization's Device Guard policy.
Contact your support person for more info.
```

This is Windows **Smart App Control** blocking the
picotool executable that the Raspberry Pi Pico extension downloaded.

### Fix: disable Smart App Control

1. Open **Windows Security**.
2. Go to **App & browser control**.
3. Click **Smart App Control settings**.
4. Set it to **Off** and confirm.

**Note:** If the setting is grayed out or says it is managed by an organization, contact your support person (e.g. SDU IT).

## Serial Monitor not outputting

See [Exercise 2: Build and run the blinker](instructions.md#exercise-2-build-and-run-the-blinker)
for the full checklist.

1. Check the Debug Probe wiring: see beggining of the video "Pico, Software Setup and Exercise 2.mp4" or "SES-PHCE - Lecture 1 - Introduction.pdf" slide 35.
   ([Getting Started guide](https://pip.raspberrypi.com/documents/RP-008276-DS),
   section A.1.1 "Debug Probe wiring").
2. Check that `stdio_init_all();` is the first line inside `main()` - without it the
   Serial Monitor shows nothing. This was an accidentally omitted step in the first Exercise sessions instructions.md

## Run / Flash fails: picotool cannot connect to the board

**Symptom:** Running (flashing) from the Pico extension fails with:

```
No accessible RP-series devices in BOOTSEL mode were found.
RP2040 device at bus 1, address 5 appears to be in BOOTSEL mode, but picotool was unable to connect.
You may need to install a driver via Zadig.
See "Getting started with Raspberry Pi Pico" for more information
```

Windows does not ship a driver for the Pico's BOOTSEL USB interface, so picotool
cannot talk to the board until one is installed.

### Fix: install the WinUSB driver with Zadig

1. Download [Zadig](https://zadig.akeo.ie/) and run it.
2. Click **Install WICD Driver**

Full instructions: [Getting Started guide](https://pip.raspberrypi.com/documents/RP-008276-DS),
section B.2 "Using picotool". See also the
[Raspberry Pi forum thread](https://forums.raspberrypi.com/viewtopic.php?t=382309)
describing this exact error.

## Linux: read the setup in the online Getting Started guide

On Linux, installing the VS Code extension is not enough — read through the setup
chapter in the online [Getting Started guide](https://pip.raspberrypi.com/documents/RP-008276-DS)
(PDF), section 3.1.2 "Linux". Two things in particular:

1. **USB permissions** — install the udev rules so picotool (loading) and OpenOCD
   (debugging) can access the Pico without `sudo`. Without them, flashing and debugging
   fail with permission errors.
2. **picotool** — even though the extension is already installed, picotool may still
   have to be downloaded separately (Appendix B "Picotool"). Download the pre-built
   binary or build it from source — see the
   [picotool repository](https://github.com/raspberrypi/picotool).

## Other issue?

Send the full error and what you tried to svgot23@student.sdu.dk :)
