#pragma once

#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <freertos/task.h>
#include "esp_log.h"
#include "driver/gpio.h"
#include "System.h"

class MotorManager
{
private:
public:
    TaskHandle_t handle;
    void init(void);
    void rumble(void);
};

extern MotorManager motorManager;