#pragma once

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "driver/gpio.h"

char begin_scan_keypad();

void init_keypad();