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
#include "StorageManager.h"

static const char *TAG = "MAIN"; //FIXME: THIS IS UNUSED RIGHT NOW

extern "C" void app_main(void)
{
    /* Initializes nvs (needed for wifi prov too!) */
    storage_manager_init();

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

    char username[SETTING_STR_LEN::USERNAME];
    size_t len = SETTING_STR_LEN::USERNAME;
    nvs_read(SETTING_HEADERS::USERNAME, username, &len);
    strcpy(username, "mitch");
    nvs_write(SETTING_HEADERS::USERNAME, username);

    // wifi_send(register_enqueue());

    // for (int i = 0; i < 5; i++)
    // {
    //     wifi_send(create_new_game_req());
    //     vTaskDelay(1000 / portTICK_RATE_MS);
    // }

    wifi_stop();
}