# Exercise Session 1

Work through the exercises in order. Write your answers directly in this file, in the
`_Answer:_` blocks under each question, and commit it together with any `.cpp` files
you are asked to attach, to your own copy of the repository.

---

## Exercise 1: Verify your setup

Confirm that the [`SETUP.md`](../SETUP.md)
instructions have been followed and your Pico development environment is in place.

**Anything that did not work, and how you solved it:**

> _Answer:_
> Everything worked

---

## Exercise 2: Build and run the blinker

Create, build and flash the example **Blink** project to confirm that your development
environment works from source code all the way to running firmware on the Pico W.

### Instructions

1. Open **Visual Studio Code**.
2. Open the **Raspberry Pi Pico** extension from the sidebar.
3. Create a new project and select the **Blink** example.
   - Select **Pico W** as the target board.
   - Choose a folder for the project.
   - Create/open the project in VS Code.
4. Add `stdio_init_all();` as the first step inside main()
5. Add `printf("Test");` inside while loop in main()
6. Build the project.
   - Use the **Compile / Build** command from the Raspberry Pi Pico extension.
   - Watch the terminal output and make sure the build finishes without errors.
7. Connect the Pico W to your computer using USB.
8. Flash the program to the Pico W using the **Run / Flash** command in the
   Raspberry Pi Pico extension or by manually uploading the .uf2 file.
9. Check the board. The LED should now blink continuously.
10. Connect the **Debug Probe** (hardware) and open the **Serial Monitor**, then look for the output.

### Checklist
- [x] Blink project created and opened
- [x] Project builds without errors
- [x] Program flashed to the Pico W
- [x] The LED is blinking
- [x] Serial Monitor output is visible

**If the build or flash failed, what was the error and what fixed it?**

> _Answer:_
> First flash wasn't working due to not having proper USB driver installed,
> following the steps from the troubleshoot guide solved the issue

---

## Exercise 3: Explore PicoBricks

For each hardware module on the PicoBricks board, answer the following
questions:

1. What is the name of this module?
2. What does this module do?
3. Is it an input or output?

### Module 1

| # | Question | Answer |
|---|----------|--------|
| 1 | Name | Raspberry Pi Pico  |
| 2 | Purpose | The core module responsible for </br> communication with all the others|
| 3 | Input or output | General Purpose I/O |


### Module 2

| # | Question | Answer |
|---|----------|--------|
| 1 | Name | OLED Screen |
| 2 | Purpose | Monochrome Display for <br/> compact portable applications |
| 3 | Input or output | Output (I<sup>2</sup>C Interface) |

### Module 3

| # | Question | Answer |
|---|----------|--------|
| 1 | Name | Temperature and Humidity Sensor |
| 2 | Purpose | Measures humidity and temperature <br/> of the environment and outputs the data|
| 3 | Input or output | Digital signal output <br/> (response signal of 40-bit data) |

### Module 4

| # | Question | Answer |
|---|----------|--------|
| 1 | Name | LED Button |
| 2 | Purpose | Send a signal to the control <br/> circuit when pressed |
| 3 | Input or output | Control data signal output/input |

### Module 5

| # | Question | Answer |
|---|----------|--------|
| 1 | Name | RGB Led |
| 2 | Purpose | Emit composite RGB light |
| 3 | Input or output | Output |

### Module 6

| # | Question | Answer |
|---|----------|--------|
| 1 | Name | Motor Driver |
| 2 | Purpose | Controls speed and direction of <br/> an external motor through change <br/> in frequency |
| 3 | Input or output | Output for motor movement direction |

### Module 7

| # | Question | Answer |
|---|----------|--------|
| 1 | Name | LDR Sensor |
| 2 | Purpose | Used to detect the change in <br/> the light intensity (photo-conductive) |
| 3 | Input or output | Input |

### Module 8

| # | Question | Answer |
|---|----------|--------|
| 1 | Name | Power Relay |
| 2 | Purpose | Control other electrical circuits <br/> by opening and closing them|
| 3 | Input or output | Input/Output |

### Module 9

| # | Question | Answer |
|---|----------|--------|
| 1 | Name | Rotary Potentiometer |
| 2 | Purpose | Allows to control the current flow <br/> by manually adjusting its resistance |
| 3 | Input or output | Signal input |

### Module 10

| # | Question | Answer |
|---|----------|--------|
| 1 | Name | IoT Board |
| 2 | Purpose | Provides simple connectivity through <br/> UART interface|
| 3 | Input or output | General Purpose I/O |

### Module 11

| # | Question | Answer |
|---|----------|--------|
| 1 | Name | Buzzer |
| 2 | Purpose | Produces sound at a variable frequency |
| 3 | Input or output | Sound output |

### Module 12

| # | Question | Answer |
|---|----------|--------|
| 1 | Name | Breadboard/Protoboard |
| 2 | Purpose | Used as a construction base for <br/> prototyping various electronic systems |
| 3 | Input or output | n/a (it's just a piece of plastic :P) |

---

## Exercise 4: Morse Code

Make a program that blinks `help!` as Morse
code using the LED abstractions in the blink project (`pico_set_led`). How you represent
and translate Morse code is up to you.

Rules:

- dot = 1 time unit (200ms)
- dash = 3 time units
- gap between dot/dash elements within one letter = 1 unit
- gap between letters = 3 units
- gap between words = 7 units

Use the Morse-code reference below and the C++ book/documentation, Google as needed.

Reference: <https://en.wikipedia.org/wiki/Morse_code>

Simply copy the modified .c file(s) containing your solution to the exercise folder.

**Attached file(s):**

> _Filename:_
> morse.c
