#include "StorageManager.h"

static const char *TAG = "STORAGE";

Settings::Settings()
{
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

#ifdef CONFIG_STORAGE_RESET_ALL
    //TODO: NEEDS IMPLEMENTATION
    ESP_LOGW(TAG, "Resetting User NVS Settings");
    Settings::unset(SETTING_HEADERS::USERNAME);
#else
    nvs_handle_t handle = Settings::open_handle();

    size_t len = SETTING_STR_LEN::USERNAME;
    char username[SETTING_STR_LEN::USERNAME];
    Settings::read_str(handle, SETTING_HEADERS::USERNAME, username, &len);

    strcpy(this->username, username);
#endif
}

nvs_handle_t Settings::open_handle()
{
    // Open
    printf("\n");
    printf("Opening Non-Volatile Storage (NVS) handle... ");
    nvs_handle_t my_handle;
    esp_err_t err = nvs_open("storage", NVS_READWRITE, &my_handle);
    if (err != ESP_OK)
    {
        printf("Error (%s) opening NVS handle!\n", esp_err_to_name(err));
        return 0;
    }
    else
    {
        printf("Done\n");
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

    printf("Erasing %s from NVS ... ", key);
    esp_err_t err = nvs_erase_key(handle, key);

    switch (err)
    {
    case ESP_OK:
        printf("Done\n");
        break;
    case ESP_ERR_NVS_NOT_FOUND:
        printf("The value is not initialized yet!\n");
        break;
    default:
        printf("Error (%s) reading!\n", esp_err_to_name(err));
    }

    // Close
    nvs_close(handle);
}

void Settings::read_str(nvs_handle_t handle, const char *key, char *val, size_t *len)
{
    printf("Reading %s from NVS ... ", key);
    esp_err_t err = nvs_get_str(handle, key, val, len);

    switch (err)
    {
    case ESP_OK:
        printf("Done\n");
        printf("is %s = %s: len=%d\n", key, val, *len);
        break;
    case ESP_ERR_NVS_NOT_FOUND:
        printf("The value is not initialized yet!\n");
        break;
    default:
        printf("Error (%s) reading!\n", esp_err_to_name(err));
    }

    // Close
    nvs_close(handle);
}

void Settings::write_str(nvs_handle_t handle, const char *key, char *val)
{
    // Write
    printf("Updating %s to %s in NVS ... ", key, val);
    esp_err_t err = nvs_set_str(handle, key, val);
    printf((err != ESP_OK) ? "Failed!\n" : "Done\n");

    // Commit written value.
    // After setting any values, nvs_commit() must be called to ensure changes are written
    // to flash storage. Implementations may write to storage at other times,
    // but this is not guaranteed.
    printf("Committing updates in NVS ... ");
    err = nvs_commit(handle);
    printf((err != ESP_OK) ? "Failed!\n" : "Done\n");

    // Close
    nvs_close(handle);
}
