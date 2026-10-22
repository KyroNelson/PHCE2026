#ifndef POTENTIOMETER_H
#define POTENTIOMETER_H

#include <cstdint>

namespace hw
{
    // A potentiometer on an ADC pin. read() gives 0 to MAX_VALUE.
    class Potentiometer
    {
    public:
        static constexpr std::uint16_t MAX_VALUE {4095};  // the largest 12-bit value

        explicit Potentiometer(std::uint32_t pin);

        std::uint16_t read() const;

    private:
        std::uint32_t pin_;
        // GP26 is ADC input 0, GP27 is input 1 (26 is potentiometer, 27 is light sensor)
        std::uint32_t adc_input_;
    };
}

#endif  // POTENTIOMETER_H
