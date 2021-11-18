#include "MotorManager.h"

static const char *TAG = "MOTOR";

#define RUMBLE_DURATION_MS 1000

#define GPIO_MOTOR_DRIVER_MASK (1ULL << MOTOR_DRIVER_PIN)

MotorManager motorManager; // singleton instance of class

static void rumble_motor_task(void *args)
{
    for (;;)
    {
        ESP_LOGI(TAG, "Starting...");
        vTaskDelay(pdMS_TO_TICKS(RUMBLE_DURATION_MS));
        gpio_set_level(MOTOR_DRIVER_PIN, 0);
        ESP_LOGI(TAG, "...Stopping");
        vTaskSuspend(motorManager.handle);
    }
}

void MotorManager::init(void)
{
    ESP_LOGI(TAG, "Initializing...");

    gpio_config_t io_conf;
    io_conf = {
        .pin_bit_mask = GPIO_MOTOR_DRIVER_MASK,
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    gpio_config(&io_conf);

    // start motor task
    xTaskCreate(rumble_motor_task, "rumble_motor_task", 2048, NULL, 10, &this->handle);
    vTaskSuspend(this->handle);

    gpio_set_level(MOTOR_DRIVER_PIN, 0);

    ESP_LOGI(TAG, "Success!");
}

void MotorManager::rumble(void)
{
    gpio_set_level(MOTOR_DRIVER_PIN, 1);
    vTaskResume(this->handle);
}