#pragma once

#include "ShipPosition.h"

#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <freertos/task.h>
#include "esp_log.h"
#include "driver/gpio.h"

#include "System.h"
#include "ScreenManager.h"
#include "LEDManager.h"

#define NUM_SHIPS 4

class ShipManager
{
private:
    ShipPosition ships[NUM_SHIPS]; // NOTE: indexed by ship_position_t
    bool fullShip;
    uint8_t prevRow;
    uint8_t prevCol;

public:
#ifdef DEBUG_SHIP_POSITIONS
    int row;
    int col;
    void up(void);
    void down(void);
    void left(void);
    void right(void);
#endif
    uint64_t filled = 0; // each bit coresponds to spot on board
    void init();
    bool addPosition(int row, int col);
    void updateShip(ship_position_t type, uint8_t r1, uint8_t c1, uint8_t r2, uint8_t c2);
    bool getShip(ship_position_t type, uint8_t *r1, uint8_t *c1, uint8_t *r2, uint8_t *c2);
    bool isReady();
    int shipsRemaining();
    void notify_leds();
};

extern ShipManager shipManager;