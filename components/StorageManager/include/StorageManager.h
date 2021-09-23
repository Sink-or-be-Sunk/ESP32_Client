#pragma once

#include <stdio.h>
#include <nvs_flash.h>
#include <stdint.h>
#include <esp_log.h>
#include <string.h>
namespace SETTING_STR_LEN
{
    constexpr int USERNAME = 32;
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

    Settings();

    void unset(const char *key);
    void save();
};
