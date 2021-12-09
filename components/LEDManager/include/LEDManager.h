#pragma once

#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/rmt.h"
#include "led_strip.h"
#include "System.h"

#define NUMBER_OF_LEDS 128

namespace led_position_t
{
    constexpr char EMPTY = 'E';
    constexpr char FULL = 'F';
    constexpr char HIT = 'H';
    constexpr char MISS = 'M';
    constexpr char SUNK = 'S';
    constexpr char PENDING = 'P';
}

class LEDManager
{
private:
    TaskHandle_t handle;

public:
    void init(void);
    void start(void);
    void pause(void);
    void update(const char *str);
};

extern LEDManager ledManager;