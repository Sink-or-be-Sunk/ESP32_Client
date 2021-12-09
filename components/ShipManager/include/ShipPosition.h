#pragma once

#include <stdint.h>

class ShipPosition
{
private:
public:
    bool isReady = 0;
    uint8_t front_r;
    uint8_t front_c;
    uint8_t back_r;
    uint8_t back_c;

    void init();
    bool remove(int row, int col);

    void position(uint8_t r1, uint8_t c1, uint8_t r2, uint8_t c2);
};