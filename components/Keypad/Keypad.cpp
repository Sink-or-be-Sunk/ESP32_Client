#include "Keypad.h"

#define ROW_1_PIN GPIO_NUM_19
#define ROW_2_PIN GPIO_NUM_18
#define ROW_3_PIN GPIO_NUM_5
#define ROW_4_PIN GPIO_NUM_17
#define COL_1_PIN GPIO_NUM_16
#define COL_2_PIN GPIO_NUM_4
#define COL_3_PIN GPIO_NUM_0
#define COL_4_PIN GPIO_NUM_2

char decipher_keypad(int row, int col)
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

void init_keypad()
{
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
}

char begin_scan_keypad()
{

    bool key_pressed = false;
    char key = 0;
    while (!key_pressed)
    {
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
        }
    }
    return key;
}