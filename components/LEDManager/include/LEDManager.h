#pragma once

#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/rmt.h"
#include "led_strip.h"

class LEDManager
{
private:
    TaskHandle_t handle;

public:
    void init(void);
    void start(void);
    void pause(void);
};

extern LEDManager ledManager;