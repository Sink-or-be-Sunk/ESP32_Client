#include "ButtonManager.h"
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <freertos/task.h>
#include <freertos/event_groups.h>
#include "driver/gpio.h"
#include "ScreenManager.h"
#include "LEDManager.h"

#define ROW_0_PIN GPIO_NUM_39
#define ROW_1_PIN GPIO_NUM_14
#define ROW_2_PIN GPIO_NUM_22
#define ROW_3_PIN GPIO_NUM_23
// #define ROW_3_PIN GPIO_NUM_2
#define COL_0_PIN GPIO_NUM_12
#define COL_1_PIN GPIO_NUM_27
#define COL_2_PIN GPIO_NUM_33
#define COL_3_PIN GPIO_NUM_15

#define GPIO_ROW_MASK ((1ULL << ROW_0_PIN) | \
                       (1ULL << ROW_1_PIN) | \
                       (1ULL << ROW_2_PIN) | \
                       (1ULL << ROW_3_PIN))

#define GPIO_COL_MASK ((1ULL << COL_0_PIN) | \
                       (1ULL << COL_1_PIN) | \
                       (1ULL << COL_2_PIN) | \
                       (1ULL << COL_3_PIN))

static gpio_num_t decode_row_pins[4] = {ROW_0_PIN, ROW_1_PIN, ROW_2_PIN, ROW_3_PIN};
static gpio_num_t decode_col_pins[4] = {COL_0_PIN, COL_1_PIN, COL_2_PIN, COL_3_PIN};
static char decode_letter[16] = {'1', '2', '3', 'A', '4', '5', '6', 'B', '7', '8', '9', 'C', '*', '0', '#', 'D'};

static xQueueHandle gpio_evt_queue = NULL;

static void handle_button_event(int row, int col)
{
    printf("row: %d, col: %d\n", row, col);
    char letter = decode_letter[row * 4 + col];
    printf("key: %c\n", letter);
    switch (letter)
    {
    case '9':
    {
        screenManager.upArrow();
        break;
    }
    case '#':
    {
        screenManager.downArrow();
        break;
    }
    case 'D':
    {
        screenManager.rightArrow();
        break;
    }
    case '0':
    {
        screenManager.leftArrow();
        break;
    }
    case '*':
    {
        screenManager.enter();
        break;
    }
    case '8':
    {
        screenManager.leftPage();
        break;
    }
    case 'C':
    {
        screenManager.rightPage();
        break;
    }
    case '7':
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
    case '3':
    {
        screenManager.press(2); // zero indexed row/col
        break;
    }
    case 'A':
    {
        screenManager.press(3); // zero indexed row/col
        break;
    }
    case '4':
    {
        screenManager.press(4); // zero indexed row/col
        break;
    }
    case '5':
    {
        screenManager.press(5); // zero indexed row/col
        break;
    }
    case '6':
    {
        screenManager.press(6); // zero indexed row/col
        break;
    }
    case 'B':
    {
        screenManager.press(7); // zero indexed row/col
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
    uint32_t row;
    for (;;)
    {
        if (xQueueReceive(gpio_evt_queue, &row, portMAX_DELAY))
        {
            // bring all cols low
            gpio_set_level(COL_0_PIN, 0);
            gpio_set_level(COL_1_PIN, 0);
            gpio_set_level(COL_2_PIN, 0);
            gpio_set_level(COL_3_PIN, 0);

            for (int col = 0; col < 4; col++)
            {
                gpio_set_level(decode_col_pins[col], 1); // turn on col

                vTaskDelay(10 / portTICK_RATE_MS); // allow pin voltage to settle

                if (gpio_get_level(decode_row_pins[row]))
                {
                    handle_button_event(row, col);
                    while (gpio_get_level(decode_row_pins[row]))
                    {
                        vTaskDelay(10 / portTICK_PERIOD_MS); // wait for button to be released
                    }
                    break;
                }

                gpio_set_level(decode_col_pins[col], 0); // turn off col
            }

            // bring all cols high
            gpio_set_level(COL_0_PIN, 1);
            gpio_set_level(COL_1_PIN, 1);
            gpio_set_level(COL_2_PIN, 1);
            gpio_set_level(COL_3_PIN, 1);
        }
    }
}

void button_manager_init(void)
{
    gpio_config_t io_conf;
    io_conf = {
        .pin_bit_mask = GPIO_COL_MASK,
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
        .pull_down_en = GPIO_PULLDOWN_ENABLE,
        .intr_type = GPIO_INTR_POSEDGE,
    };
    gpio_config(&io_conf);

    // create a queue to handle gpio event from isr
    gpio_evt_queue = xQueueCreate(10, sizeof(uint32_t));

    // start gpio task
    xTaskCreate(gpio_buttons_task, "gpio_buttons_task", 4096, NULL, 10, NULL);

    // install gpio isr service
    gpio_install_isr_service(0);

    // hook isr handler for specific gpio pin
    gpio_isr_handler_add(ROW_0_PIN, gpio_isr_handler, (void *)0);
    gpio_isr_handler_add(ROW_1_PIN, gpio_isr_handler, (void *)1);
    gpio_isr_handler_add(ROW_2_PIN, gpio_isr_handler, (void *)2);
    gpio_isr_handler_add(ROW_3_PIN, gpio_isr_handler, (void *)3);

    // set rows high to initiate interrupt on button press
    gpio_set_level(COL_0_PIN, 1);
    gpio_set_level(COL_1_PIN, 1);
    gpio_set_level(COL_2_PIN, 1);
    gpio_set_level(COL_3_PIN, 1);
}