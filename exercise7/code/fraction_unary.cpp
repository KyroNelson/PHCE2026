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

    // TODO 1: add operator- with no parameter. It returns the negated fraction
    //         and leaves this one as it is.

    // Prefix ++: adds 1, gives the new value
    Fraction& operator++()
    {
        numerator_ += denominator_;
        return *this;
    }

    // TODO 2: add the prefix --. It takes away 1, like ++ adds it.

    // Postfix ++: adds 1, gives the old value. Uses the prefix ++.
    const Fraction operator++(int)
    {
        const Fraction copy {*this};
        ++(*this);
        return copy;
    }

    // TODO 3: add the postfix --, like the postfix ++.

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

// True if both Fractions are the same number. A non-member: both operands are
// parameters, and it uses the public numerator() and denominator().
bool operator==(const Fraction& left, const Fraction& right)
{
    return left.numerator() == right.numerator() && left.denominator() == right.denominator();
}

// TODO 5: add operator<< for std::ostream and Fraction. It prints like print().

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
        Fraction f {3, 4};
        show("f   = ", f);

        // TODO 4: show -f, then f.

        show("++f = ", ++f);
        show("f   = ", f);
        show("f++ = ", f++);
        show("f   = ", f);

        // TODO 4 (continued): show --f and f--, each followed by f, like ++.

        // After the -- lines, f is 3/4 again
        std::cout << "f == 3/4: " << (f == Fraction {3, 4} ? "yes" : "no") << std::endl;

        // TODO 6: print f, -f and f++ again, in one line, with << only.

        std::cout << std::endl;
        sleep_ms(REPEAT_MS);
    }
}
