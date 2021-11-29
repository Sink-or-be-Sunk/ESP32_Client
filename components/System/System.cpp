#include "System.h"

void get_device_ssid(char result[SSID_MAX_LEN])
{
    wifi_ap_record_t ap;
    esp_wifi_sta_get_ap_info(&ap);
    snprintf(result, SSID_MAX_LEN, "%s", ap.ssid);
}

void get_device_id(char *service_name)
{
    uint8_t eth_mac[6];
    esp_wifi_get_mac(WIFI_IF_STA, eth_mac);
    snprintf(service_name, DEVICE_ID_MAX_LEN, "%02X%02X%02X%02X%02X%02X",
             eth_mac[0], eth_mac[1], eth_mac[2],
             eth_mac[3], eth_mac[4], eth_mac[5]);
}