#pragma once
#include <string.h>
#include "ShipManager.h"
#include "Settings.h"

#define RESULT_HIT 'H'
#define RESULT_MISS 'M'
#define RESULT_SUNK 'S'

#define MOVE_RESULT_BUFFER_SIZE 5

enum game_state_t
{
    SETUP,      // pair device, create game, join game
    LOBBY,      // invite friend, position ships, game type
    IN_PROGRESS // make move
};

class GameState
{
private:
    int8_t attackSel; // index of selected element
public:
    // TODO: MOVE COORDS AND BOARDS TO SUBCLASS FOR BETTER ABSTRACTION
    int8_t attackCoords[2] = {0, 0};          // [col, row]
    int8_t opponentAttackCoords[2] = {0, 0};  //[col, row]
    char opponent[SETTING_STR_LEN::USERNAME]; // opposing player
    game_state_t state;
    uint16_t prev_board; // (empty, hit, miss, filled)
    uint16_t board;
    bool myTurn;                               // 1->mine, 0->theirs
    char lastMoveRes[MOVE_RESULT_BUFFER_SIZE]; // holds, HIT, MISS, SUNK

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
    void moveReceived(int c, int r, char *to, char result);
    void set_attack(int num);
};

extern GameState gameState;