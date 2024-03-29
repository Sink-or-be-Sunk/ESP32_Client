#pragma once

#include "cJSON.h"
#include <string.h>
#include <stdio.h>
#include "esp_log.h"
#include <esp_wifi.h>
#include "Settings.h"
#include "GameState.h"
#include "ShipManager.h"
#include "System.h"

// TODO: CREATE NAMESPACE FOR ALL THESE ENUMS TO AVOID NAME COLLISIONS
enum GAME_REQ_TYPE
{
    NEW_GAME,
    MAKE_MOVE,
    POSITION_SHIPS,
    JOIN_GAME_REQ,
};

enum REGISTRATION_TYPE
{
    ENQUEUE,
    CONFIRM,
    DEQUEUE,
};

enum DATABASE_REQ_TYPE
{
    INVITE,
    GET_FRIENDS,
};

enum ATTACK_TYPE
{
    SOLO,
};

class Messenger
{
private:
    char *register_base(const char *type_str, const char *username);
    char *build_game_msg(GAME_REQ_TYPE type, cJSON *data);

public:
    void init(void);
    char *build_connected_msg(bool init);
    char *build_registration_msg(REGISTRATION_TYPE type);
    char *build_new_game_msg(void);
    char *build_attack_msg(char r, char c, ATTACK_TYPE type, const char *to);
    char *build_join_game_msg(const char *game);
    char *build_position_ships(void);
    char *build_db_msg(DATABASE_REQ_TYPE type, const char *username);
    char *build_leave_game_msg();
};

extern Messenger messenger;