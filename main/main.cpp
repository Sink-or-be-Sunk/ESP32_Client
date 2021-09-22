/* Wi-Fi Provisioning Manager Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <stdio.h>
#include <string.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/event_groups.h>

#include <esp_log.h>
#include <esp_wifi.h>
#include <esp_event.h>
#include <nvs_flash.h>

#include "Websocket.h"

#include "RegistrationManager.h"
#include "WifiManager.h"

static const char *TAG = "MAIN"; //FIXME: THIS IS UNUSED RIGHT NOW

// nvs code

#define REGISTER_TAG "isRegistered"

int8_t nvs_read_i8(const char *key)
{

    int8_t val = 0; // value will default to 0, if not set yet in NVS

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
        printf("Reading isRegistered from NVS ... ");
        err = nvs_get_i8(my_handle, key, &val);
        switch (err)
        {
        case ESP_OK:
            printf("Done\n");
            printf("is %s = %d\n", key, val);
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
    return val;
}

void nvs_write_i8(const char *key, int8_t val)
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
        printf("Updating %s to %d in NVS ... ", key, val);
        err = nvs_set_i8(my_handle, key, val);
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

extern "C" void app_main(void)
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

    /* Initialize TCP/IP */
    ESP_ERROR_CHECK(esp_netif_init());

    /* Initialize the event loop */
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    wifi_manager_init();

    /* Start Websocket */
    websocket_app_start();

    //FIXME: THIS IS NOT HOW THESE FUNCTIONS SHOULD BE CALLED: SEE README (this is testing only)
    char *enqueue = register_enqueue();
    printf("%s\n", enqueue);
    free(enqueue);

    char *confirm = register_confirm();
    printf("%s\n", confirm);
    free(confirm);

    registration_manager_init();

    int8_t isRegistered = nvs_read_i8(REGISTER_TAG);
    isRegistered++;
    nvs_write_i8(REGISTER_TAG, isRegistered);

    // wifi_send(register_enqueue());

    // for (int i = 0; i < 5; i++)
    // {
    //     wifi_send(create_new_game_req());
    //     vTaskDelay(1000 / portTICK_RATE_MS);
    // }

    wifi_stop();
}