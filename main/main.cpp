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

#include "System.h"
#include "Websocket.h"
#include "ButtonManager.h"
#include "WifiManager.h"
#include "Settings.h"
#include "Display.h"
#include "ScreenManager.h"
#include "LEDManager.h"
#include "GameState.h"
#include "ShipManager.h"
#include "MotorManager.h"

static const char *TAG = "MAIN";

extern "C" void app_main(void)
{
    ESP_LOGI(TAG, "Starting App Main");

    // init display spi drivers
    display.init();

    // need to init display before screen manager
    screenManager.init();

    /* Initializes nvs (needed for wifi prov too!) */
    settings.init();

    /* Initialize the event loop */
    ESP_ERROR_CHECK(esp_event_loop_create_default()); // FIXME: IS THIS NEEDED

    ledManager.init();

    buttonManager.init();

    wifiManager.init();

    settings.update(); // needs to occur after wifi connection established

    gameState.init();

    shipManager.init();

    motorManager.init();

    websocket.start();

    screenManager.splash(HOME, CREATE_GAME);

    // wifi_stop(); //FIXME: REMOVE THIS, NEVER WILL BE USED
}