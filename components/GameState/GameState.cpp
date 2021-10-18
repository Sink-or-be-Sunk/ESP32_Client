#include "GameState.h"

GameState gameState; // singleton instance of class

void GameState::init(void)
{
    strcpy(this->opponent, "OPPONENT ERROR");
    this->state = SETUP;
}

void GameState::next_state(void)
{
    switch (this->state)
    {
    case SETUP:
    {
        this->state = LOBBY;
        break;
    }
    case LOBBY:
    {
        this->state = IN_PROGRESS;
        break;
    }
    case IN_PROGRESS:
    {
        this->state = SETUP;
        break;
    }
    }
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
