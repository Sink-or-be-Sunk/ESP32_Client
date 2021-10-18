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
    int8_t attack_row;
    int8_t attack_col;

public:
    char opponent[USERNAME_MAX_LEN]; // opposing player
    game_state_t state;

    void init(void);
    void next_state(void);
    void increment_attack_row(void);
    void increment_attack_col(void);
    void decrement_attack_row(void);
    void decrement_attack_col(void);
    uint8_t get_attack_row(void);
    uint8_t get_attack_col(void);
};

extern GameState gameState;