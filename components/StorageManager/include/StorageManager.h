#pragma once

#include <stdio.h>
#include <nvs_flash.h>
#include <stdint.h>

#define REGISTER_TAG "isRegistered"

void storage_manager_init(void);

int8_t nvs_read_i8(const char *key);

void nvs_write_i8(const char *key, int8_t val);
