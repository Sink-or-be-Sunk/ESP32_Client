#include "ShipPosition.h"

static const char *TAG = "SHIP_POS";

void ShipPosition::init()
{
    this->front_r = -1;
    this->front_c = -1;
    this->back_r = -1;
    this->back_c = -1;
    this->isReady = false;
    ESP_LOGW(TAG, "Ship Reset (r:%d,c:%d), (r:%d,c:%d)", this->front_r, this->front_c, this->back_r, this->back_c);
}

void ShipPosition::position(int r1, int c1, int r2, int c2)
{
    this->front_r = r1;
    this->back_r = r2;
    this->front_c = c1;
    this->back_c = c2;
    this->isReady = true;
    ESP_LOGW(TAG, "Positioned: (r:%d,c:%d), (r:%d,c:%d)", this->front_r, this->front_c, this->back_r, this->back_c);
}

bool ShipPosition::remove(int row, int col)
{
    ESP_LOGI(TAG, "Checking: (r:%d,c:%d) in (r:%d,c:%d), (r:%d,c:%d)", row, col, this->front_r, this->front_c, this->back_r, this->back_c);
    bool valid = false;
    bool isFront = this->front_r == row && this->front_c == col;
    bool isBack = this->back_r == row && this->back_c == col;

    if (isFront || isBack)
    {
        ESP_LOGW(TAG, "Found to Remove (r:%d,c:%d), (r:%d,c:%d)", this->front_r, this->front_c, this->back_r, this->back_c);
        this->init();
        ESP_LOGE(TAG, "After Remove (r:%d,c:%d), (r:%d,c:%d)", this->front_r, this->front_c, this->back_r, this->back_c);
        valid = true;
    }
    ESP_LOGI(TAG, "Not Found to Remove from => (r:%d,c:%d), (r:%d,c:%d)", this->front_r, this->front_c, this->back_r, this->back_c);

    return valid;
}