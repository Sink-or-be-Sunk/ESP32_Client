#include "ShipPosition.h"

static const char *TAG = "SHIP_POS";

void ShipPosition::init()
{
    this->front_r = -1;
    this->front_c = -1;
    this->back_r = -1;
    this->back_c = -1;
    this->isReady = false;
}

void ShipPosition::position(uint8_t r1, uint8_t c1, uint8_t r2, uint8_t c2)
{
    this->front_r = r1;
    this->back_r = r2;
    this->front_c = c1;
    this->back_c = c2;
    this->isReady = true;
}

bool ShipPosition::remove(int row, int col)
{
    bool valid = false;
    bool isFront = this->front_r == row && this->front_c == col;
    bool isBack = this->back_r == row && this->back_c == col;

    if (isFront || isBack)
    {
        this->init();
        valid = true;
    }
    return valid;
}