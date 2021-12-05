#pragma once

#include <stdint.h>
#include "esp_log.h"

/**
 * Ship Position and Index in ShipManager Array
 */
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
    uint8_t front_r;
    uint8_t front_c;
    uint8_t back_r;
    uint8_t back_c;

public:
    bool isReady = 0;

    void position(uint8_t r1, uint8_t c1, uint8_t r2, uint8_t c2);
    bool remove(int row, int col, int *row_pair, int *col_pair);
    void get(uint8_t *r1, uint8_t *c1, uint8_t *r2, uint8_t *c2);
};