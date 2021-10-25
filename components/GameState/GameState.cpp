#include "GameState.h"

GameState gameState; // singleton instance of class

void GameState::init(void)
{
    this->attackSel = 0;
    strcpy(this->opponent, "OPPONENT ERROR");
    this->state = SETUP;
}

void GameState::setState(game_state_t state)
{
    this->state = state;
}

void GameState::increment_attack(void)
{
    int8_t i = this->attackSel;
    this->attackCoords[i]++;
    if (this->attackCoords[i] >= 8)
    {
        this->attackCoords[i] = 7;
    }
}

void GameState::decrement_attack(void)
{
    int8_t i = this->attackSel;
    this->attackCoords[i]--;
    if (this->attackCoords[i] < 0)
    {
        this->attackCoords[i] = 0;
    }
}

void GameState::request_left_sel_attack(void)
{
    this->attackSel--;
    if (this->attackSel < 0)
    {
        this->attackSel = 0;
    }
}

void GameState::request_right_sel_attack(void)
{
    this->attackSel++;
    if (this->attackSel > 1)
    {
        this->attackSel = 1;
    }
}

uint8_t GameState::get_attack_row(void)
{
    return this->attackCoords[1];
}

uint8_t GameState::get_attack_col(void)
{
    return this->attackCoords[0];
}

char GameState::get_attack_col_tag(void)
{
    return (this->attackSel == 0) ? 'C' : 'c';
}

char GameState::get_attack_row_tag(void)
{
    return (this->attackSel == 1) ? 'R' : 'r';
}

void GameState::updateShip(ship_position_t type, uint8_t r1, uint8_t c1, uint8_t r2, uint8_t c2)
{
    this->ship_layout[type].position(r1, c1, r2, c2);
}

void GameState::getShip(ship_position_t type, uint8_t *r1, uint8_t *c1, uint8_t *r2, uint8_t *c2)
{
    ShipPosition pos = this->ship_layout[type];
    *r1 = pos.front_r;
    *c1 = pos.front_c;
    *r2 = pos.back_r;
    *c2 = pos.back_c;
}

void GameState::moveReceived(int c, int r, char *to, char result)
{
    // TODO: NEED TO ADD SOMETHING HERE FOR DEALING WITH SUNK SHIPS
    printf("result: %c\n", result);
    if (strcmp(settings.username, to))
    {
        // confirming move you just made
        printf("Attack Board Updated\n");

        this->attackBoard[c * BOARD_WIDTH + r] = result;
        this->attackCoords[0] = c; // TODO: THIS IS PROBABLY REDUNDANT
        this->attackCoords[1] = r;
        this->myTurn = false;
    }
    else
    {
        // move from opponent
        printf("Opponent Board updated\n");

        this->opponentAttackCoords[0] = c;
        this->opponentAttackCoords[1] = r;
        this->positionBoard[c * BOARD_WIDTH + r] = result;
        this->myTurn = true;
    }
}
