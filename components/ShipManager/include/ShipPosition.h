#pragma once

#include <stdint.h>

enum ship_position_t
{
    INVALID = -1,
    PATROL = 0,
    SUBMARINE = 1,
    BATTLESHIP = 2,
    CARRIER = 3,
};

class ShipPosition
{
private:
    ship_position_t type = INVALID;

public:
    uint8_t front_r;
    uint8_t front_c;
    uint8_t back_r;
    uint8_t back_c;

    void position(uint8_t r1, uint8_t c1, uint8_t r2, uint8_t c2);
};