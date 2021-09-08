#pragma once

#include "cJSON.h"
#include <string.h>
#include <stdio.h>
#include "esp_wifi.h"

typedef struct ships
{
    uint8_t type;
} ship;
namespace game_type
{
    constexpr char CLASSIC[] = "CLASSIC";
    constexpr char BASIC[] = "BASIC";
}
namespace move_type
{
    constexpr char SOLO[] = "SOLO";
    constexpr char SALVO[] = "SALVO";
}

typedef enum
{
    SOLO,
    SALVO,
    //...idk something else
} move_type_t;
//create a monitor with a list of supported resolutions
//NOTE: Returns a heap allocated string, you are required to free it after use.
char *
create_new_game_req(void);

char *make_move(uint8_t c, uint8_t r);
// char *make_move(uint8_t c, uint8_t r, type, at); //TODO: ADD GAME TYPE AND MORE THAN 2 PLAYER GAMES

//FIXME: NEED TO UPDATE THESE WITH ARGUMENTS
char *get_friends_list(void);

char *register_board(void);

char *confirm_register(void);

char *join_game(void);

char *set_game_type(void);

// char *position_ship(uint8_t c, uint8_t r, ship positions[5]);
char *position_ship(uint8_t c, uint8_t r, ship *positions);