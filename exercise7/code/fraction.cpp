#include <cstdint>
#include <iostream>
#include <numeric>

#include "pico/stdlib.h"

constexpr std::uint32_t REPEAT_MS {5000};

// A fraction like 3/4. Always stored reduced, with the sign on top.
// The denominator must not be 0.
class Fraction
{
public:
    Fraction(int numerator, int denominator)
        : numerator_ {numerator}, denominator_ {denominator}
    {
        reduce();
    }

    int numerator() const { return numerator_; }
    int denominator() const { return denominator_; }

    void print() const { std::cout << numerator_ << '/' << denominator_; }

    // Adds other to this Fraction
    Fraction& operator+=(const Fraction& other)
    {
        numerator_ = numerator_ * other.denominator_ + other.numerator_ * denominator_;
        denominator_ *= other.denominator_;
        reduce();
        return *this;
    }

    // TODO 1: add operator+ (adds two Fractions).

    // TODO 2: add operator* with an int on the right (half * 3).

    // TODO 3: add operator== (true if both Fractions are the same number).

private:
    void reduce()  // 2/4 becomes 1/2, 1/-2 becomes -1/2
    {
        const int divisor {std::gcd(numerator_, denominator_)};
        numerator_ /= divisor;
        denominator_ /= divisor;
        if (denominator_ < 0)
        {
            numerator_ = -numerator_;
            denominator_ = -denominator_;
        }
    }

    int numerator_;
    int denominator_;
};

// Subtracts two Fractions. A non-member: both operands are parameters, and it
// uses the public numerator() and denominator().
Fraction operator-(const Fraction& left, const Fraction& right)
{
    const int numerator {left.numerator() * right.denominator()
                         - right.numerator() * left.denominator()};
    return Fraction {numerator, left.denominator() * right.denominator()};
}

// TODO 4: add operator* with an int on the left (3 * half).

void show(const char* label, const Fraction& fraction)
{
    std::cout << label;
    fraction.print();
    std::cout << std::endl;
}

int main()
{
    stdio_init_all();

    while (true)
    {
        const Fraction half {1, 2};
        const Fraction third {1, 3};
        show("half          = ", half);
        show("third         = ", third);

        Fraction sum {0, 1};
        sum += third;
        sum += third;
        sum += third;
        show("sum           = ", sum);
        show("half - third  = ", half - third);

        // TODO 5: show half + third, half * 3 and 3 * half. Print "sum == 1/1: yes"
        //         or "no", using ==.

        std::cout << std::endl;
        sleep_ms(REPEAT_MS);
    }
}
