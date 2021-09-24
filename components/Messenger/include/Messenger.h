#pragma once

#include "cJSON.h"
#include <string.h>
#include <stdio.h>
#include <esp_wifi.h>
#include "Settings.h"

/** This comes for ESP IDF definitions*/
#define SSID_MAX_LEN 33
#define DEVICE_ID_MAX_LEN 13

enum GAME_REQ_TYPE
{
    NEW_GAME
};

enum REGISTRATION_TYPE
{
    ENQUEUE,
    CONFIRM,
};

class Messenger
{
private:
    char device_id[DEVICE_ID_MAX_LEN];
    char device_ssid[SSID_MAX_LEN];
    char *register_base(const char *type_str, const char *username);

public:
    void init(void);
    char *build_registration_msg(REGISTRATION_TYPE type);
    char *build_game_msg(GAME_REQ_TYPE type);
};

extern Messenger messenger;