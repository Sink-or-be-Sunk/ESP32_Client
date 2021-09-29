#pragma once

#include "neopixel.h"

class LEDManager
{
private:
    Pixels *pixels;

public:
    void init(void);
};

extern LEDManager ledManager;