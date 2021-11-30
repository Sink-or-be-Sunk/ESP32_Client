#pragma once

#include <stdio.h>
#include <nvs_flash.h>
#include <stdint.h>
#include <esp_log.h>
#include <string.h>
#include <esp_wifi.h>

#include "System.h"

namespace SETTING_STR_LEN
{
    /** These come from ESP IDF definitions*/
    constexpr int USERNAME = 32;
    constexpr int SSID = 33;
};

namespace SETTING_HEADERS
{
    constexpr char USERNAME[] = "USERNAME";
};

class Settings
{
private:
    nvs_handle_t open_handle();
    void write_str(nvs_handle_t handle, const char *key, char *val);
    void read_str(nvs_handle_t handle, const char *key, char *val, size_t *len);

public:
    char username[SETTING_STR_LEN::USERNAME];
    char ssid[SETTING_STR_LEN::SSID];

    void init(void);
    void update(void);
    void unset(const char *key);
    void save();
};

extern Settings settings;