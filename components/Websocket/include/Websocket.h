#pragma once

#include <string.h>
#include <map>

#include "esp_wifi.h"
#include "esp_log.h"
#include "esp_websocket_client.h"
#include "esp_event.h"
#include "esp_system.h"

#include <freertos/FreeRTOS.h>
// #include <freertos/queue.h>
#include <freertos/task.h>

#include <wifi_provisioning/manager.h>
#include <wifi_provisioning/scheme_ble.h>

#include "System.h"
#include "Messenger.h"
#include "GameState.h"
#include "ScreenManager.h"
#include "FriendManager.h"
#include "LEDManager.h"
#include "MotorManager.h"
#include "ShipManager.h"

class Websocket
{
private:
    esp_websocket_client_handle_t client;
    int dropped;

public:
    void start(void);
    void stop(void);
    void send(char *msg);
    void sendQuiet(char *msg);
    void handle(const char *msg, uint8_t len);
};

extern Websocket websocket;