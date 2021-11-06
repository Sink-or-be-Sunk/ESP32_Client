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

void GameState::moveReceived(int c, int r, char *to, char result)
{
    // TODO: NEED TO ADD SOMETHING HERE FOR DEALING WITH SUNK SHIPS
    printf("result: %c\n", result);
    if (strcmp(settings.username, to))
    {
        // confirming move you just made
        printf("Attack Board Updated\n");

        this->attackCoords[0] = c; // TODO: THIS IS PROBABLY REDUNDANT, REMOVE
        this->attackCoords[1] = r;
        this->myTurn = false;
    }
    else
    {
        // move from opponent
        printf("Opponent Board updated\n");

        this->opponentAttackCoords[0] = c;
        this->opponentAttackCoords[1] = r;
        this->myTurn = true;
    }
}
