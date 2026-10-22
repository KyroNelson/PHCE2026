#include <cstdint>
#include <cstdio>
#include <string>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "hardware/adc.h"

#include "Connection.h"


constexpr std::uint32_t LED_PIN {7};        // PicoBricks red LED
constexpr std::uint32_t BUTTON_PIN {10};    // PicoBricks button
constexpr std::uint32_t TEMP_ADC_INPUT {4}; // on-chip temperature sensor
constexpr float VREF {3.3f};                // highest voltage the ADC reads
constexpr float ADC_STEPS {4096.0f};        // 12-bit ADC
constexpr float SENSOR_REF_TEMP {27.0f};    // RP2040 datasheet: at 27 C ...
constexpr float SENSOR_REF_VOLTS {0.706f};  // ... the sensor gives 0.706 V
constexpr float SENSOR_VOLTS_PER_C {0.001721f};  // and drops 1.721 mV per degree
constexpr std::int64_t US_PER_S {1000000};  // microseconds in a second
constexpr std::uint32_t IDLE_MS {20};       // pause in the main loop


// Reads the on-chip temperature sensor, in degrees Celsius (Session 2)
float read_temperature()
{
    adc_select_input(TEMP_ADC_INPUT);
    const float voltage = adc_read() * VREF / ADC_STEPS;
    return SENSOR_REF_TEMP - (voltage - SENSOR_REF_VOLTS) / SENSOR_VOLTS_PER_C;
}


int main()
{
    stdio_init_all();

    if (cyw43_arch_init())
    {
        printf("failed to initialise\n");
        return 1;
    }

    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    gpio_put(LED_PIN, false);

    gpio_init(BUTTON_PIN);
    gpio_set_dir(BUTTON_PIN, GPIO_IN);
    gpio_pull_down(BUTTON_PIN);

    adc_init();
    adc_set_temp_sensor_enabled(true);

    // SSIs fill in the tags in index.shtml, like <!--#uptime-->

    // Seconds since the hotspot started
    HTTPServer::AddSSIHandler({"uptime",
        []() -> std::string
        {
            const std::int64_t uptime_s {absolute_time_diff_us(HTTPServer::m_wifi_started_time, get_absolute_time()) / US_PER_S};
            return std::to_string(uptime_s);
        }
    });

    // The red LED - ON or OFF
    HTTPServer::AddSSIHandler({"led",
        []() -> std::string
        {
            return gpio_get(LED_PIN) ? "ON" : "OFF";
        }
    });

    // The button - PRESSED or RELEASED
    HTTPServer::AddSSIHandler({"button",
        []() -> std::string
        {
            return gpio_get(BUTTON_PIN) ? "PRESSED" : "RELEASED";
        }
    });

    // TODO 1: add an SSI "temp" with the chip temperature.


    // ===================================================================

    // CGIs are routes: a path, and the page to send back

    // "/led/on" switches the red LED on
    tCGI cgi_on;
    cgi_on.pcCGIName = "/led/on";
    cgi_on.pfnCGIHandler = [](int, int, char *[], char *[]) -> const char*
    {
        gpio_put(LED_PIN, true);
        return "/index.shtml";
    };
    HTTPServer::AddCGIHandler(cgi_on);

    // TODO 2: add a CGI "/led/off" that switches the red LED off.


    // ===================================================================

    HTTPServer::Init();                              // INIT of the HTTPServer, start of the webserver


    while (true)
    {
        sleep_ms(IDLE_MS);
    }
}
