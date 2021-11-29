#pragma once
#include "cJSON.h"
#include "stdio.h"
#include "esp_log.h"
#include "Settings.h"
#include "System.h"

#define FRIEND_BUFFER_SIZE 5

class FriendManager
{
private:
    uint8_t listIdx = 0; // points to last idx + 1 after update()
    uint8_t cur = 0;     // points to current user selection
    char namesList[FRIEND_BUFFER_SIZE][SETTING_STR_LEN::USERNAME];
    char uidList[FRIEND_BUFFER_SIZE][SETTING_STR_LEN::USERNAME];

public:
    void update(const cJSON *list);
    const char *getCurDisplayName();
    const char *getCurUsername();
    /** returns false when no next allowed */
    bool next();
    /** returns false when no prev allowed */
    bool prev();
};

extern FriendManager friendManager;