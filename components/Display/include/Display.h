#pragma once

#include <string.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "driver/gpio.h"
#include "driver/spi_common.h"
#include "driver/spi_master.h"
#include "esp_log.h"
#include "System.h"

class Display
{
private:
    spi_device_handle_t handle;
    void send_cmd(const uint8_t cmd);
    void send_data(const uint8_t cmd);
    void init_oled(void);

public:
    void init(void);
    void clear(void);
    void display1(const char *str);
    void display2(const char *str);
};

extern Display display;