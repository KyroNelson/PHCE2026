# Exercise Session 5, Homework

Do these after Exercises 1-3, in [`instructions.md`](instructions.md). Write answers in the
`_Answer:_` blocks. Commit this file and your `.cpp` files.

---

## Exercise 4: Swap two numbers

**Goal:** Swap two variables inside a function.

Session 4 flipped a `bool` three ways. Now you swap two `int`s. The three ways are the
same.

### Instructions

1. Create a project `swap`. Copy in this program:

   ```cpp
   #include <iostream>

   #include "pico/stdlib.h"

   void swap_by_value(int a, int b)  // gets copies
   {
       const int temp {a};
       a = b;
       b = temp;
   }

   void swap_by_pointer(int* a, int* b)  // gets addresses
   {
       // TODO 1: swap the two ints that a and b point at.
   }

   void swap_by_reference(int& a, int& b)  // gets other names
   {
       // TODO 2: swap a and b.
   }

   int main()
   {
       stdio_init_all();
       sleep_ms(2000);  // time to open the Serial Monitor

       int x {1};
       int y {2};
       std::cout << "start:        x = " << x << ", y = " << y << std::endl;

       swap_by_value(x, y);
       std::cout << "by value:     x = " << x << ", y = " << y << std::endl;

       swap_by_pointer(&x, &y);
       std::cout << "by pointer:   x = " << x << ", y = " << y << std::endl;

       swap_by_reference(x, y);
       std::cout << "by reference: x = " << x << ", y = " << y << std::endl;

       while (true)
       {
           sleep_ms(1000);
       }
   }
   ```
2. Fill in TODO 1. Copy the body of `swap_by_value()`. Put `*` before every `a` and `b`.
3. Fill in TODO 2. Copy the body of `swap_by_value()`. Change nothing.
4. Open the Serial Monitor. Then build and run. Fill in the table.

| Line | `x` | `y` |
|---|---|---|
| start | 1 | 2 |
| by value | | |
| by pointer | | |
| by reference | | |

### Checklist

- [ ] TODO 1 and 2 filled in
- [ ] Table filled in

**Why did `swap_by_value()` not swap?**

> _Answer:_
>

**The last line shows `x = 1` again. Why?**

> _Answer:_
>

**Attached file(s):**

> _Filename:_
>

---

## Exercise 5: A function that calls itself

**Goal:** Write a small recursive function.

A recursive function calls itself. Each call gets a smaller number. The base case says
when to stop.

### Instructions

1. Create a project `recursion`. Copy in this program:

   ```cpp
   #include <iostream>

   #include "pico/stdlib.h"

   // Prints n, n - 1, ... 1, then "Liftoff!". Then each call says when it ends.
   void countdown(int n)
   {
       if (n == 0)  // the base case: stop here
       {
           std::cout << "Liftoff!" << std::endl;
           return;
       }
       std::cout << n << std::endl;
       countdown(n - 1);  // the function calls itself
       std::cout << "back in " << n << std::endl;
   }

   // Adds 1 + 2 + ... + n.
   int sum_to(int n)
   {
       // TODO 1: if n is 0, return 0.
       // TODO 2: else return n + sum_to(n - 1).
       return 0;
   }

   int main()
   {
       stdio_init_all();
       sleep_ms(2000);  // time to open the Serial Monitor

       countdown(5);
       std::cout << "sum_to(10) = " << sum_to(10) << std::endl;

       while (true)
       {
           sleep_ms(1000);
       }
   }
   ```
2. Open the Serial Monitor. Then build and run. Read `countdown()` and its output. Why do
   the `back in` lines count up?
3. Fill in TODO 1 and 2. You should see `sum_to(10) = 55`.
4. Define `factorial(int n)` like `sum_to()`. Use `*` instead of `+`. `factorial(0)` is 1.
5. Print `factorial(5)` in `main()`. You should see 120.
6. Delete the base case in `countdown()`. Build, run and watch. Then put it back and
   re-flash.

### Checklist

- [ ] `sum_to(10) = 55`
- [ ] `factorial(5) = 120`
- [ ] Step 6 tried, then fixed

**What does the base case in `sum_to()` do?**

> _Answer:_
>

**What happened in step 6? Why?**

> _Answer:_
>

**Attached file(s):**

> _Filename:_
>

---

*Read more (optional): Beginning C++17, Chapter 8 ("Pass-by-Value", "Pass-by-Reference",
"Recursion").*
