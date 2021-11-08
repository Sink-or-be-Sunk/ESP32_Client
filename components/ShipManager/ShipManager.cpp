#include "ShipManager.h"

static const char *TAG = "SHIPS";

// #define RESCAN_DELAY_MS 100
#define RESCAN_DELAY_MS 2000

#define BOARD_WIDTH 8

#define MUX_ROW_SEL_0 GPIO_NUM_18
#define MUX_ROW_SEL_1 GPIO_NUM_19
#define MUX_ROW_SEL_2 GPIO_NUM_21
// #define MUX_COL_SEL_0 GPIO_NUM_32
#define MUX_COL_SEL_0 GPIO_NUM_30
#define MUX_COL_SEL_1 GPIO_NUM_26
// #define MUX_COL_SEL_2 GPIO_NUM_27
#define MUX_COL_SEL_2 GPIO_NUM_31
// #define BOAT_INPUT GPIO_NUM_5
#define BOAT_INPUT GPIO_NUM_13

#define GPIO_BOAT_INPUT_MASK (1ULL << BOAT_INPUT)

#define GPIO_MUX_SEL_MASK ((1ULL << MUX_ROW_SEL_0) | \
                           (1ULL << MUX_ROW_SEL_1) | \
                           (1ULL << MUX_ROW_SEL_2) | \
                           (1ULL << MUX_COL_SEL_0) | \
                           (1ULL << MUX_COL_SEL_1) | \
                           (1ULL << MUX_COL_SEL_2))

ShipManager shipManager; // singleton instance of class

bool ShipManager::addPosition(int row, int col)
{
    if (this->fullShip)
    {
        // have two positions, detect ships
        int rowInLine = row - prevRow;
        int colInLine = col - prevCol;
        int dist;
        if (rowInLine == 0)
        {
            // vertical boat
            dist = col - prevCol;
            if (dist < 0)
            {
                dist *= -1;
            }
        }
        else if (colInLine)
        {
            // horizontal boat
            dist = row - prevRow;
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

        dist -= 2; // normalize with ship_position_t enum
        if (dist > CARRIER || dist < PATROL)
        {
            ESP_LOGE(TAG, "Invalid ship size detected!");
            return false;
        }
        this->ships[dist].front_r = row;
        this->ships[dist].back_r = prevRow;
        this->ships[dist].front_c = col;
        this->ships[dist].back_r = prevCol;
        this->ships[dist].isReady = true;

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

static void ship_detect_task(void *args)
{
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

                if (shipManager.filled & mask)
                {
                    continue; // already filled
                }

                gpio_set_level(MUX_ROW_SEL_0, c & 0x1);
                gpio_set_level(MUX_ROW_SEL_1, c & 0x2);
                gpio_set_level(MUX_ROW_SEL_2, c & 0x4);

                if (gpio_get_level(BOAT_INPUT))
                {
                    if (!shipManager.addPosition(r, c))
                    {
                        // TODO: HANDLE ERRORS HERE
                    }
                    ESP_LOGI(TAG, "Boat Detected");
                    shipManager.filled |= mask;
                }

                vTaskDelay(pdMS_TO_TICKS(10)); // allow time for signal to settle in mux
            }
        }

        vTaskDelay(pdMS_TO_TICKS(RESCAN_DELAY_MS));
    }
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

void ShipManager::updateShip(ship_position_t type, uint8_t r1, uint8_t c1, uint8_t r2, uint8_t c2)
{
    this->ships[type].position(r1, c1, r2, c2);
}

void ShipManager::getShip(ship_position_t type, uint8_t *r1, uint8_t *c1, uint8_t *r2, uint8_t *c2)
{
    ShipPosition pos = this->ships[type];
    *r1 = pos.front_r;
    *c1 = pos.front_c;
    *r2 = pos.back_r;
    *c2 = pos.back_c;
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