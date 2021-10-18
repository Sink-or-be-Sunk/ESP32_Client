#include "GameState.h"

GameState gameState; // singleton instance of class

void GameState::init(void)
{
    strcpy(this->opponent, "OPPONENT ERROR");
    this->state = SETUP;
}

void GameState::setState(game_state_t state)
{
    this->state = state;
}

void GameState::increment_attack_row(void)
{
    this->attack_row++;
    if (this->attack_row >= 8)
    {
        this->attack_row = 7;
    }
}

void GameState::increment_attack_col(void)
{
    this->attack_col++;
    if (this->attack_col >= 8)
    {
        this->attack_col = 7;
    }
}

void GameState::decrement_attack_row(void)
{
    this->attack_row--;
    if (this->attack_row < 0)
    {
        this->attack_row = 0;
    }
}

void GameState::decrement_attack_col(void)
{
    this->attack_col--;
    if (this->attack_col < 0)
    {
        this->attack_col = 0;
    }
}

uint8_t GameState::get_attack_row(void)
{
    return this->attack_row;
}

uint8_t GameState::get_attack_col(void)
{
    return this->attack_col;
}
