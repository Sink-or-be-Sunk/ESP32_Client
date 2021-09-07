#pragma once

#include <string.h>

#include "esp_wifi.h"
#include "esp_log.h"
#include "esp_websocket_client.h"
#include "esp_event.h"
#include "esp_system.h"

#include <wifi_provisioning/manager.h>
#include <wifi_provisioning/scheme_ble.h>

#include "WebSocketMsg.h"

/* comment to save configurations on reboot */
#define RESET_PROVISIONED

void websocket_app_start(void);

int wifi_send(char *msg);

void wifi_stop(void);

void wifi_provision(void);