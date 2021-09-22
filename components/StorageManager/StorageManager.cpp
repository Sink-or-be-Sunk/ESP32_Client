#include "StorageManager.h"

//private headers
void nvs_erase(const char *key);

static const char *TAG = "STORAGE";

void storage_manager_init(void)
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
    nvs_erase(SETTING_HEADERS::USERNAME);
#endif
}

#ifdef CONFIG_STORAGE_RESET_ALL
void nvs_erase(const char *key)
{
    // Open
    nvs_handle_t my_handle;
    esp_err_t err = nvs_open("storage", NVS_READWRITE, &my_handle);
    if (err != ESP_OK)
    {
        printf("Error (%s) opening NVS handle!\n", esp_err_to_name(err));
    }
    else
    {
        printf("Done\n");

        printf("Erasing %s from NVS ... ", key);
        err = nvs_erase_key(my_handle, key);

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
        nvs_close(my_handle);
    }
}
#endif

void nvs_read(const char *key, char *val, size_t *len)
{
    // Open
    printf("\n");
    printf("Opening Non-Volatile Storage (NVS) handle... ");
    nvs_handle_t my_handle;
    esp_err_t err = nvs_open("storage", NVS_READWRITE, &my_handle);
    if (err != ESP_OK)
    {
        printf("Error (%s) opening NVS handle!\n", esp_err_to_name(err));
    }
    else
    {
        printf("Done\n");

        // Read
        printf("Reading %s from NVS ... ", key);
        // err = nvs_get_i8(my_handle, key, &val);
        err = nvs_get_str(my_handle, key, val, len);

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
        nvs_close(my_handle);
    }
}

void nvs_write(const char *key, char *val)
{
    // Open
    printf("\n");
    printf("Opening Non-Volatile Storage (NVS) handle... ");
    nvs_handle_t my_handle;
    esp_err_t err = nvs_open("storage", NVS_READWRITE, &my_handle);
    if (err != ESP_OK)
    {
        printf("Error (%s) opening NVS handle!\n", esp_err_to_name(err));
    }
    else
    {
        // Write
        printf("Updating %s to %s in NVS ... ", key, val);
        err = nvs_set_str(my_handle, key, val);
        printf((err != ESP_OK) ? "Failed!\n" : "Done\n");

        // Commit written value.
        // After setting any values, nvs_commit() must be called to ensure changes are written
        // to flash storage. Implementations may write to storage at other times,
        // but this is not guaranteed.
        printf("Committing updates in NVS ... ");
        err = nvs_commit(my_handle);
        printf((err != ESP_OK) ? "Failed!\n" : "Done\n");

        // Close
        nvs_close(my_handle);
    }
}
