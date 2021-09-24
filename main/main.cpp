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
#include "ButtonManager.h"
#include "WifiManager.h"
#include "Settings.h"

static const char *TAG = "MAIN"; //FIXME: THIS IS UNUSED RIGHT NOW

extern "C" void app_main(void)
{
    ESP_LOGI(TAG, "Starting App Main");

    /* Initializes nvs (needed for wifi prov too!) */
    settings.init();

    /* Initialize the event loop */
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    wifi_manager_init();

    /* Start Websocket */
    websocket.start();

    button_manager_init();

    // printf("username: %s\n", settings.username);
    // settings.username[0] += 1;
    // settings.save();

    // wifi_send(register_enqueue());

    // for (int i = 0; i < 5; i++)
    // {
    //     wifi_send(create_new_game_req());
    //     vTaskDelay(1000 / portTICK_RATE_MS);
    // }

    // wifi_stop();
}