#include "LEDManager.h"

LEDManager ledManager;

static const char *TAG = "LED";

#define RMT_TX_CHANNEL RMT_CHANNEL_0
#define EXAMPLE_CHASE_SPEED_MS (10)

led_strip_t *strip;

// FIXME: REMOVE THE LED RAINBOW DEMO FROM FINAL PROJECT BUILD (see config.h #define LED_RAINBOW_DEMO)
// TODO: OR MAYBE CHANGE THE RAINBOW INTO SOME WATER SPLASH FOR USE IN GAMEPLAY SOMETIME

/**
 * @brief Simple helper function, converting HSV color space to RGB color space
 *
 * Wiki: https://en.wikipedia.org/wiki/HSL_and_HSV
 *
 */
static void
led_strip_hsv2rgb(uint32_t h, uint32_t s, uint32_t v, uint32_t *r, uint32_t *g, uint32_t *b)
{
    h %= 360; // h -> [0,360]
    uint32_t rgb_max = v * 2.55f;
    uint32_t rgb_min = rgb_max * (100 - s) / 100.0f;

    uint32_t i = h / 60;
    uint32_t diff = h % 60;

    // RGB adjustment amount by hue
    uint32_t rgb_adj = (rgb_max - rgb_min) * diff / 60;

    switch (i)
    {
    case 0:
        *r = rgb_max;
        *g = rgb_min + rgb_adj;
        *b = rgb_min;
        break;
    case 1:
        *r = rgb_max - rgb_adj;
        *g = rgb_max;
        *b = rgb_min;
        break;
    case 2:
        *r = rgb_min;
        *g = rgb_max;
        *b = rgb_min + rgb_adj;
        break;
    case 3:
        *r = rgb_min;
        *g = rgb_max - rgb_adj;
        *b = rgb_max;
        break;
    case 4:
        *r = rgb_min + rgb_adj;
        *g = rgb_min;
        *b = rgb_max;
        break;
    default:
        *r = rgb_max;
        *g = rgb_min;
        *b = rgb_max - rgb_adj;
        break;
    }
}

static void led_task(void *arg)
{
    uint32_t red = 0;
    uint32_t green = 0;
    uint32_t blue = 0;
    uint16_t hue = 0;
    for (;;)
    {
        for (int i = 0; i < 360; i++)
        {
            for (int j = 0; j < NUMBER_OF_LEDS; j++)
            {
                hue = (uint16_t)(i + j * 20);
                led_strip_hsv2rgb(hue, 100, 100, &red, &green, &blue);
                // Write RGB values to strip driver
                ESP_ERROR_CHECK(strip->set_pixel(strip, j, red, green, blue));
            }

            // Flush RGB values to LEDs
            ESP_ERROR_CHECK(strip->refresh(strip, 100));
            vTaskDelay(pdMS_TO_TICKS(EXAMPLE_CHASE_SPEED_MS));
        }
    }
}

void LEDManager::init(void)
{
    ESP_LOGI(TAG, "Initializing...");

    rmt_config_t config = RMT_DEFAULT_CONFIG_TX(LED_PIN, RMT_TX_CHANNEL);
    // set counter clock to 40MHz
    config.clk_div = 2;

    ESP_ERROR_CHECK(rmt_config(&config));
    ESP_ERROR_CHECK(rmt_driver_install(config.channel, 0, 0));

    // install ws2812 driver
    led_strip_config_t strip_config = LED_STRIP_DEFAULT_CONFIG(NUMBER_OF_LEDS, (led_strip_dev_t)config.channel);
    strip = led_strip_new_rmt_ws2812(&strip_config);
    if (!strip)
    {
        ESP_LOGE(TAG, "install WS2812 driver failed");
        return;
    }
    // Clear LED strip (turn off all LEDs)
    ESP_ERROR_CHECK(strip->clear(strip, 100));
    // Show simple rainbow chasing pattern

    // start led task
    xTaskCreate(led_task, "led_task", 2048, NULL, 10, &this->handle);
    vTaskSuspend(this->handle);

    ESP_LOGI(TAG, "Success");
}

void LEDManager::start(void)
{
    vTaskResume(this->handle);
}

void LEDManager::pause(void)
{
    vTaskSuspend(this->handle);

    vTaskDelay(pdMS_TO_TICKS(10));
    // Clear LED strip (turn off all LEDs)
    ESP_ERROR_CHECK(strip->clear(strip, 100));
}

void LEDManager::update(const char *str)
{
    for (int i = 0; i < NUMBER_OF_LEDS; i++)
    {
        switch (str[i])
        {
        case led_position_t::EMPTY:
        {
            ESP_ERROR_CHECK(strip->set_pixel(strip, i, 0, 0, 0x0f)); // TODO: DO WE WANT BLUE FOR EMPTY?
            break;
        }
        case led_position_t::FULL:
        {
            ESP_ERROR_CHECK(strip->set_pixel(strip, i, 0, 0xff, 0));
            break;
        }
        case led_position_t::HIT:
        {
            ESP_ERROR_CHECK(strip->set_pixel(strip, i, 0xff, 0, 0));
            break;
        }
        case led_position_t::MISS:
        {
            ESP_ERROR_CHECK(strip->set_pixel(strip, i, 0xff, 0xff, 0xff));
            break;
        }
        case led_position_t::SUNK:
        {
            ESP_ERROR_CHECK(strip->set_pixel(strip, i, 0xff, 0xa5, 0x00));
            break;
        }
        case led_position_t::POSITION:
        {
            ESP_ERROR_CHECK(strip->set_pixel(strip, i, 0xff, 0x00, 0xff));
            break;
        }
        default:
        {
            ESP_LOGE(TAG, "Invalid Board Position: %c", str[i]);
            break;
        }
        }
    }

    // Flush RGB values to LEDs
    ESP_ERROR_CHECK(strip->refresh(strip, 100));
}