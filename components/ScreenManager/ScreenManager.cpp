#include "ScreenManager.h"
#include "Display.h"
#include "Websocket.h"
#include "Messenger.h"

ScreenManager screenManager; //singleton instance of class

void ScreenManager::init(void)
{
    this->state = HOME;
    ScreenManager::render();
}

void ScreenManager::render(void)
{
    display.clear();
    switch (this->state)
    {
    case HOME:
    {
        //               "-=-=-=-=-=-=-=-="
        display.display1("  Welcome to");
        display.display2("Sink or be Sunk!");
        break;
    }
    case INIT_PAIRING:
    {
        //               "-=-=-=-=-=-=-=-="
        display.display1("Start Pairing");
        display.display2("Press #");
        break;
    }
    case CONFIRM_PAIRING:
    {
        //               "-=-=-=-=-=-=-=-="
        display.display1("Confirm Device");
        display.display2("Press #");
        break;
    }
    }
}

void ScreenManager::upArrow(void)
{
    switch (this->state)
    {
    case HOME:
    {
        this->state = INIT_PAIRING;
        break;
    }
    case INIT_PAIRING:
    {
        this->state = CONFIRM_PAIRING;
        break;
    }
    case CONFIRM_PAIRING:
    {
        this->state = HOME;
        break;
    }
    }
    ScreenManager::render();
}

void ScreenManager::downArrow(void)
{
    switch (this->state)
    {
    case HOME:
    {
        this->state = CONFIRM_PAIRING;
        break;
    }
    case INIT_PAIRING:
    {
        this->state = HOME;
        break;
    }
    case CONFIRM_PAIRING:
    {
        this->state = INIT_PAIRING;
        break;
    }
    }
    ScreenManager::render();
}

void ScreenManager::enter(void)
{
    switch (this->state)
    {
    case HOME:
    {
        printf("Do Nothing\n");
        break;
    }
    case INIT_PAIRING:
    {
        websocket.send(messenger.build_registration_msg(ENQUEUE));
        break;
    }
    case CONFIRM_PAIRING:
    {
        websocket.send(messenger.build_registration_msg(CONFIRM));
        break;
    }
    }
}