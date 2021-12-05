#include "ShipManager.h"

static const char *TAG = "SHIPS";

// #define RESCAN_DELAY_MS 100
#define RESCAN_DELAY_MS 2000

#define BOARD_WIDTH 8

#define GPIO_BOAT_INPUT_MASK (1ULL << BOAT_INPUT)

#define GPIO_MUX_SEL_MASK ((1ULL << MUX_ROW_SEL_0) | \
                           (1ULL << MUX_ROW_SEL_1) | \
                           (1ULL << MUX_ROW_SEL_2) | \
                           (1ULL << MUX_COL_SEL_0) | \
                           (1ULL << MUX_COL_SEL_1) | \
                           (1ULL << MUX_COL_SEL_2))

ShipManager shipManager; // singleton instance of class

int ship = 0; // FIXME: REMOVE THIS, FOR DEBUGGING ONLY

bool ShipManager::addPosition(int row, int col)
{
    if (this->fullShip)
    {
        // have two positions, detect ships
        int rowInLine = row - this->prevRow;
        int colInLine = col - this->prevCol;
        int dist;
        if (rowInLine == 0)
        {
            // vertical boat
            dist = col - this->prevCol;
            if (dist < 0)
            {
                dist *= -1;
            }
        }
        else if (colInLine == 0)
        {
            // horizontal boat
            dist = row - this->prevRow;
            if (dist < 0)
            {
                dist *= -1;
            }
        }
        else
        {
            ESP_LOGE(TAG, "Invalid set of positions detected!");
            return false;
        }

        dist -= 1; // normalize with ship_position_t enum
        if (dist > CARRIER || dist < PATROL)
        {
            ESP_LOGE(TAG, "Invalid ship size detected!");
            return false;
        }
        if (this->ships[dist].isReady)
        {
            ESP_LOGE(TAG, "Repeat of ship shize detected: %d!", dist);
            this->prevCol = 0;
            this->prevRow = ship;
            ESP_LOGE(TAG, "Setting: c: %d, r: %d", prevCol, prevRow);
            return false;
        }
        // this->ships[dist].front_r = row;
        // this->ships[dist].back_r = this->prevRow;
        // this->ships[dist].front_c = col;
        // this->ships[dist].back_c = this->prevCol;
        // this->ships[dist].isReady = true;
        this->ships[dist].position(row, col, this->prevRow, this->prevCol);
        this->fullShip = false;
        ship++;
        ESP_LOGI(TAG, "Ship Added: %d", dist);

        screenManager.conditionalRender(READY_UP_SHIPS); // re-render READY_UP_SHIPS screen

        return true;
    }
    else
    {
        this->prevRow = row;
        this->prevCol = col;
        this->fullShip = true;
        return true;
    }
}

void ShipManager::removePosition(int row, int col, int *row_pair, int *col_pair)
{
    for (int i = PATROL; i <= CARRIER; i++)
    {
        ShipPosition ship = shipManager.ships[i];
        if (ship.remove(row, col, row_pair, col_pair))
        {
            return;
        }
    }
    ESP_LOGE(TAG, "Cannot remove position: r:%d, c:%d", row, col);
}

static void ship_detect_task(void *args)
{
#ifdef HARDCODE_SHIP_POSITIONS
    ESP_LOGW(TAG, "Ship positions set to developement values");
    int hc_row[] = {0, 0, 1, 1, 2, 2, 3, 3};
    int hc_col[] = {0, 1, 0, 2, 0, 3, 0, 4};
    int ships = 4 * 2;
    for (int i = 0; i < ships; i++)
    {
        shipManager.addPosition(hc_row[i], hc_col[i]);
    }
    for (;;)
    {
        vTaskDelay(pdMS_TO_TICKS(1000)); // RTOS task cannot return
    }
#else
    for (;;)
    {
        for (int r = 0; r < BOARD_WIDTH; r++)
        {
            gpio_set_level(MUX_ROW_SEL_0, r & 0x1);
            gpio_set_level(MUX_ROW_SEL_1, r & 0x2);
            gpio_set_level(MUX_ROW_SEL_2, r & 0x4);

            vTaskDelay(pdMS_TO_TICKS(10)); // allow time for signal to settle in mux

            for (int c = 0; c < BOARD_WIDTH; c++)
            {
                int mask = (1 << c) << (r * 8);

                gpio_set_level(MUX_COL_SEL_0, c & 0x1);
                gpio_set_level(MUX_COL_SEL_1, c & 0x2);
                gpio_set_level(MUX_COL_SEL_2, c & 0x4);

                if (gpio_get_level(BOAT_INPUT))
                {

                    if (shipManager.filled & mask)
                    {
                        // previously filled
                    }
                    else
                    {
                        ESP_LOGW(TAG, "Position Detected: (c:%d, r:%d)", c, r);
                        if (!shipManager.addPosition(r, c))
                        {
                            // FIXME: ACTUALLY HANDLE ERRORS HERE
                        }
                        shipManager.filled |= mask;
                    }
                }
                else if (shipManager.filled & mask)
                {
                    // ship has been removed
                    int rp;
                    int cp;
                    shipManager.removePosition(r, c, &rp, &cp);
                    int mask_p = (1 << cp) << (rp * 8);
                    shipManager.filled &= !mask_p;
                }

                vTaskDelay(pdMS_TO_TICKS(10)); // allow time for signal to settle in mux
            }
        }

        vTaskDelay(pdMS_TO_TICKS(RESCAN_DELAY_MS));
    }
#endif
}

void ShipManager::init(void)
{
    ESP_LOGI(TAG, "Initializing...");

    this->fullShip = false;

    gpio_config_t io_conf;
    io_conf = {
        .pin_bit_mask = GPIO_BOAT_INPUT_MASK,
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    gpio_config(&io_conf);
    io_conf = {
        .pin_bit_mask = GPIO_MUX_SEL_MASK,
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    gpio_config(&io_conf);

    // start task
    xTaskCreate(ship_detect_task, "ship_detect_task", 4096, NULL, 10, NULL);

    ESP_LOGI(TAG, "Success!");
}

// void ShipManager::updateShip(ship_position_t type, uint8_t r1, uint8_t c1, uint8_t r2, uint8_t c2)
// {
//     this->ships[type].position(r1, c1, r2, c2);
// }

void ShipManager::getShip(ship_position_t type, uint8_t *r1, uint8_t *c1, uint8_t *r2, uint8_t *c2)
{
    ShipPosition pos = this->ships[type];

    pos.get(r1, c1, r2, c2);
}

bool ShipManager::isReady()
{
    return this->shipsRemaining() == 0;
}

int ShipManager::shipsRemaining()
{
    int count = 0;
    for (int i = PATROL; i <= CARRIER; i++)
    {
        if (!shipManager.ships[i].isReady)
        {
            count++;
        }
        else
        {
            printf("ship ready: %d\n", i);
        }
    }
    return count;
}