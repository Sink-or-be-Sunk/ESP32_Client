#include "Settings.h"

static const char *TAG = "STORAGE";

Settings settings; // singleton instance of class

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

#ifdef STORAGE_RESET_ALL
    ESP_LOGW(TAG, "Resetting User NVS Settings");
    Settings::unset(SETTING_HEADERS::USERNAME);

    ESP_LOGW(TAG, "Setting Default Values");
    this->username = something;

    Settings::save();
#else
    nvs_handle_t handle = Settings::open_handle();

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
    ESP_LOGI(TAG, "\nOpening Non-Volatile Storage (NVS) handle... ");
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
