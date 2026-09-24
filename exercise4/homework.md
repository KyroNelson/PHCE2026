# Exercise Session 4, Homework

Do these after Exercises 1-3 (in [`instructions.md`](instructions.md)). Write your
answers directly in this file, in the `_Answer:_` blocks under each question, and commit
it to your own copy of the repository.

---

## Exercise 4: Raw pointers and smart pointers

**Goal:** Free memory yourself with a raw pointer, then let a smart pointer do it for you.

### Instructions

1. Create a project `smart_pointers` and copy in this program:

   ```cpp
   #include <iostream>
   #include <memory>

   #include "pico/stdlib.h"

   int main()
   {
       stdio_init_all();

       // Part 1: a raw pointer. You asked for the memory with new, so you must give it back.
       int* raw {new int {42}};
       std::cout << "*raw = " << *raw << std::endl;

       // TODO 1: give the memory back with delete, then set raw to nullptr.

       std::cout << "raw is " << (raw ? "still set" : "nullptr") << std::endl;

       // Part 2: a smart pointer. It frees its memory by itself, so there is no delete.
       std::unique_ptr<int> first {std::make_unique<int>(42)};
       std::unique_ptr<int> second {nullptr};

       // TODO 2: move the int from first to second: second = std::move(first);

       std::cout << "first is " << (first ? "full" : "empty")
                 << ", second is " << (second ? "full" : "empty") << std::endl;

       while (true)
       {
           sleep_ms(1000);
       }
   }
   ```
2. Fill in both TODOs. Open the Serial Monitor, then build and run. You got it right if
   you see exactly this:

   ```
   *raw = 42
   raw is nullptr
   first is empty, second is full
   ```
3. Replace your TODO 2 line with `second = first;` and build. Read the error, then put
   your line back.
4. Now crash it on purpose: forget the `delete`. Replace the `while (true)` loop at the
   end with this one, build and run:

   ```cpp
   int leaked_kb {0};
   while (true)
   {
       int* leak {new int[256]};  // 1 KB, and no delete[]
       ++leaked_kb;
       std::cout << "leaked " << leaked_kb << " KB, last one at " << leak << std::endl;
   }
   ```
   It stops with `*** PANIC *** Out of memory`. Your board is fine, just re-flash it.

### Checklist

- [ ] Output matches step 2
- [ ] Error from `second = first;` read
- [ ] Out of memory panic seen

**Part 2 has no `delete`. When is the memory that `second` owns freed?**

> _Answer:_
>

**Why won't `second = first;` compile? What does the "unique" in `unique_ptr` mean?**

> _Answer:_
>

**How many KB leaked before the panic in step 4? The chip has 264 KB of RAM, so why not
264?**

> _Answer:_
>

---

## Exercise 5: Pointers vs. references

**Goal:** Change a variable in `main()` from inside a function. The LED shows whether
it worked.

### Instructions

1. Create a project `references`, with the LED on GPIO 7 as in Exercise 1, and copy in
   this program:

   ```cpp
   #include <iostream>

   #include "hardware/gpio.h"
   #include "pico/stdlib.h"

   constexpr uint LED_PIN {7};

   void toggle_by_value(bool led_on)  // gets a copy
   {
       led_on = !led_on;
   }

   void toggle_by_pointer(bool* led_on)  // gets the address
   {
       // TODO 1: flip the bool that led_on points at.
   }

   void toggle_by_reference(bool& led_on)  // gets another name for the same bool
   {
       // TODO 2: flip the bool.
   }

   int main()
   {
       stdio_init_all();
       gpio_init(LED_PIN);
       gpio_set_dir(LED_PIN, GPIO_OUT);

       bool led_on {false};

       while (true)
       {
           toggle_by_value(led_on);  // the line you change in steps 3 and 4

           gpio_put(LED_PIN, led_on);
           std::cout << "led_on is " << (led_on ? "ON" : "OFF") << std::endl;
           sleep_ms(500);
       }
   }
   ```
2. Build and run it as it is. The LED stays off.
3. Fill in TODO 1 and change the call to `toggle_by_pointer(&led_on);`. The LED blinks.
4. Fill in TODO 2 and change the call to `toggle_by_reference(led_on);`. The LED blinks.

### Checklist

- [ ] By value: LED stays off
- [ ] By pointer: LED blinks
- [ ] By reference: LED blinks

**Why does the LED stay off with `toggle_by_value()`?**

> _Answer:_
>

**A pointer can be `nullptr`, a reference can't. Which of the two would you use for this
function, and why?**

> _Answer:_
>

---

*Read more (optional): Beginning C++17, Chapter 6 ("Dynamic Memory Allocation", "Raw
Pointers and Smart Pointers") and Chapter 8 ("References vs. Pointers").*
