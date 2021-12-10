#pragma once

#include <stdint.h>
#include "esp_log.h"

class ShipPosition
{
private:
public:
    bool isReady;
    int front_r;
    int front_c;
    int back_r;
    int back_c;

    void init();
    bool remove(int row, int col);

    void position(int r1, int c1, int r2, int c2);
};