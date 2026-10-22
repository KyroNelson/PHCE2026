#include "Connection.h"

// The DHCP server hands out addresses to the PCs that join the Pico's hotspot.
// It is C code without extern "C" in its header.
extern "C"
{
#include "dhserver.h"
}

// Line by line output is off, so the address below is easy to find.
// Add HTTP_SERVER_LINE_DEBUG to target_compile_definitions to turn it on.
#ifdef HTTP_SERVER_LINE_DEBUG
#define LineDebug(msg) printf("HTTP Server Line Debug: %s\n", msg)
#else
#define LineDebug(msg) ((void)(msg))
#endif
#define InfoDebug(msg) printf("HTTP Server: %s\n", msg)
#define ErrorDebug(msg) printf("HTTP Server Error: %s\n", msg)

constexpr std::size_t HOSTNAME_MAC_CHARS {4};           // last 4 hex digits of the MAC go into the host name
constexpr std::size_t HOSTNAME_MAC_OFFSET {8};          // skip the first 8 hex digits
constexpr std::size_t MAC_BYTES {6};                    // a MAC address has 6 bytes
constexpr unsigned NIBBLE_BITS {4};                     // one hex digit is 4 bits
constexpr std::uint8_t NIBBLE_MASK {0xF};               // keeps the lowest hex digit
constexpr u16_t PBUF_NOT_FOUND {0xFFFF};                // pbuf_memfind() returns this when nothing matches
constexpr u16_t HTTP_PORT {80};                         // standard web server port
constexpr u32_t MDNS_TTL_S {60};                        // how long others may cache our name (old lwIP only)
constexpr char MDNS_TXT_PATH[] {"path=/"};              // mDNS text record: the page is at "/"

constexpr int WIFI_ITF {CYW43_ITF_AP};                  // the Pico is the hotspot
constexpr std::size_t AP_SSID_MAX_LEN {32};             // WiFi names have at most 32 characters
constexpr std::size_t AP_PASSWORD_MIN_LEN {8};          // WPA2 needs at least 8 characters
constexpr std::uint32_t SCAN_POLL_MS {100};             // how often to check if the scan is done
constexpr u16_t DHCP_SERVER_PORT {67};                  // standard DHCP server port
constexpr std::size_t DHCP_CLIENTS {4};                 // PCs that can join at once
constexpr std::uint8_t DHCP_FIRST_HOST {16};            // first address handed out: x.x.x.16
constexpr std::uint32_t DHCP_LEASE_S {24 * 60 * 60};    // a PC keeps its address for a day

static_assert(sizeof(AP_SSID) > sizeof(""), "AP_SSID must not be empty");
static_assert(sizeof(AP_SSID) - 1 <= AP_SSID_MAX_LEN, "AP_SSID can have at most 32 characters");
static_assert(sizeof(AP_PASSWORD) - 1 >= AP_PASSWORD_MIN_LEN, "AP_PASSWORD needs at least 8 characters (WPA2)");


// Static member definitions
std::vector<tCGI> HTTPServer::m_cgi_handlers;            // Routes, kind of
std::vector<SSIHandler> HTTPServer::m_ssi_registry;      // Dynamic variables
std::vector<const char*> HTTPServer::m_tag_names;        // Dynamic variables, private for the C functions
std::vector<std::string> HTTPServer::m_tag_name_storage; // Dynamic variables, private storage of the data, to keep it persistent
std::vector<PostHandler> HTTPServer::m_post_handlers;    // POST handlers, what should happen for a given post
std::vector<PostContext> HTTPServer::m_post_context;     // Data stored between POSTs about the POST, used as context for a function to handle it
absolute_time_t HTTPServer::m_wifi_started_time;         // When the hotspot started


// Used to setup and start the server, should be used only once and only AFTER adding SSIs and CGIs
// returns 0 on success
// returns -1 if the hotspot did not start
int HTTPServer::Init(void)
{
    cyw43_arch_enable_sta_mode();                                                       LineDebug("Station mode on: the MAC and the scan need it");
    char hostname[sizeof(CYW43_HOST_NAME) + HOSTNAME_MAC_CHARS];                                             
    memcpy(&hostname[0], CYW43_HOST_NAME, sizeof(CYW43_HOST_NAME) - 1);                     LineDebug("Set a host name variable");
    get_mac_ascii(CYW43_HAL_MAC_WLAN0, HOSTNAME_MAC_OFFSET, HOSTNAME_MAC_CHARS, &hostname[sizeof(CYW43_HOST_NAME) - 1]);       LineDebug("Get MAC Address as ASCII");
    hostname[sizeof(hostname) - 1] = '\0';                                                  LineDebug("Setting termination character");

    netif_set_hostname(&cyw43_state.netif[WIFI_ITF], hostname);                        LineDebug("Set the hostname");

    if (!start_access_point())
    {
        ErrorDebug("Hotspot not started. Exiting with -1");
        return -1;
    }

    InfoDebug("WiFi is up");
    printf("\n=====================================\n");
    printf("Open in your browser: http://%s\n", ip4addr_ntoa(netif_ip4_addr(&cyw43_state.netif[WIFI_ITF])));
    printf("=====================================\n");

    
    m_wifi_started_time = get_absolute_time();                                          LineDebug("Getting the start time");

    #if LWIP_MDNS_RESPONDER
    dns_init(hostname);                                                                 LineDebug("DNS Setup");
    #endif

    m_tag_names.reserve(m_tag_name_storage.size());                                     LineDebug("Space for the m_tag_names (SSIs) is reserved");

    for (std::size_t i = 0; i < m_tag_name_storage.size();i++)
    {
        m_tag_names.push_back(m_tag_name_storage[i].c_str());                           LineDebug(("Adding a tag: " + m_tag_name_storage[i]).c_str());
    }



    cyw43_arch_lwip_begin();                                                            LineDebug("cyw43_arch_lwip_begin");

    httpd_init();                                                                       LineDebug("HTTP inited");

    http_set_cgi_handlers(m_cgi_handlers.data(), m_cgi_handlers.size());                LineDebug("Adding CGI handlers");

    http_set_ssi_handler(ssi_handler_function, m_tag_names.data(), m_tag_names.size()); LineDebug("Adding SSI handlers");

    cyw43_arch_lwip_end();                                                              LineDebug("cyw43_arch_lwip_end");

    return 0;

}


// Used to add CGI Handlers, which are the routes
// returns true on success
// returns false if the handler already exists for that path
bool HTTPServer::AddCGIHandler(tCGI& handler)
{
    InfoDebug(("Adding a CGI handler - " + std::string(handler.pcCGIName)).c_str());
    InfoDebug("Checking whether the CGI handler already exists");
    for(auto& cgi : m_cgi_handlers)
    {
        if(strcmp(cgi.pcCGIName, handler.pcCGIName) == 0)
        {
            ErrorDebug("Returning false as the handler already exists");
            return false;
        }
    }

    LineDebug("Reserving space for the CGI handlers");
    m_cgi_handlers.reserve(m_cgi_handlers.capacity() + 1); // In order to prevent dynamic vector size increase, which is 2n, doubling the capacity if not enough
    m_cgi_handlers.push_back(handler);                          LineDebug("CGI Handler added");
    return true;
}

// Get the reference to the vector with CGIs (Routes)
// returns a const reference, so that it cannot be changed
const std::vector<tCGI>& HTTPServer::GetCGIHandlers(void)
{
    LineDebug("Returning CGI Handlers reference");
    return m_cgi_handlers;
}


// Used to get MAC of the device as ASCII
// returns size of the array
// returned mac is in dest_in
std::size_t HTTPServer::get_mac_ascii(int idx, std::size_t chr_off, std::size_t chr_len, char *dest_in)
{
    static constexpr char hexchr[] {"0123456789ABCDEF"};                              LineDebug("Setting up the MAC variable");
    std::uint8_t mac[MAC_BYTES];
    char *dest = dest_in;
    assert(chr_off + chr_len <= (2 * sizeof(mac)));
    cyw43_hal_get_mac(idx, mac);                                                    LineDebug("Getting a MAC from the drivers");
    for (; chr_len && (chr_off >> 1) < sizeof(mac); ++chr_off, --chr_len) 
    {
        *dest++ = hexchr[mac[chr_off >> 1] >> (NIBBLE_BITS * (1 - (chr_off & 1))) & NIBBLE_MASK];
    }
    return dest - dest_in;
}


// Used to get the parameter value from a pbuf (POST buffer object)
// retuns the pointer to the buffer
// returned value is stored inside value_buf
// returns nullptr on fail
char *HTTPServer::httpd_param_value(struct pbuf *p, const char *param_name, char *value_buf, std::size_t value_buf_len) 
{
    LineDebug(("Getting a param " + std::string(param_name) + " value").c_str());
    const std::size_t param_len = strlen(param_name);
    u16_t param_pos = pbuf_memfind(p, param_name, param_len, 0);            LineDebug("Found up the position of the param");
    if (param_pos != PBUF_NOT_FOUND) 
    {
        u16_t param_value_pos = param_pos + param_len;
        u16_t param_value_len = 0;
        u16_t tmp = pbuf_memfind(p, "&", 1, param_value_pos);
        if (tmp != PBUF_NOT_FOUND) 
        {
            param_value_len = tmp - param_value_pos;
        } else 
        {
            param_value_len = p->tot_len - param_value_pos;
        }
        if (param_value_len > 0 && param_value_len < value_buf_len)
        {
            char *result = (char *)pbuf_get_contiguous(p, value_buf, value_buf_len, param_value_len, param_value_pos);  LineDebug("Getting the result from the buffer");
            LineDebug("Checking if the result exists");
            if (result)
            {
                result[param_value_len] = 0;        LineDebug("Adding a teminator");
                LineDebug("Returning the result");
                return result;
            }else
            {
                ErrorDebug("Parameter doesn't exists");
            }
        }
    }
    ErrorDebug("No parameter found, returning nullptr");
    return nullptr;
}


// Used to add SSI Handler, which is a dynamic variable
// The function copies the handler and important information, so persistency is not needed
// returns true on success
// returns false on fail - Handler already exists
// Be aware of LWIP_HTTPD_MAX_TAG_NAME_LEN, as it is the maximum length for the value of the returning value
bool HTTPServer::AddSSIHandler(const SSIHandler& handler) 
{
    LineDebug("Checking whether the tags repeat");
    for (auto& h : m_ssi_registry)
    {
        if (h.tag == handler.tag)
        {
            ErrorDebug("The handler already exists, return false and aborting the operation");
            return false;
        }
    }

    LineDebug("Reserving space for the additional tag");
    m_tag_name_storage.reserve(m_tag_name_storage.capacity()+1);
    m_tag_name_storage.push_back(handler.tag);                      LineDebug("Tag added");

    LineDebug("Reserving space for the SSI Handler");
    m_ssi_registry.reserve(m_ssi_registry.capacity() + 1); // In order to prevent dynamic vector size increase, which is 2n, doubling the capacity if not enough
    m_ssi_registry.push_back(handler);                              LineDebug("SSI Handler added");
    return true;
}


// Get the reference to the vector with SSIs (Dynamic Variables)
// returns a const reference, so that it cannot be changed
const std::vector<SSIHandler>& HTTPServer::GetSSIHandlers(void)
{
    LineDebug("Returning SSI Handlers reference");
    return m_ssi_registry;
}


// Used to clean up the HTTPServer before closing the program, removes netif
void HTTPServer::Cleanup(void)
{
LineDebug("Starting the cleanup");
#if LWIP_MDNS_RESPONDER
    mdns_resp_remove_netif(&cyw43_state.netif[WIFI_ITF]);      LineDebug("Netif removed");
#endif
    cyw43_arch_deinit();                                            LineDebug("cyw43_arch deinited");

    m_cgi_handlers.clear();                                         LineDebug("Clearing CGI handlers");
    m_post_context.clear();                                         LineDebug("Clearing Post contexts");
    m_post_handlers.clear();                                        LineDebug("Clearing Post handlers");
    m_ssi_registry.clear();                                         LineDebug("Clearing SSI handlers");
    m_tag_name_storage.clear();                                     LineDebug("Clearing CGI tag storage");
    m_tag_names.clear();                                            LineDebug("Clearing CGI tag pointers, storage for C");

}



// Are POSTs supported?
#if LWIP_HTTPD_SUPPORT_POST

// Used to add Post Handler, so what happens for a given post
// returns true on success
// To add check for the URL
bool HTTPServer::AddPostHandler(PostHandler& handler)
{
    InfoDebug("Checking whether the Post handler already exists");
    for(auto& ph : m_post_handlers)
    {
        if(ph.url == handler.url)
        {
            ErrorDebug("Returning false as the handler already exists");
            return false;
        }
    }

    LineDebug("Reserving space for the additional Post handler");
    m_post_handlers.reserve(m_post_handlers.capacity() + 1); // In order to prevent dynamic vector size increase, which is 2n, doubling the capacity if not enough
    m_post_handlers.push_back(handler);                                 LineDebug("Post handler added");
    return true;
}


// Get the reference to the vector with POST handlers
// returns a const reference, so that it cannot be changed
const std::vector<PostHandler>& HTTPServer::GetPostHandler(void)
{
    LineDebug("Returning Post Handlers reference");
    return m_post_handlers;
}


// Internal function to handle begining of the POST
// It creates Post Context for a given connection and stores important data
// Placeholder page is places
// Sets state of the Context connection to be BEGIN
// returns ERR_OK on success
// returns ERR_VAL on fail: No handler found; this connection already has on POST which is being processed
err_t HTTPServer::httpd_post_begin(void *connection, const char *uri, const char *http_request,u16_t http_request_len, int content_len, char *response_uri,u16_t response_uri_len, u8_t *post_auto_wnd)
{
    LineDebug("POST Begin triggered");
    LWIP_UNUSED_ARG(http_request);
    LWIP_UNUSED_ARG(http_request_len);
    LWIP_UNUSED_ARG(content_len);

    for (auto& h : m_post_handlers) 
    {
        if (strcmp(uri, h.url.c_str()) == 0) 
        {
            LineDebug("POST Handler for the URL Found");

            for(auto& p : m_post_context)
            {
                if(p.connection == connection)
                {
                    ErrorDebug("POST context for this connection already exists, aborting due to single POST policy");
                    return ERR_VAL;
                }
            }

            PostContext pc{connection, uri, BEGIN, nullptr};            LineDebug("Post context created");

            m_post_context.push_back(pc);                               LineDebug("Post context added");

            snprintf(response_uri, response_uri_len, "%s", h.placeholder_page.c_str());     LineDebug("Setting the placeholder name as the response url");

            *post_auto_wnd = 1;
            return ERR_OK;
        }
    }

    ErrorDebug("No Post Handler was found for this POST");
    return ERR_VAL; // no handler found
            
}


// Internal function to handle receiving data from the POST
// Saves the buffer and changes the state of a context to RECEIVE
// returns ERR_OK on success
// returns ERR_VAL on fail, in case no context was found (which should not happened ever)
err_t HTTPServer::httpd_post_receive_data(void *connection, struct pbuf *p) 
{
    LineDebug("POST Receive data stage is happening");
    LWIP_ASSERT("NULL pbuf", p != nullptr); // could be removed

    for (auto& pc : m_post_context) 
    {
        if (pc.connection == connection && pc.ps == BEGIN)
        {
            LineDebug("Context for this connection was found");
            pc.ps = RECEIVE;
            pc.buf = p;                 LineDebug("Buffer is saved into the context");

            return ERR_OK;
        }
        // pbuf_free(p); Cannot free it here, as the lambda to determine what to do runs later on and uses it
    }

    ErrorDebug("No context was found so nothing happen (This should not happen)");
    return ERR_VAL;
}


// Internal function to handle finishing the POST 
// Sets the state to END at the start
// Runs the function in the handler, saves the result as the new uri (NULL for no change)
// If things went well, state changes to REMOVE, so it cleans up the POST buffer and erases currect context, so that a user can do another POST
void HTTPServer::httpd_post_finished(void *connection, char *response_uri, u16_t response_uri_len) 
{
    LineDebug("POST finish stage is happening");
    for (std::size_t i = 0; i < m_post_context.size(); i++) 
    {
        PostContext& pc = m_post_context[i];

        if (pc.connection == connection && pc.ps == RECEIVE)
        {
            LineDebug("Post context for this connection was found");
            pc.ps = END;

            for(PostHandler& ph : m_post_handlers)
            {
                if(ph.url == pc.url)
                {
                    LineDebug("Post handler for this context was found");
                    std::string result = ph.fn(connection,&pc);                         LineDebug("Running the post handler function");
                    snprintf(response_uri, response_uri_len, "%s", result.c_str());     LineDebug("Response URL was set");
                    break;
                }
            }
            
            pc.ps = REMOVE;
            pbuf_free(pc.buf);                                                          LineDebug("Post buffer freed");
            m_post_context.erase(m_post_context.begin() + i);                           LineDebug("Context for removed, so the user can submit new POST");
            return;
        }
    }

    ErrorDebug("No suitable context found, should not happen");
}



#endif // LWIP_HTTPD_SUPPORT_POST


// Scans for WiFi networks around the Pico
// returns true if one of them is called ssid, or if the scan fails
bool HTTPServer::ssid_in_use(const char* ssid)
{
    struct ScanState
    {
        const char* ssid;
        bool found;
    };
    ScanState state {ssid, false};

    cyw43_wifi_scan_options_t options {};
    const int error = cyw43_wifi_scan(&cyw43_state, &options, &state,
        [](void* env, const cyw43_ev_scan_result_t* result) -> int
        {
            auto* scan = static_cast<ScanState*>(env);
            const std::size_t length = strlen(scan->ssid);
            if (result && result->ssid_len == length && memcmp(result->ssid, scan->ssid, length) == 0)
            {
                scan->found = true;
            }
            return 0;
        });
    if (error)
    {
        ErrorDebug("WiFi scan failed");
        return true;
    }

    while (cyw43_wifi_scan_active(&cyw43_state))
    {
        sleep_ms(SCAN_POLL_MS);
    }
    return state.found;
}


// Starts the Pico's own hotspot, called AP_SSID, and a DHCP server for the PCs that join
// returns false if a network called AP_SSID is already around
bool HTTPServer::start_access_point(void)
{
    InfoDebug("Checking that nobody else uses the name " AP_SSID);
    if (ssid_in_use(AP_SSID))
    {
        ErrorDebug("A WiFi called " AP_SSID " is already around. Pick a unique AP_SSID in wifi.cmake");
        return false;
    }

    cyw43_arch_disable_sta_mode();
    cyw43_arch_enable_ap_mode(AP_SSID, AP_PASSWORD, CYW43_AUTH_WPA2_AES_PSK);
    printf("HTTP Server: Hotspot " AP_SSID " is up. Join it with your PC.\n");

    // The Pico's own address, from the driver, is the router and DNS for the clients
    const ip4_addr_t* own = netif_ip4_addr(&cyw43_state.netif[CYW43_ITF_AP]);
    const ip4_addr_t* mask = netif_ip4_netmask(&cyw43_state.netif[CYW43_ITF_AP]);

    static dhcp_entry_t entries[DHCP_CLIENTS] {};
    for (std::size_t i = 0; i < DHCP_CLIENTS; i++)
    {
        entries[i].addr[0] = ip4_addr1(own);
        entries[i].addr[1] = ip4_addr2(own);
        entries[i].addr[2] = ip4_addr3(own);
        entries[i].addr[3] = DHCP_FIRST_HOST + i;
        entries[i].subnet[0] = ip4_addr1(mask);
        entries[i].subnet[1] = ip4_addr2(mask);
        entries[i].subnet[2] = ip4_addr3(mask);
        entries[i].subnet[3] = ip4_addr4(mask);
        entries[i].lease = DHCP_LEASE_S;
    }

    static dhcp_config_t config {};
    config.addr[0] = config.dns[0] = ip4_addr1(own);
    config.addr[1] = config.dns[1] = ip4_addr2(own);
    config.addr[2] = config.dns[2] = ip4_addr3(own);
    config.addr[3] = config.dns[3] = ip4_addr4(own);
    config.port = DHCP_SERVER_PORT;
    config.domain = "";
    config.num_entry = DHCP_CLIENTS;
    config.entries = entries;

    cyw43_arch_lwip_begin();
    const err_t error = dhserv_init(&config);
    cyw43_arch_lwip_end();
    if (error != ERR_OK)
    {
        ErrorDebug("DHCP server failed to start");
        return false;
    }
    return true;
}



// Should we support DNS server?
#if LWIP_MDNS_RESPONDER

// Adds a text record to the DNS
// The page is accessible from the "/" path
void HTTPServer::srv_txt(struct mdns_service *service, void *txt_userdata)
{
    LineDebug("Setting up the service adding");
    err_t res;
    LWIP_UNUSED_ARG(txt_userdata);

    res = mdns_resp_add_service_txtitem(service, MDNS_TXT_PATH, sizeof(MDNS_TXT_PATH) - 1);
    LWIP_ERROR("mdns add service txt failed\n", (res == ERR_OK), return);
}


// Sets so that the page can be accessed through a host name (hostname)
void HTTPServer::dns_init(char* hostname)
{
    LineDebug("Setting up the dns server");
    // Setup mdns
    cyw43_arch_lwip_begin();
    mdns_resp_init();
    printf("Or, if your PC finds .local names: http://%s.local\n", hostname);
#if LWIP_VERSION_MAJOR >= 2 && LWIP_VERSION_MINOR >= 2
    mdns_resp_add_netif(&cyw43_state.netif[WIFI_ITF], hostname);
    mdns_resp_add_service(&cyw43_state.netif[WIFI_ITF], "pico_httpd", "_http", DNSSD_PROTO_TCP, HTTP_PORT, srv_txt, nullptr);
#else
    mdns_resp_add_netif(&cyw43_state.netif[WIFI_ITF], hostname, MDNS_TTL_S);
    mdns_resp_add_service(&cyw43_state.netif[WIFI_ITF], "pico_httpd", "_http", DNSSD_PROTO_TCP, HTTP_PORT, MDNS_TTL_S, srv_txt, nullptr);
#endif
    cyw43_arch_lwip_end();

    LineDebug("DNS set up");
}


#endif


// A small hack to make the C linker access the correct functions for the POST operations
// Because just having them static doesn't work with the linker 
#if LWIP_HTTPD_SUPPORT_POST
extern "C" {

err_t httpd_post_begin(void *conn, const char *uri, const char *http_request,
                       u16_t http_request_len, int content_len, char *response_uri,
                       u16_t response_uri_len, u8_t *post_auto_wnd)
{
    return HTTPServer::httpd_post_begin(conn, uri, http_request, http_request_len,
                                       content_len, response_uri, response_uri_len, post_auto_wnd);
}

err_t httpd_post_receive_data(void *conn, struct pbuf *p)
{
    return HTTPServer::httpd_post_receive_data(conn, p);
}

void httpd_post_finished(void *conn, char *response_uri, u16_t response_uri_len)
{
    HTTPServer::httpd_post_finished(conn, response_uri, response_uri_len);
}

} // extern "C"
#endif


// Internal, private function to handle SSI (Dynamic functions), function for the C lib
// Checks if the name exists in saved states, if so put it into the buffer, so it can be changed on the page
// return the size of the inserted buffer
u16_t HTTPServer::ssi_handler_function(int iIndex, char *pcInsert, int iInsertLen
#if LWIP_HTTPD_SSI_MULTIPART
, u16_t current_tag_part, u16_t *next_tag_part
#endif
)
{
    LineDebug("SSI handler run");
#if LWIP_HTTPD_SSI_MULTIPART
    LWIP_UNUSED_ARG(current_tag_part);
    LWIP_UNUSED_ARG(next_tag_part);
#endif
    if (iIndex < 0 || iIndex >= static_cast<int>(m_ssi_registry.size()))
        return 0;

    std::string result = m_ssi_registry[iIndex].func();         LineDebug("Running the SSI function");

    // Copy safely into buffer
    strncpy(pcInsert, result.c_str(), iInsertLen);              LineDebug("Copy into the buffer");
    pcInsert[iInsertLen - 1] = '\0';                            LineDebug("Adding NULL terminator");
    const std::size_t inserted = strnlen(pcInsert, iInsertLen);            LineDebug("Getting the length to be returned");

    return inserted;
}