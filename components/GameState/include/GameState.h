#pragma once
#include <string.h>
#include "ShipPosition.h"
#include "Settings.h"

#define RESULT_HIT 'H'
#define RESULT_MISS 'M'
#define RESULT_SUNK 'S'

#define BOARD_WIDTH 8
#define BOARD_SIZE (BOARD_WIDTH * BOARD_WIDTH)

enum game_state_t
{
    SETUP,      // pair device, create game, join game
    LOBBY,      // invite friend, position ships, game type
    IN_PROGRESS // make move
};

class GameState
{
private:
    int8_t attackSel;            // index of selected element
    ShipPosition ship_layout[4]; // index of ships governed by ship_position_t
public:
    int8_t attackCoords[2] = {0, 0};          // [col, row]
    char attackBoard[BOARD_SIZE];             // board holding your attacks on opponent
    int8_t opponentAttackCoords[2] = {0, 0};  //[col, row]
    char positionBoard[BOARD_SIZE];           // board holding opponent's attacks on you
    char opponent[SETTING_STR_LEN::USERNAME]; // opposing player
    game_state_t state;
    uint16_t prev_board; // (empty, hit, miss, filled)
    uint16_t board;
    bool myTurn; // 1->mine, 0->theirs

    void
    init(void);
    void setState(game_state_t state);
    void increment_attack(void);
    void decrement_attack(void);
    void request_right_sel_attack(void);
    void request_left_sel_attack(void);
    uint8_t get_attack_row(void);
    uint8_t get_attack_col(void);
    char get_attack_row_tag(void);
    char get_attack_col_tag(void);
    void updateShip(ship_position_t type, uint8_t r1, uint8_t c1, uint8_t r2, uint8_t c2);
    void getShip(ship_position_t type, uint8_t *r1, uint8_t *c1, uint8_t *r2, uint8_t *c2);
    void moveReceived(int c, int r, char *to, char result);
};

extern GameState gameState;