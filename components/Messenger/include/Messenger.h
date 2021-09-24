#pragma once

#include "cJSON.h"
#include <string.h>
#include <stdio.h>
#include <esp_wifi.h>
#include "Settings.h"

/** This comes for ESP IDF definitions*/
#define SSID_MAX_LEN 33
#define DEVICE_ID_MAX_LEN 13

enum CLIENT_GAME_HEADER
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
};

extern Messenger messenger;

// FIXME: NEED TO CLEAN UP EVERYTHING BELOW THIS LINE
// typedef struct ships
// {
//     uint8_t type;
// } ship;
// namespace game_type
// {
//     constexpr char CLASSIC[] = "CLASSIC";
//     constexpr char BASIC[] = "BASIC";
// }
// namespace move_type
// {
//     constexpr char SOLO[] = "SOLO";
//     constexpr char SALVO[] = "SALVO";
// }

// typedef enum
// {
//     SOLO,
//     SALVO,
//     //...idk something else
// } move_type_t;

char *create_new_game_req(void);

char *make_move(uint8_t c, uint8_t r);
// char *make_move(uint8_t c, uint8_t r, type, at); //TODO: ADD GAME TYPE AND MORE THAN 2 PLAYER GAMES

//FIXME: NEED TO UPDATE THESE WITH ARGUMENTS
char *get_friends_list(void);

char *register_board(void);

char *confirm_register(void);

char *join_game(void);

char *set_game_type(void);

// char *position_ship(uint8_t c, uint8_t r, ship positions[5]);
// char *position_ship(uint8_t c, uint8_t r, ship *positions);