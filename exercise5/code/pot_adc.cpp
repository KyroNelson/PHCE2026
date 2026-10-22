#include <cstdint>
#include <iostream>

#include "hardware/adc.h"
#include "pico/stdlib.h"

constexpr uint POT_PIN {26};             // GP26 is ADC input 0
constexpr uint POT_ADC_INPUT {0};
constexpr float VREF {3.3f};             // highest voltage the ADC reads
constexpr float ADC_MAX {4095.0f};       // 12 bits: 0 to 4095
constexpr float POT_OHMS {10000.0f};     // the value printed on your pot (B10K = 10 kOhm)

// Raw ADC reading to volts.
float raw_to_volts(std::uint16_t raw)
{
    // TODO 1: divide raw by ADC_MAX. That gives 0.0 to 1.0, how far the knob is turned.
    //         Multiply that by VREF to get volts.
    return 0.0f;
}

// Raw ADC reading to ohms, from the knob to GND.
float raw_to_ohms(std::uint16_t raw)
{
    // TODO 2: same idea, with POT_OHMS instead of VREF.
    return 0.0f;
}

int main()
{
    stdio_init_all();

    adc_init();
    adc_gpio_init(POT_PIN);           // GP26 becomes an analog input
    adc_select_input(POT_ADC_INPUT);  // read from input 0

    std::cout << "pot_adc started. Turn the knob." << std::endl;

    while (true)
    {
        const std::uint16_t raw {adc_read()};

        std::cout << "raw " << raw
                  << "  volts " << raw_to_volts(raw)
                  << "  ohms " << raw_to_ohms(raw) << std::endl;

        sleep_ms(250);
    }
}
