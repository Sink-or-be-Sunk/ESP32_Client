#pragma once
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <freertos/task.h>
#include <freertos/event_groups.h>
#include "driver/gpio.h"
#include "esp_log.h"
#include "ScreenManager.h"
#include "LEDManager.h"
#include "System.h"

class ButtonManager
{
public:
    void init(void);
};

extern ButtonManager buttonManager;