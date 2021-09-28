#include "ButtonManager.h"
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <freertos/task.h>
#include <freertos/event_groups.h>
#include "driver/gpio.h"
#include "Websocket.h"

// #define ROW_1_PIN GPIO_NUM_19
// #define ROW_2_PIN GPIO_NUM_18
// #define ROW_3_PIN GPIO_NUM_5
// #define ROW_4_PIN GPIO_NUM_17
// #define COL_1_PIN GPIO_NUM_16
// #define COL_2_PIN GPIO_NUM_4
// #define COL_3_PIN GPIO_NUM_0
// #define COL_4_PIN GPIO_NUM_2
#define ROW_1_PIN GPIO_NUM_15
#define ROW_2_PIN GPIO_NUM_33
#define ROW_3_PIN GPIO_NUM_27
#define ROW_4_PIN GPIO_NUM_12
#define COL_1_PIN GPIO_NUM_23
#define COL_2_PIN GPIO_NUM_22
#define COL_3_PIN GPIO_NUM_14
#define COL_4_PIN GPIO_NUM_32

#define GPIO_ROW_MASK ((1ULL << ROW_1_PIN) | \
                       (1ULL << ROW_2_PIN) | \
                       (1ULL << ROW_3_PIN) | \
                       (1ULL << ROW_4_PIN))

#define GPIO_COL_MASK ((1ULL << COL_1_PIN) | \
                       (1ULL << COL_2_PIN) | \
                       (1ULL << COL_3_PIN) | \
                       (1ULL << COL_4_PIN))

static gpio_num_t decode_row_pins[4] = {ROW_1_PIN, ROW_2_PIN, ROW_3_PIN, ROW_4_PIN};
static gpio_num_t decode_col_pins[4] = {COL_1_PIN, COL_2_PIN, COL_3_PIN, COL_4_PIN};
static char decode_letter[16] = {'1', '2', '3', 'A', '4', '5', '6', 'B', '7', '8', '9', 'C', '*', '0', '#', 'D'};

static void gpio_buttons_task(void *arg)
{
    for (;;)
    {
        bool key_pressed = false;
        char key = 0;
        for (int row_num = 0; row_num < 4 && !key_pressed; row_num++)
        {
            gpio_num_t row_to_pin = decode_row_pins[row_num];

            gpio_set_level(row_to_pin, 0);

            for (int c = 0; c < 4; c++)
            {
                if (gpio_get_level(decode_col_pins[c]) == 0)
                {
                    key = decode_letter[row_num * 4 + c];
                    key_pressed = true;
                    break;
                }
            }

            gpio_set_level(row_to_pin, 1);
            vTaskDelay(20 / portTICK_PERIOD_MS);
        }
        if (key_pressed)
        {
            printf("key pressed: %c\n", key); //TODO: ATTACH THIS TO BUTTON MANAGER CLASS TO PRODUCE BUTTON PRESSED EVENTS
        }
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}

void button_manager_init(void)
{
    // gpio_config_t io_conf;
    // io_conf = {
    //     .pin_bit_mask = GPIO_ROW_MASK,
    //     .mode = GPIO_MODE_OUTPUT,
    //     .pull_up_en = GPIO_PULLUP_DISABLE,
    //     .pull_down_en = GPIO_PULLDOWN_DISABLE,
    //     .intr_type = GPIO_INTR_DISABLE,
    // };
    // gpio_config(&io_conf);
    // io_conf = {
    //     .pin_bit_mask = GPIO_COL_MASK,
    //     .mode = GPIO_MODE_INPUT,
    //     .pull_up_en = GPIO_PULLUP_DISABLE,
    //     .pull_down_en = GPIO_PULLDOWN_DISABLE,
    //     .intr_type = GPIO_INTR_DISABLE,
    // };
    // gpio_config(&io_conf);

    // gpio_set_level(ROW_1_PIN, 1);
    // gpio_set_level(ROW_2_PIN, 1);
    // gpio_set_level(ROW_3_PIN, 1);
    // gpio_set_level(ROW_4_PIN, 1);

    gpio_reset_pin(ROW_1_PIN);
    gpio_reset_pin(ROW_2_PIN);
    gpio_reset_pin(ROW_3_PIN);
    gpio_reset_pin(ROW_4_PIN);

    gpio_set_direction(ROW_1_PIN, GPIO_MODE_OUTPUT);
    gpio_set_direction(ROW_2_PIN, GPIO_MODE_OUTPUT);
    gpio_set_direction(ROW_3_PIN, GPIO_MODE_OUTPUT);
    gpio_set_direction(ROW_4_PIN, GPIO_MODE_OUTPUT);
    gpio_set_level(ROW_1_PIN, 1);
    gpio_set_level(ROW_2_PIN, 1);
    gpio_set_level(ROW_3_PIN, 1);
    gpio_set_level(ROW_4_PIN, 1);

    //columns
    gpio_reset_pin(COL_1_PIN);
    gpio_reset_pin(COL_2_PIN);
    gpio_reset_pin(COL_3_PIN);
    gpio_reset_pin(COL_4_PIN);

    gpio_set_direction(COL_1_PIN, GPIO_MODE_INPUT);
    gpio_set_direction(COL_2_PIN, GPIO_MODE_INPUT);
    gpio_set_direction(COL_3_PIN, GPIO_MODE_INPUT);
    gpio_set_direction(COL_4_PIN, GPIO_MODE_INPUT);
    vTaskDelay(100 / portTICK_PERIOD_MS);

    //start gpio task
    xTaskCreate(gpio_buttons_task, "gpio_buttons_task", 2048, NULL, 10, NULL);
}