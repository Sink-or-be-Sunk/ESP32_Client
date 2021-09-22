#pragma once

#include <stdio.h>
#include <nvs_flash.h>
#include <stdint.h>
#include <esp_log.h>

// #define REGISTER_TAG "isRegistered"

namespace SETTING_STR_LEN
{
    constexpr int USERNAME = 32;
};

namespace SETTING_HEADERS
{
    constexpr char USERNAME[] = "USERNAME";
};

void storage_manager_init(void);

void nvs_read(const char *key, char *val, size_t *len);

void nvs_write(const char *key, char *val);
