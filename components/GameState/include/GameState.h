#pragma once
#include <string.h>

#define USERNAME_MAX_LEN 33

class GameState
{
public:
    char opponent[USERNAME_MAX_LEN]; // opposing player
    void init(void);
};

extern GameState gameState;