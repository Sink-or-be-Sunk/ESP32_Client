#include "ButtonManager.h"
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <freertos/task.h>
#include <freertos/event_groups.h>
#include "driver/gpio.h"
#include "Websocket.h"

#define ROW_1_PIN GPIO_NUM_19
#define ROW_2_PIN GPIO_NUM_18
#define ROW_3_PIN GPIO_NUM_5
#define ROW_4_PIN GPIO_NUM_17
#define COL_1_PIN GPIO_NUM_16
#define COL_2_PIN GPIO_NUM_4
#define COL_3_PIN GPIO_NUM_0
#define COL_4_PIN GPIO_NUM_2

#define GPIO_ROW_MASK ((1ULL << ROW_1_PIN) | \
                       (1ULL << ROW_2_PIN) | \
                       (1ULL << ROW_3_PIN) | \
                       (1ULL << ROW_4_PIN))

#define GPIO_COL_MASK ((1ULL << COL_1_PIN) | \
                       (1ULL << COL_2_PIN) | \
                       (1ULL << COL_3_PIN) | \
                       (1ULL << COL_4_PIN))

static char decipher_keypad(int row, int col)
{
    printf("Row:%d Col:%d\n", row, col);
    switch (row)
    {
    case 0:
    {
        switch (col)
        {
        case 0:
            return '1';
        case 1:
            return '2';
        case 2:
            return '3';
        case 3:
            return 'A';
        }
        break;
    }
    case 1:
        switch (col)
        {
        case 0:
            return '4';
        case 1:
            return '5';
        case 2:
            return '6';
        case 3:
            return 'B';
        }
        break;
    case 2:
        switch (col)
        {
        case 0:
            return '7';
        case 1:
            return '8';
        case 2:
            return '9';
        case 3:
            return 'C';
        }
        break;
    case 3:
        switch (col)
        {
        case 0:
            return '*';
        case 1:
            return '0';
        case 2:
            return '#';
        case 3:
            return 'D';
        }
        break;
    default:
        break;
    }
    return '\0';
}

static void gpio_buttons_task(void *arg)
{
    for (;;)
    {
        bool key_pressed = false;
        char key = 0;
        int row_num = 0;
        for (row_num = 0; row_num < 4 && !key_pressed; row_num++)
        {
            gpio_num_t row_to_pin = (gpio_num_t)0;
            switch (row_num)
            {
            case 0:
                row_to_pin = ROW_1_PIN;
                break;
            case 1:
                row_to_pin = ROW_2_PIN;
                break;
            case 2:
                row_to_pin = ROW_3_PIN;
                break;
            case 3:
                row_to_pin = ROW_4_PIN;
                break;
            default:
                printf("ERROR\n");
                break;
            }
            gpio_set_level(row_to_pin, 0);

            if (gpio_get_level(COL_1_PIN) == 0)
            {
                key = decipher_keypad(row_num, 0);
                key_pressed = true;
                vTaskDelay(100 / portTICK_PERIOD_MS);
            }
            else if (gpio_get_level(COL_2_PIN) == 0)
            {
                key = decipher_keypad(row_num, 1);
                key_pressed = true;
                vTaskDelay(100 / portTICK_PERIOD_MS);
            }
            else if (gpio_get_level(COL_3_PIN) == 0)
            {
                key = decipher_keypad(row_num, 2);
                key_pressed = true;
                vTaskDelay(100 / portTICK_PERIOD_MS);
            }
            else if (gpio_get_level(COL_4_PIN) == 0)
            {
                key = decipher_keypad(row_num, 3);
                key_pressed = true;
                vTaskDelay(100 / portTICK_PERIOD_MS);
            }
            if (row_num == 3)
            {
                row_num = -1;
            }
            gpio_set_level(row_to_pin, 1);
            printf("key pressed: %c\n", key); //TODO: ATTACH THIS TO BUTTON MANAGER CLASS TO PRODUCE BUTTON PRESSED EVENTS
        }
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

void button_manager_init(void)
{
    gpio_config_t io_conf;
    io_conf = {
        .pin_bit_mask = GPIO_ROW_MASK,
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    gpio_config(&io_conf);
    io_conf = {
        .pin_bit_mask = GPIO_ROW_MASK,
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    //start gpio task
    xTaskCreate(gpio_buttons_task, "gpio_buttons_task", 2048, NULL, 10, NULL);
}