#include "GameState.h"

GameState gameState; // singleton instance of class

void GameState::init(void)
{
    strcpy(this->opponent, "OPPONENT ERROR");
}