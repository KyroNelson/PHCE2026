#ifndef PWM_LED_H
#define PWM_LED_H

#include <cstdint>

namespace hw
{
    // An LED on a PWM pin. Its brightness goes from 0 (off) to MAX_LEVEL (fully on).
    class PwmLed
    {
    public:
        // static: one copy, shared by all PwmLed objects
        static constexpr std::uint16_t MAX_LEVEL {255};

        explicit PwmLed(std::uint32_t pin);
        ~PwmLed();

        void set_level(std::uint16_t level);
        std::uint16_t level() const;

    private:
        std::uint32_t pin_;
        std::uint16_t level_;
    };
}

#endif  // PWM_LED_H
