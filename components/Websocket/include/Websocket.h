#pragma once

#include <string.h>
#include <map>

#include "esp_wifi.h"
#include "esp_log.h"
#include "esp_websocket_client.h"
#include "esp_event.h"
#include "esp_system.h"

#include <wifi_provisioning/manager.h>
#include <wifi_provisioning/scheme_ble.h>

#include "WebSocketMsg.h"

// #define CONFIG_WEBSOCKET_URI "ws://sink-or-be-sunk.herokuapp.com"

class Websocket
{
private:
    esp_websocket_client_handle_t client;

public:
    void start(void);
    void stop(void);
    int send(char *msg);
    void handle(const char *msg, uint8_t len);
};

extern Websocket websocket;