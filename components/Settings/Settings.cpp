#include "Settings.h"

static const char *TAG = "SETTINGS";

Settings settings; // singleton instance of class

static void get_device_ssid(char result[SETTING_STR_LEN::SSID])
{
    wifi_ap_record_t ap;
    esp_wifi_sta_get_ap_info(&ap);
    snprintf(result, SETTING_STR_LEN::SSID, "%s", ap.ssid);
}

static void get_device_id(char *service_name)
{
    uint8_t eth_mac[6];
    esp_wifi_get_mac(WIFI_IF_STA, eth_mac);
    snprintf(service_name, 13, "%02X%02X%02X%02X%02X%02X",
             eth_mac[0], eth_mac[1], eth_mac[2],
             eth_mac[3], eth_mac[4], eth_mac[5]);
}

void Settings::init(void)
{
    ESP_LOGI(TAG, "Initializing...");

    /* Initialize NVS partition */
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        /* NVS partition was truncated
         * and needs to be erased */
        ESP_ERROR_CHECK(nvs_flash_erase());

        /* Retry nvs_flash_init */
        ESP_ERROR_CHECK(nvs_flash_init());
    }
    strncpy(this->ssid, "DEFAULT_SSID", SETTING_STR_LEN::SSID);

#ifdef STORAGE_RESET_ALL
    ESP_LOGW(TAG, "Resetting User NVS Settings");
    Settings::unset(SETTING_HEADERS::USERNAME);

    ESP_LOGW(TAG, "Setting Default Values");
    get_device_id(this->username);

    Settings::save();
#else
    nvs_handle_t handle = Settings::open_handle();

    // FIXME: THIS CAN BE REMOVED TO REPLACE WITH JUST THIS->USERNAME
    size_t len = SETTING_STR_LEN::USERNAME;
    char username[SETTING_STR_LEN::USERNAME];
    Settings::read_str(handle, SETTING_HEADERS::USERNAME, username, &len);
    strncpy(this->username, username, len);
    ESP_LOGI(TAG, "Loaded Username: %s", this->username);
#endif

    ESP_LOGI(TAG, "Success");
}

nvs_handle_t Settings::open_handle()
{
    // Open
    ESP_LOGI(TAG, "Opening Non-Volatile Storage (NVS) handle... ");
    nvs_handle_t my_handle;
    esp_err_t err = nvs_open("storage", NVS_READWRITE, &my_handle);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Error (%s) opening NVS handle!", esp_err_to_name(err));
        return 0;
    }
    else
    {
        ESP_LOGI(TAG, "Done");
        return my_handle;
    }
}

void Settings::updateSSID(void)
{
    get_device_ssid(this->ssid);
}

void Settings::save()
{
    nvs_handle_t handle = Settings::open_handle();

    if (handle)
    {
        Settings::write_str(handle, SETTING_HEADERS::USERNAME, this->username);
    }
}

void Settings::unset(const char *key)
{
    nvs_handle_t handle = Settings::open_handle();

    ESP_LOGW(TAG, "Erasing %s from NVS ... ", key);
    esp_err_t err = nvs_erase_key(handle, key);

    switch (err)
    {
    case ESP_OK:
        ESP_LOGI(TAG, "Done\n");
        break;
    case ESP_ERR_NVS_NOT_FOUND:
        ESP_LOGE(TAG, "The value is not initialized yet!");
        break;
    default:
        ESP_LOGE(TAG, "Error (%s) reading!", esp_err_to_name(err));
    }

    // Close
    nvs_close(handle);
}

void Settings::read_str(nvs_handle_t handle, const char *key, char *val, size_t *len)
{
    ESP_LOGI(TAG, "Reading %s from NVS ... ", key);
    esp_err_t err = nvs_get_str(handle, key, val, len);

    switch (err)
    {
    case ESP_OK:
        ESP_LOGI(TAG, "Done");
        ESP_LOGI(TAG, "%s = %s: len=%d\n", key, val, *len);
        break;
    case ESP_ERR_NVS_NOT_FOUND:
        ESP_LOGE(TAG, "The value is not initialized yet!");
        break;
    default:
        ESP_LOGE(TAG, "Error (%s) reading!", esp_err_to_name(err));
    }

    // Close
    nvs_close(handle);
}

void Settings::write_str(nvs_handle_t handle, const char *key, char *val)
{
    // Write
    ESP_LOGW(TAG, "Updating %s to %s in NVS ... ", key, val);
    esp_err_t err = nvs_set_str(handle, key, val);
    if (err == ESP_OK)
    {
        ESP_LOGI(TAG, "Done");
    }
    else
    {
        ESP_LOGE(TAG, "Error (%s) writing!", esp_err_to_name(err));
    }

    // Commit written value.
    // After setting any values, nvs_commit() must be called to ensure changes are written
    // to flash storage. Implementations may write to storage at other times,
    // but this is not guaranteed.
    ESP_LOGI(TAG, "Committing updates in NVS ...");
    err = nvs_commit(handle);
    if (err == ESP_OK)
    {
        ESP_LOGI(TAG, "Done");
    }
    else
    {
        ESP_LOGE(TAG, "Error (%s) commiting!", esp_err_to_name(err));
    }

    // Close
    nvs_close(handle);
}
