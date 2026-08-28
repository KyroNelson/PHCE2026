# Setup of the toolchain for the RPi Pico

Please complete the following setup before attending the first exercise session.

## Hardware

Before the exercise, borrow a PicoBricks kit containing a Raspberry Pi Pico W from the SDU Library. The kit should consist of:

1. PicoBricks Protoboard
2. Raspberry Pi Pico W mounted on top of (1)
3. Servo Motor SG90-HV
4. Debug Probe + wires to attach to the Pico
5. 2x USB cable (Type A to Micro), one for powering the Pico and one for connecting to the Debug Probe

Check that all items are present, and that the kit is not visibly damaged. You are responsible for the kit you borrow, and be there any damage when handing them back, you will have to pay for it. To each lecture you are expected to bring the complete kit and your laptop. The use of the kit itself will be covered during first exercise, you are only asked to borrow and install the necessary software.

There are not enough kits for one per student, so work in groups of two with one kit per group. Each group should preferably include at least one Linux or Windows laptop. While ARM-based macOS is technically supported by the toolchain, it tends to run into problems throughout the course, which is not tailored for it. If there are no kits left at the library, find a friend who did manage to get their hands on one.

## Software

The following software is required:

1. Git
2. Visual Studio Code (you may use alternative IDEs such as CLion, but it may require some different setup to work with the Raspberry Pi Pico) with disabled telemetry
3. A VS Code profile with the Raspberry Pi Pico extension (and its dependencies), Serial Monitor extension and C/C++ extension pack.
4. USB Driver

### Windows

On Windows, follow the steps below.

#### Install Git

Download and install Git from [git-scm.com](https://git-scm.com/downloads). The default options are fine.

#### Install Visual Studio Code

Download the current desktop version from [code.visualstudio.com](https://code.visualstudio.com) and complete the normal installation. Start VS Code once after installation.

#### Disable Visual Studio Code Telemetry

Open the Visual Studio Code settings using Ctrl+,. Search for `Telemetry: Telemetry Level` and set it to `Off`. Next, search for `Workbench: Enable Experiments` and disable it. These settings control Visual Studio Code itself. Extensions may have separate privacy settings, so only install the extensions required for the course.

#### Create a Course Profile

To avoid conflicts with extensions used for other courses, create a separate Visual Studio Code profile. Open File → Preferences → Profiles, select New Profile, choose Empty Profile, and name it `SDU PHCE`. Make sure this profile is active before installing the Raspberry Pi Pico extension.

#### Install the Raspberry Pi Pico Extension

1. Open Extensions with Ctrl+Shift+X.
2. Search for `Raspberry Pi Pico` and install the extension published by Raspberry Pi: `raspberry-pi.raspberry-pi-pico`.
3. Accept the extension's prompts to download the Pico SDK, toolchain, CMake and Ninja.
4. Wait for the installation to finish before closing VS Code. The first download may take 5-10 minutes.
5. Install the C/C++ Extension Pack and Serial Monitor extensions.

Do not install similarly named MicroPython extensions. This course uses the official C/C++ SDK.

#### picotool Driver

On Windows, picotool may require the WinUSB driver for the RP2040 BOOTSEL interface. If you get the error `RP2040 device at bus X, address X appears to be in BOOTSEL mode, but picotool was unable to connect.`, or any other trouble, install the driver as described in the Getting Started with Raspberry Pi Pico-series Guide (see [References](#references)), or ask for help during the exercise session.

#### Verify the Extension

1. Restart VS Code with the `SDU PHCE` profile active.
2. Open the Command Palette with Ctrl+Shift+P.
3. Type `Raspberry Pi Pico`. Confirm that Pico commands appear.
4. Open the Pico extension panel and confirm that an SDK and toolchain version are available.

You do not need to connect or run anything before the extension, but the boards come with preloaded test software, so you can power it on and play with buttons. DO NOT disassemble the board, as it will then require wires to connect back together and you will have to pay for a replacement.

### Linux

Most Linux distributions come preconfigured with all of the dependencies needed to run the extension. Use the Windows instructions above, and check the Getting Started guide linked in [References](#references) as needed.

## Ready-for-class checklist

- PicoBricks kit borrowed from the SDU Library
- VS Code installed
- `SDU PHCE` profile created and active
- VS Code telemetry set to off
- Official Raspberry Pi Pico extension installed successfully
- Pico SDK and toolchain download completed
- No Claude, Codex or any other AI addons added to this profile!!!

## References

[Raspberry Pi Pico Documentation](https://www.raspberrypi.com/documentation/microcontrollers/pico-series.html#pico1)

[Getting Started with Raspberry Pi Pico-series Guide](https://pip.raspberrypi.com/documents/RP-008276-DS)
