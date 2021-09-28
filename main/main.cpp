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
#include "Display.h"

static const char *TAG = "MAIN";

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

    // button_manager_init();

    display.init();

    display.display1("Sink or be Sunk");
    display.display2("Enter Coords:");

    // while (1)
    // {
    //     spi_display1(oled, disp1);
    //     spi_display2(oled, disp2);
    //     key = begin_scan_keypad();
    //     memcpy(disp1, "You Entered:    ", 16);
    //     memcpy(disp2, "                ", 16);
    //     memcpy(disp2, &key, 1);
    //     // strcpy(disp1, "You entered:");
    //     // strcpy(disp2, &key);
    // }

    // wifi_stop();
}