#pragma once

#include <wifi_provisioning/manager.h>
#include <wifi_provisioning/scheme_ble.h>
#include "qrcode.h"

#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <freertos/task.h>
#include <freertos/event_groups.h>
#include <esp_system.h>
#include <nvs.h>
#include <esp_log.h>
#include <esp_wifi.h>
#include <string.h>

#include "System.h"

class WifiManager
{
public:
    void init(void);
};

extern WifiManager wifiManager;