#include "ButtonManager.h"
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <freertos/task.h>
#include <freertos/event_groups.h>
#include "driver/gpio.h"
#include "ScreenManager.h"
#include "LEDManager.h"

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

static xQueueHandle gpio_evt_queue = NULL;

static void handle_button_event(int row, int col)
{
    char letter = decode_letter[row * 4 + col];
    printf("key: %c\n", letter);
    switch (letter)
    {
    case '6':
    {
        screenManager.upArrow();
        break;
    }
    case '#':
    {
        screenManager.downArrow();
        break;
    }
    case 'C':
    {
        screenManager.rightArrow();
        break;
    }
    case '8':
    {
        screenManager.leftArrow();
        break;
    }
    case '9':
    {
        screenManager.enter();
        break;
    }
    case '0':
    {
        screenManager.leftPage();
        break;
    }
    case 'D':
    {
        screenManager.rightPage();
        break;
    }
    case 'B':
    {
        screenManager.back();
        break;
    }
    case '1':
    {
        ledManager.start();
        break;
    }
    case '2':
    {
        ledManager.pause();
        break;
    }
    }
}

static void IRAM_ATTR gpio_isr_handler(void *arg)
{
    uint32_t gpio_num = (uint32_t)arg;
    xQueueSendFromISR(gpio_evt_queue, &gpio_num, NULL);
}

static void gpio_buttons_task(void *arg)
{
    uint32_t col;
    for (;;)
    {
        if (xQueueReceive(gpio_evt_queue, &col, portMAX_DELAY))
        {
            // bring all rows low
            gpio_set_level(ROW_1_PIN, 0);
            gpio_set_level(ROW_2_PIN, 0);
            gpio_set_level(ROW_3_PIN, 0);
            gpio_set_level(ROW_4_PIN, 0);

            for (int row = 0; row < 4; row++)
            {
                gpio_set_level(decode_row_pins[row], 1); // turn on row

                vTaskDelay(10 / portTICK_RATE_MS); // allow pin voltage to settle

                if (gpio_get_level(decode_col_pins[col]))
                {
                    handle_button_event(row, col);
                    while (gpio_get_level(decode_col_pins[col]))
                    {
                        vTaskDelay(10 / portTICK_PERIOD_MS); // wait for button to be released
                    }
                    break;
                }

                gpio_set_level(decode_row_pins[row], 0); // turn off row
            }

            // bring all rows high
            gpio_set_level(ROW_1_PIN, 1);
            gpio_set_level(ROW_2_PIN, 1);
            gpio_set_level(ROW_3_PIN, 1);
            gpio_set_level(ROW_4_PIN, 1);
        }
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
        .pin_bit_mask = GPIO_COL_MASK,
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_ENABLE,
        .intr_type = GPIO_INTR_POSEDGE,
    };
    gpio_config(&io_conf);

    // create a queue to handle gpio event from isr
    gpio_evt_queue = xQueueCreate(10, sizeof(uint32_t));

    // start gpio task
    xTaskCreate(gpio_buttons_task, "gpio_buttons_task", 2048, NULL, 10, NULL);

    // install gpio isr service
    gpio_install_isr_service(0);

    // hook isr handler for specific gpio pin
    gpio_isr_handler_add(COL_1_PIN, gpio_isr_handler, (void *)0);
    gpio_isr_handler_add(COL_2_PIN, gpio_isr_handler, (void *)1);
    gpio_isr_handler_add(COL_3_PIN, gpio_isr_handler, (void *)2);
    gpio_isr_handler_add(COL_4_PIN, gpio_isr_handler, (void *)3);

    // set rows high to initiate interrupt on button press
    gpio_set_level(ROW_1_PIN, 1);
    gpio_set_level(ROW_2_PIN, 1);
    gpio_set_level(ROW_3_PIN, 1);
    gpio_set_level(ROW_4_PIN, 1);
}