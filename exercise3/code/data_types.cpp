#include <cstdint>
#include <iostream>
#include <limits>

#include "pico/stdlib.h"

// Prints "Size of <label> : <size> byte(s)" - one line of Part 1's output, reused
// wherever that exact pattern (a name and a byte count) comes up again below.
void print_size(const char* label, std::size_t size)
{
    std::cout << "Size of " << label << " : " << size << " byte(s)" << std::endl;
}

int main()
{
    stdio_init_all();

    // Part 1: sizes of the basic data types

    // Given: one example call.
    print_size("char", sizeof(char));

    // TODO: add one print_size() call each for every other row in Part 1's table in
    //       instructions.md: int, short, long, long long, unsigned int, bool, float,
    //       double, long double, char16_t, size_t.

    std::cout << std::endl;  // blank line, just to separate Part 1's output from Part 2's

    // Part 2: a fixed-width type, uint8_t
    // uint8_t is really just an 8-bit unsigned char, so without the static_cast<int>
    // below, cout would print it as a character instead of a number.

    uint8_t age = 25;

    // print_size() only knows "a name and a byte count", so it fits the size line below
    // but not this one - age's value isn't a byte count, it needs its own std::cout line.
    std::cout << "uint8_t value   : " << static_cast<int>(age) << std::endl;
    print_size("uint8_t", sizeof(uint8_t));  // same pattern as Part 1 - still just a size

    // std::numeric_limits<T>::min()/max() give the smallest/largest value T can hold -
    // here, the actual range of a uint8_t, instead of us having to work it out by hand.
    std::cout << "uint8_t minimum : "
              << static_cast<int>(std::numeric_limits<uint8_t>::min()) << std::endl;
    std::cout << "uint8_t maximum : "
              << static_cast<int>(std::numeric_limits<uint8_t>::max()) << std::endl;
    std::cout << std::endl;  // blank line, just to separate Part 2's output from Part 3's

    // Part 3: what happens at the edges of uint8_t's range (0-255, per the min/max above)

    uint8_t number;

    number = 0;
    std::cout << "number = " << static_cast<int>(number) << std::endl;

    number = 255;
    std::cout << "number = " << static_cast<int>(number) << std::endl;

    number = 256;
    std::cout << "number = " << static_cast<int>(number) << std::endl;

    while (true)
    {
        sleep_ms(1000);
    }
}
