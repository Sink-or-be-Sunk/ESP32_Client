#include "ShipManager.h"

static const char *TAG = "SHIPS";

// #define RESCAN_DELAY_MS 100
#define RESCAN_DELAY_MS 2000

#define GPIO_BOAT_INPUT_MASK (1ULL << BOAT_INPUT)

#define GPIO_MUX_SEL_MASK ((1ULL << MUX_ROW_SEL_0) | \
                           (1ULL << MUX_ROW_SEL_1) | \
                           (1ULL << MUX_ROW_SEL_2) | \
                           (1ULL << MUX_COL_SEL_0) | \
                           (1ULL << MUX_COL_SEL_1) | \
                           (1ULL << MUX_COL_SEL_2))

ShipManager shipManager; // singleton instance of class

static void set_row(int r)
{
    gpio_set_level(MUX_ROW_SEL_0, r & 0x1);
    gpio_set_level(MUX_ROW_SEL_1, r & 0x2);
    gpio_set_level(MUX_ROW_SEL_2, r & 0x4);
    vTaskDelay(pdMS_TO_TICKS(10)); // allow time for signal to settle in mux
}

static void set_col(int c)
{
    gpio_set_level(MUX_COL_SEL_0, c & 0x1);
    gpio_set_level(MUX_COL_SEL_1, c & 0x2);
    gpio_set_level(MUX_COL_SEL_2, c & 0x4);
    vTaskDelay(pdMS_TO_TICKS(10)); // allow time for signal to settle in mux
}

static void ship_detect_task(void *args)
{
#if defined(HARDCODE_SHIP_POSITIONS)
    ESP_LOGW(TAG, "Ship positions set to developement values");
    int hc_row[] = {0, 1, 6, 6, 1, 4, 0, 4};
    int hc_col[] = {6, 6, 6, 4, 3, 3, 0, 0};
    int ships = 4 * 2;
    for (int i = 0; i < ships; i++)
    {
        shipManager.addPosition(hc_row[i], hc_col[i]);
    }
    for (;;)
    {
        vTaskDelay(pdMS_TO_TICKS(1000)); // RTOS task cannot return
    }
#elif defined(DEBUG_SHIP_POSITIONS)
    ESP_LOGW(TAG, "Monitoring In Debug Mode");
    for (;;)
    {
        set_row(shipManager.row);
        set_col(shipManager.col);

        if (gpio_get_level(BOAT_INPUT))
        {
            ESP_LOGI(TAG, "c:%d, r:%d = 1", shipManager.col, shipManager.row);
        }
        else
        {
            ESP_LOGE(TAG, "c:%d, r:%d = 0", shipManager.col, shipManager.row);
        }

        vTaskDelay(pdMS_TO_TICKS(250));
    }
#else
    // Production Opertion Behavior
    for (;;)
    {
        for (int r = 0; r < BOARD_WIDTH; r++)
        {
            set_row(r);

            for (int c = 0; c < BOARD_WIDTH; c++)
            {
                int filled = shipManager.isFilled(r, c);

                set_col(c);

                // ESP_LOGI(TAG, "r:%d, c:%d, filled: %d", r, c, filled);

                if (gpio_get_level(BOAT_INPUT))
                {
                    if (filled)
                    {
                        continue; // already detected
                    }
                    ESP_LOGW(TAG, "Position Detected: r:%d, c:%d", r, c);
                    if (shipManager.addPosition(r, c))
                    {
                        // valid add
                        shipManager.fill(r, c);
                    }
                }
                else if (filled)
                {
                    // position has been removed
                    ESP_LOGW(TAG, "Removing Position r:%d,c:%d", r, c);
                    shipManager.removePosition(r, c);
                    shipManager.clear(r, c);
                }
            }
        }

        shipManager.notify_leds();
        vTaskDelay(pdMS_TO_TICKS(RESCAN_DELAY_MS));
    }
#endif
}

void ShipManager::init(void)
{
    ESP_LOGI(TAG, "Initializing...");

    for (int r = 0; r < BOARD_WIDTH; r++)
    {
        for (int c = 0; c < BOARD_WIDTH; c++)
        {
            this->clear(r, c);
        }
    }
    this->fullShip = false;
    this->prevRow = -1;
    this->prevCol = -1;
    for (int i = PATROL; i <= CARRIER; i++)
    {
        this->ships[i].init();
    }

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

void ShipManager::fill(int r, int c)
{
    this->filled[r][c] = true;
}

void ShipManager::clear(int r, int c)
{
    this->filled[r][c] = false;
}

bool ShipManager::isFilled(int r, int c)
{
    return this->filled[r][c];
}

void ShipManager::notify_leds(void)
{
    char str[NUMBER_OF_LEDS];

    // build blank led string
    for (int i = 0; i < NUMBER_OF_LEDS; i++)
    {
        str[i] = led_position_t::EMPTY;
    }

    // add positioned ships
    for (int i = PATROL; i <= CARRIER; i++)
    {
        uint8_t r1;
        uint8_t r2;
        uint8_t c1;
        uint8_t c2;
        if (this->getShip((ship_position_t)i, &r1, &c1, &r2, &c2))
        {
            str[r1 * BOARD_WIDTH + c1] = led_position_t::POSITION;
            str[r2 * BOARD_WIDTH + c2] = led_position_t::POSITION;
        }
    }

    // add partial ship
    if (!this->fullShip)
    {
        int r = BOARD_WIDTH - 1 - this->prevRow;
        int c = this->prevCol;
        str[r * BOARD_WIDTH + c] = led_position_t::POSITION;
    }

    ledManager.update(str);
}

/**
 * Adds position to queue (size two).  Once queue
 * is full, the two points are analyzed to find the ship
 * size.  If the size is valid, the ships array is updated
 *
 * @param row row to add
 * @param col col to add
 * @returns - true if the position can be added, false
 * if the position causes some error when adding
 */
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
        }
        else if (colInLine == 0)
        {
            // horizontal boat
            dist = row - this->prevRow;
        }
        else
        {
            ESP_LOGE(TAG, "Invalid set of positions detected!");
            return false;
        }

        if (dist < 0)
        {
            dist *= -1;
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
            return false;
        }

        this->ships[dist].position(row, col, this->prevRow, this->prevCol);

        this->fullShip = false; // reset queue
        this->prevRow = -1;
        this->prevCol = -1;
        ESP_LOGW(TAG, "Ship Added: %d (r:%d,c:%d), (r:%d,c:%d)", dist, row, col, this->prevRow, this->prevCol);

        screenManager.conditionalRender(READY_UP_SHIPS);

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

/**
 * Removes ship at given row,col pair if present
 * Does nothing otherwise
 * @param row row of ship to remove
 * @param col col of ship to remove
 */
void ShipManager::removePosition(int row, int col)
{
    this->fullShip = false; // reset queue
    this->prevRow = -1;
    this->prevCol = -1;

    for (int i = PATROL; i <= CARRIER; i++)
    {
        ShipPosition ship = this->ships[i];
        if (ship.remove(row, col))
        {
            ESP_LOGW(TAG, "Removed Ship <%d> Position r:%d,c:%d", i, row, col);
            return;
        }
    }
    ESP_LOGE(TAG, "Removed Position r:%d,c:%d", row, col);
}

bool ShipManager::getShip(ship_position_t type, uint8_t *r1, uint8_t *c1, uint8_t *r2, uint8_t *c2)
{
    ShipPosition pos = this->ships[type];
    *r1 = pos.front_r; // convert to coordinates used by server
    *c1 = pos.front_c;
    *r2 = pos.back_r; // convert to coordinates used by server
    *c2 = pos.back_c;
    return pos.isReady;
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
    }
    return count;
}

#ifdef DEBUG_SHIP_POSITIONS

void ShipManager::right(void)
{
    shipManager.col++;
    if (shipManager.col > BOARD_WIDTH - 1)
    {
        shipManager.col = BOARD_WIDTH - 1;
    }
}
void ShipManager::left(void)
{
    shipManager.col--;
    if (shipManager.col < 0)
    {
        shipManager.col = 0;
    }
}
void ShipManager::down(void)
{
    shipManager.row--;
    if (shipManager.row < 0)
    {
        shipManager.row = 0;
    }
}
void ShipManager::up(void)
{
    shipManager.row++;
    if (shipManager.row > BOARD_WIDTH - 1)
    {
        shipManager.row = BOARD_WIDTH - 1;
    }
}

#endif