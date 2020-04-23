#include "mbed.h"
#include "mbed_trace.h"
#include "mbedtls/debug.h"

extern "C" {
    extern int subscribe_publish(void);
}

#if TARGET_DISCO_L475VG_IOT01A
NetworkInterface *NetworkInterface::get_default_instance() {
    return WiFiInterface::get_default_instance();
}
#endif

int main()
{
    mbed_trace_init();

#ifdef MBED_MAJOR_VERSION
    printf("Mbed OS version %d.%d.%d\n\n", MBED_MAJOR_VERSION, MBED_MINOR_VERSION, MBED_PATCH_VERSION);
#endif

#if TARGET_DISCO_L475VG_IOT01A
    printf("WiFi example\n");
    WiFiInterface *wifi = WiFiInterface::get_default_instance();
    if (!wifi) {
        printf("ERROR: No WiFiInterface found.\n");
        return -1;
    }

    printf("\nConnecting to %s...\n", MBED_CONF_APP_WIFI_SSID);
    int ret = wifi->connect(MBED_CONF_APP_WIFI_SSID, MBED_CONF_APP_WIFI_PASSWORD, NSAPI_SECURITY_WPA_WPA2);
    printf("MAC: %s\n", wifi->get_mac_address());
#else
    printf("\nConnecting to ethernet...\n");
    auto eth = NetworkInterface::get_default_instance();
    int ret = eth->connect();
    printf("MAC: %s\n", eth->get_mac_address());
#endif
    if (ret != 0) {
        printf("\nConnection error: %d\n", ret);
        return -1;
    }

    printf("Success\n\n");
    subscribe_publish();
    while(1);
    return 0;
}
