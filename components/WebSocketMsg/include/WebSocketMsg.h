#pragma once

#include "cJSON.h"
#include <string.h>
#include <stdio.h>
#include <esp_wifi.h>

/** This comes for ESP IDF definitions*/
#define SSID_MAX_LEN 33

char *register_enqueue(void);

char *register_confirm(void);

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