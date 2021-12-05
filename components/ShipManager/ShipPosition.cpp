#include "ShipPosition.h"

static const char *TAG = "SHIP_POS";

void ShipPosition::position(uint8_t r1, uint8_t c1, uint8_t r2, uint8_t c2)
{
    this->front_r = r1;
    this->back_r = r2;
    this->front_c = c1;
    this->back_c = c2;
    this->isReady = true;
}

bool ShipPosition::remove(int row, int col, int *row_pair, int *col_pair)
{
    bool valid = false;
    if (this->front_r == row && this->back_c == col)
    {
        this->isReady = false;
        *row_pair = this->back_r;
        *col_pair = this->back_c;
        valid = true;
    }
    else if (this->back_r == row && this->back_r == col)
    {
        this->isReady = false;
        *row_pair = this->front_r;
        *col_pair = this->front_c;
        valid = true;
    }
    ESP_LOGW(TAG, "Removed Ship: %d", this->type);
    return valid;
}

void ShipPosition::get(uint8_t *r1, uint8_t *c1, uint8_t *r2, uint8_t *c2)
{
    *r1 = this->front_r;
    *c1 = this->front_c;
    *r2 = this->back_r;
    *c2 = this->back_c;
}