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
#define BOARD_WIDTH 8

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

class ShipManager
{
private:
    TaskHandle_t handle;
    ShipPosition ships[NUM_SHIPS]; // NOTE: indexed by ship_position_t
    bool fullShip;
    int prevRow;
    int prevCol;
    bool filled[BOARD_WIDTH][BOARD_WIDTH];

public:
#ifdef DEBUG_SHIP_POSITIONS
    int row;
    int col;
    void up(void);
    void down(void);
    void left(void);
    void right(void);
#endif
    void init();
    bool addPosition(int row, int col);
    void removePosition(int row, int col);
    bool getShip(ship_position_t type, int *r1, int *c1, int *r2, int *c2);
    bool isReady();
    int shipsRemaining();
    void notify_leds();
    bool isFilled(int r, int c);
    void fill(int r, int c);
    void clear(int r, int c);
    void stopChecking();
};

extern ShipManager shipManager;