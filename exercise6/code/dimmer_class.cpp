#include <cstdint>
#include <iostream>

#include "hardware/adc.h"
#include "hardware/pwm.h"
#include "pico/stdlib.h"

constexpr std::uint32_t LED_PIN {7};
constexpr std::uint32_t POT_PIN {26};
// GP26 is ADC input 0, GP27 is input 1 (26 is potentiometer, 27 is light sensor)
constexpr std::uint32_t ADC_FIRST_PIN {26};
constexpr std::uint16_t PWM_WRAP {255};
constexpr std::uint32_t ADC_TO_PWM_SHIFT {4};
// The pin is high while the counter is below level. The counter stops at PWM_WRAP,
// so PWM_WRAP + 1 keeps the pin high all the time: fully on.
constexpr std::uint16_t FULL_ON_LEVEL {PWM_WRAP + 1};
constexpr int DEMO_UPDATES {100};        // 100 updates of 100 ms: 10 seconds
constexpr std::uint32_t UPDATE_MS {100};
constexpr std::uint32_t IDLE_MS {1000};          // main() is done: just wait

// One class does everything: ADC, PWM, the maths and the printing.
class Dimmer
{
public:
    // TODO 1: initialize led_pin_, pot_pin_ and level_ with a member initializer list.
    //         level_ starts at 0.
    Dimmer(std::uint32_t led_pin, std::uint32_t pot_pin)
    {
        adc_init();
        adc_gpio_init(pot_pin_);
        adc_select_input(pot_pin_ - ADC_FIRST_PIN);

        gpio_set_function(led_pin_, GPIO_FUNC_PWM);
        const std::uint32_t slice {pwm_gpio_to_slice_num(led_pin_)};
        pwm_set_wrap(slice, PWM_WRAP);
        pwm_set_enabled(slice, true);

        std::cout << "Dimmer created" << std::endl;
    }

    ~Dimmer()  // runs when the object is destroyed
    {
        pwm_set_gpio_level(led_pin_, 0);
        std::cout << "Dimmer destroyed" << std::endl;
    }

    // Reads the pot, sets the LED and prints both.
    void update()
    {
        const std::uint16_t raw {adc_read()};
        level_ = raw >> ADC_TO_PWM_SHIFT;
        // at PWM_WRAP the pin is still low for one count per period
        if (level_ == PWM_WRAP)
        {
            level_ = FULL_ON_LEVEL;
        }
        pwm_set_gpio_level(led_pin_, level_);
        std::cout << "raw " << raw << "  level " << level_ << std::endl;
    }

    // TODO 2: write a member function level() that returns level_.
    //         Make it const: it must not change the object.

private:
    std::uint32_t led_pin_;
    std::uint32_t pot_pin_;
    std::uint16_t level_;  // the last PWM level, 0 to FULL_ON_LEVEL
};

// dimmer is a reference-to-const: no copy, and only const member functions can be called.
void print_level(const Dimmer& dimmer)
{
    // TODO 3: print "last level " and dimmer.level().
}

int main()
{
    stdio_init_all();

    {
        Dimmer dimmer {LED_PIN, POT_PIN};

        for (int i {0}; i < DEMO_UPDATES; ++i)
        {
            dimmer.update();
            sleep_ms(UPDATE_MS);
        }

        print_level(dimmer);
    }  // dimmer goes out of scope here

    std::cout << "main() is done" << std::endl;

    while (true)
    {
        sleep_ms(IDLE_MS);
    }
}
