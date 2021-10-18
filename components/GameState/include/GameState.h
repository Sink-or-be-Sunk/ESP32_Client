#pragma once
#include <string.h>

#define USERNAME_MAX_LEN 33

enum game_state_t
{
    SETUP,      // pair device, create game, join game
    LOBBY,      // invite friend, position ships, game type
    IN_PROGRESS // make move
};

class GameState
{
private:
    int8_t attackSel;                // index of selected element
    int8_t attackCoords[2] = {0, 0}; //[col, row]

public:
    char opponent[USERNAME_MAX_LEN]; // opposing player
    game_state_t state;

    void init(void);
    void setState(game_state_t state);
    void increment_attack(void);
    void decrement_attack(void);
    void request_right_sel_attack(void);
    void request_left_sel_attack(void);
    uint8_t get_attack_row(void);
    uint8_t get_attack_col(void);
    char get_attack_row_tag(void);
    char get_attack_col_tag(void);
};

extern GameState gameState;