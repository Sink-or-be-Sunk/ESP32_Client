#include "ScreenManager.h"
#include "Display.h"
#include "Websocket.h"
#include "Messenger.h"
#include "GameState.h"

ScreenManager screenManager; // singleton instance of class

void ScreenManager::setState(SCREEN_STATE state)
{
    this->state = state;
    ScreenManager::render();
}

void ScreenManager::init(void)
{
    this->state = WIFI_CONNECTING;
    ScreenManager::render();
}

void ScreenManager::render(void)
{
    display.clear();
    switch (this->state)
    {
    case WIFI_CONNECTING:
    {
        //               "-=-=-=-=-=-=-=-="
        display.display1("No WiFi Detected");
        display.display2("Provision Device");
        break;
    }
    case HOME:
    {
        //               "-=-=-=-=-=-=-=-="
        display.display1("  Welcome to");
        display.display2("Sink or be Sunk!");
        break;
    }
    case CONFIRM_PAIRING:
    {
        //               "-=-=-=-=-=-=-=-="
        display.display1("Confirm Device");
        display.display2("Press Enter");
        break;
    }
    case INIT_PAIRING:
    {
        //               "-=-=-=-=-=-=-=-="
        display.display1("Start Pairing");
        display.display2("Press Enter");
        break;
    }
    case WAITING_PAIRING:
    {
        //               "-=-=-=-=-=-=-=-="
        display.display1("Waiting for Web");
        display.display2("Confirmation");
        break;
    }
    case CREATE_GAME:
    {
        //               "-=-=-=-=-=-=-=-="
        display.display1("Create Game");
        display.display2("Press Enter");
        break;
    }
    case FIND_GAME:
    {
        //               "-=-=-=-=-=-=-=-="
        display.display1("Find Games");
        display.display2("Press Enter");
        break;
    }
    case JOIN_GAME:
    {
        //               "-=-=-=-=-=-=-=-="
        display.display1("Join Game");
        display.display2("Scroll Down"); // TODO: NEEDS IMPLEMENTATION
        break;
    }
    case READY_UP_SHIPS:
    {
        //               "-=-=-=-=-=-=-=-="
        display.display1("Ready Up");
        display.display2("Press Enter");
        break;
    }
    case INVITE_FRIEND:
    {
        //               "-=-=-=-=-=-=-=-="
        display.display1("Ready Up");
        display.display2("Scroll :Down");
        break;
    }
    case ATTACK:
    {
        //               "-=-=-=-=-=-=-=-="
        display.display1("Enter Attack");
        char buff[20];
        sprintf(buff, "Coords: r%d, c%d", gameState.get_attack_row(), gameState.get_attack_col());
        display.display2(buff);
        break;
    }
    }
}

void ScreenManager::rightArrow(void)
{
    switch (gameState.state)
    {
    case SETUP:
    {
        switch (this->state)
        {
        case INIT_PAIRING:
        {
            this->state = CREATE_GAME;
            break;
        }
        case CREATE_GAME:
        {
            this->state = FIND_GAME;
            break;
        }
        case FIND_GAME:
        {
            this->state = INIT_PAIRING;
            break;
        }
        default:
        {
            printf("Right Arrow Ignored!\n");
            break;
        }
        }
        break;
    }
    case LOBBY:
    {
        switch (this->state)
        {
        case READY_UP_SHIPS:
        {
            this->state = INVITE_FRIEND;
            break;
        }
        case INVITE_FRIEND:
        {
            this->state = READY_UP_SHIPS;
            break;
        }
        default:
        {
            printf("Right Arrow Ignored!\n");
            break;
        }
        }
        break;
    }
    case IN_PROGRESS:
    {
        switch (this->state)
        {
        default:
        {
            printf("Right Arrow Ignored!\n");
            break;
        }
        }
        break;
    }
    }

    ScreenManager::render();
}

void ScreenManager::leftArrow(void)
{
    ScreenManager::rightArrow(); // FIXME: COPY/UPDATE THIS WITH RIGHT ARROW
    // ScreenManager::render();
}

void ScreenManager::upArrow(void)
{
    switch (this->state)
    {
    default:
    {
        printf("Up Arrow Ignored!\n");
        break;
    }
    }
    ScreenManager::render();
}

void ScreenManager::downArrow(void)
{
    switch (this->state)
    {
    default:
    {
        printf("Down Arrow Ignored!\n");
        break;
    }
    }
    ScreenManager::render();
}

void ScreenManager::enter(void)
{
    switch (gameState.state)
    {
    case SETUP:
    {
        switch (this->state)
        {
        case INIT_PAIRING:
        {
            websocket.send(messenger.build_registration_msg(ENQUEUE));
            this->state = WAITING_PAIRING;
            ScreenManager::render();
            break;
        }
        case CREATE_GAME:
        {
            websocket.send(messenger.build_new_game_msg());
            break;
        }
        case JOIN_GAME:
        {
            // TODO:
            break;
        }
        default:
        {
            printf("Enter Ignored!\n");
            break;
        }
        }
        break;
    }
    case LOBBY:
    {
        switch (this->state)
        {
        case READY_UP_SHIPS:
        {
            this->state = INVITE_FRIEND;
            break;
        }
        case INVITE_FRIEND:
        {
            this->state = READY_UP_SHIPS;
            break;
        }
        default:
        {
            printf("Right Arrow Ignored!\n");
            break;
        }
        }
        break;
    }
    case IN_PROGRESS:
    {
        switch (this->state)
        {
        case ATTACK:
        {
            // websocket.send(messenger.build_registration_msg(CONFIRM));
            websocket.send(messenger.build_attack_msg(1, 2, SOLO, gameState.opponent));
            break;
        }
        default:
        {
            printf("Enter Ignored!\n");
            break;
        }
        }
        break;
    }
    }
}

void ScreenManager::back(void)
{
    switch (gameState.state)
    {
    case SETUP:
    {
        switch (this->state)
        {
        case WAITING_PAIRING:
        {
            this->state = INIT_PAIRING;
            ScreenManager::render();
            break;
        }
        default:
        {
            printf("back ignored!\n");
            break;
        }
        }
        break;
    }
    case LOBBY:
    {
        // TODO:
        break;
    }
    case IN_PROGRESS:
    {
        // TODO:
        break;
    }
    }
}