#include "Display.h"

static const char *TAG = "DISPLAY";

Display display; // singleton instance of class

// Initialize the display
void Display::init_oled(void)
{
    ESP_LOGI(TAG, "Initializing...");

    vTaskDelay(100 / portTICK_RATE_MS);
    Display::send_cmd(0x38);
    Display::send_cmd(0x08);
    Display::send_cmd(0x01);
    vTaskDelay(100 / portTICK_RATE_MS);
    Display::send_cmd(0x06);
    Display::send_cmd(0x02);
    Display::send_cmd(0x0c);

    ESP_LOGI(TAG, "Success");
}

void Display::clear(void)
{
    Display::send_cmd(0x01);
    vTaskDelay(100 / portTICK_RATE_MS);
}

void Display::init(void)
{
    esp_err_t ret;
    // GPIO
    gpio_reset_pin(PIN_NUM_MISO);
    gpio_reset_pin(PIN_NUM_MOSI);
    gpio_reset_pin(PIN_NUM_SCLK);
    gpio_reset_pin(PIN_NUM_CS);
    gpio_set_direction(PIN_NUM_MISO, GPIO_MODE_INPUT);
    gpio_set_direction(PIN_NUM_MOSI, GPIO_MODE_OUTPUT);
    gpio_set_direction(PIN_NUM_SCLK, GPIO_MODE_OUTPUT);
    gpio_set_direction(PIN_NUM_CS, GPIO_MODE_OUTPUT);

    // SPI
    spi_bus_config_t config;
    config.mosi_io_num = PIN_NUM_MOSI;
    config.miso_io_num = PIN_NUM_MISO;
    config.sclk_io_num = PIN_NUM_SCLK;
    config.quadwp_io_num = -1; // -1 not used
    config.quadhd_io_num = -1; // -1 not used
    config.max_transfer_sz = 16;
    ret = spi_bus_initialize(SPI2_HOST, &config, SPI_DMA_DISABLED);
    assert(ret == ESP_OK);

    spi_device_interface_config_t spi_config = {
        .command_bits = 2,
        .address_bits = 0,
        .dummy_bits = 0,
        .mode = 0, // SPI mode 0
        .duty_cycle_pos = 0,
        .cs_ena_pretrans = 0,
        .cs_ena_posttrans = 0,
        .clock_speed_hz = 500 * 1000, // 500KHz?
        .input_delay_ns = 0,
        .spics_io_num = PIN_NUM_CS, // CS pin
        .flags = 0,
        .queue_size = 1, // We want to be able to queue 1 transactions at a time
    };

    spi_bus_add_device(SPI2_HOST, &spi_config, &this->handle);
    Display::init_oled();
    vTaskDelay(100 / portTICK_RATE_MS);
}

void Display::send_cmd(const uint8_t cmd)
{
    esp_err_t ret;
    spi_transaction_t t;
    memset(&t, 0, sizeof(t)); // Zero out the transaction
    t.length = 8;             // Command is 8 bits
    t.cmd = 0x0;
    // uint16_t cmd_fix = cmd;
    t.tx_buffer = &cmd;                                  // The data is the cmd itself
    t.user = (void *)0;                                  // D/C needs to be set to 0
    ret = spi_device_polling_transmit(this->handle, &t); // Transmit!
    assert(ret == ESP_OK);                               // Should have had no issues.
}

void Display::send_data(const uint8_t data)
{
    esp_err_t ret;
    spi_transaction_t t;

    memset(&t, 0, sizeof(t)); // Zero out the transaction
    t.length = 8;             // Len is in bytes, transaction length is in bits.
    t.cmd = 0x2;
    // uint16_t data_fix = data;
    t.tx_buffer = &data;                                 // Data               //Command is 8 bits
    t.user = (void *)1;                                  // D/C needs to be set to 1
    ret = spi_device_polling_transmit(this->handle, &t); // Transmit!
    assert(ret == ESP_OK);                               // Should have had no issues.
}

void Display::display1(const char *str)
{
    Display::send_cmd(0x02);
    int i = 0;
    for (i = 0; str[i] != '\0'; i++)
        Display::send_data(((uint8_t)str[i]));
}

void Display::display2(const char *str)
{
    Display::send_cmd(0xc0);
    int i = 0;
    for (i = 0; str[i] != '\0'; i++)
        Display::send_data(((uint8_t)str[i]));
}
