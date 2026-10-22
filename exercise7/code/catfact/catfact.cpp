#include <cstdint>
#include <iostream>
#include <string>

#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "lwip/altcp_tls.h"

// The helper is C code without extern "C" in its header (see Session 3).
extern "C"
{
#include "example_http_client_util.h"
}

// Set these in CMakeLists.txt (step 2). These are only placeholders.
#ifndef WIFI_SSID
#define WIFI_SSID "WIFI NAME HERE"
#endif

#ifndef WIFI_PASSWORD
#define WIFI_PASSWORD "PASSWORD HERE"
#endif

constexpr const char* HOST {"catfact.ninja"};
constexpr const char* URL {"/fact"};
constexpr std::uint32_t CONNECT_TIMEOUT_MS {30000};
constexpr std::uint32_t BUTTON_PIN {10};  // PicoBricks button
constexpr std::uint32_t POLL_MS {20};      // how often to check the button

// Called by lwIP with each piece of the body. arg is the std::string to fill.
err_t on_body(void* arg, struct altcp_pcb* conn, struct pbuf* p, err_t)
{
    auto* body = static_cast<std::string*>(arg);

    for (std::uint_fast16_t i {0}; i < p->tot_len; ++i)  // add each byte of the piece
    {
        *body += static_cast<char>(pbuf_get_at(p, i));
    }

    altcp_recved(conn, p->tot_len);  // tell lwIP the bytes were read
    pbuf_free(p);                    // and give the buffer back
    return ERR_OK;
}

// Finds "fact":"..." in the JSON body and returns the text between the quotes.
std::string extract_fact(const std::string& json)
{
    const std::string key {"\"fact\":\""};
    const auto start = json.find(key);
    if (start == std::string::npos)
    {
        return "(no fact in: " + json + ")";
    }
    const auto from = start + key.size();
    const auto end = json.find('"', from);
    return json.substr(from, end - from);
}

int main()
{
    stdio_init_all();

    if (cyw43_arch_init())
    {
        std::cout << "Wi-Fi chip failed to start" << std::endl;
        return 1;
    }
    cyw43_arch_enable_sta_mode();

    std::cout << "Connecting to " << WIFI_SSID << "..." << std::endl;
    if (cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASSWORD, CYW43_AUTH_WPA2_AES_PSK,
                                           CONNECT_TIMEOUT_MS))
    {
        std::cout << "Failed to connect" << std::endl;
        return 1;
    }

    std::cout << "Connected" << std::endl;

    gpio_init(BUTTON_PIN);
    gpio_set_dir(BUTTON_PIN, GPIO_IN);
    gpio_pull_down(BUTTON_PIN);

    for (unsigned fact_number {1}; true; ++fact_number)
    {
        std::cout << "Press the button for a cat fact" << std::endl;
        while (!gpio_get(BUTTON_PIN))
        {
            sleep_ms(POLL_MS);
        }

        std::string body;

        EXAMPLE_HTTP_REQUEST_T request {};
        request.hostname = HOST;
        request.url = URL;
        request.recv_fn = on_body;      // lwIP calls on_body with each piece
        request.callback_arg = &body;   // and hands it &body
        request.tls_config = altcp_tls_create_config_client(nullptr, 0);  // HTTPS

        const int result {http_client_request_sync(cyw43_arch_async_context(), &request)};
        altcp_tls_free_config(request.tls_config);

        if (result != 0)
        {
            std::cout << "Request failed: " << result << std::endl;
        }
        else
        {
            std::cout << "Body: " << body << std::endl;
            std::cout << "Fact " << fact_number << ": " << extract_fact(body) << std::endl;
        }

        while (gpio_get(BUTTON_PIN))  // wait for release, so one press gives one fact
        {
            sleep_ms(POLL_MS);
        }
    }
}
