#include "LEDManager.h"

LEDManager ledManager;

#define LED_PIN GPIO_NUM_17
#define LED_COUNT 1

void LEDManager::init(void)
{
    this->pixels = new Pixels(LED_PIN, LED_COUNT, Pixels::StripType::ws6812, RMT_CHANNEL_0, 2.8);

    this->pixels->Clear();

    this->pixels->SetPixel(0, 0xff, 0x0, 0x0, 0xff);

    this->pixels->Write();
    //TOOD: NEED TO ADD FUNCTIONS FOR SETTING DIFFERENT COLOR PATTERNS
}