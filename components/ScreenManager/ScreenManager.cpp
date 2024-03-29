#include "ScreenManager.h"

static const char *TAG = "SCREEN";

ScreenManager screenManager; // singleton instance of class

void ScreenManager::setState(SCREEN_STATE state)
{
    this->state = state;
    this->render();
}

SCREEN_STATE ScreenManager::getState(void)
{
    return this->state;
}

void ScreenManager::init(void)
{
    ESP_LOGI(TAG, "Initializing...");

    this->state = WIFI_CONNECTING;
    this->render();

    ESP_LOGI(TAG, "Success");
}

void ScreenManager::conditionalRender(SCREEN_STATE state)
{
    if (this->state == state)
    {
        this->render();
    }
    else
    {
        ESP_LOGE(TAG, "Conditional Render Failed");
    }
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
    case DEVICE_PAIRED:
    {
        //               "-=-=-=-=-=-=-=-="
        display.display1("Device Paired to");
        char buff[17];
        strncpy(buff, settings.username, 16);
        display.display2(buff);
        break;
    }
    case REBOOT:
    {
        //               "-=-=-=-=-=-=-=-="
        display.display1("REBOOT REQUIRED");
        display.display2(" PLEASE WAIT");
        break;
    }
    case MOVE_MADE:
    {
        if (gameState.myTurn) // NOTE: These are flipped because after I make a move
        // it will no longer be my turn, i.e. myTurn is false
        {
            //               "-=-=-=-=-=-=-=-="
            display.display1("Attack Made At");
        }
        else
        {
            //               "-=-=-=-=-=-=-=-="
            display.display1("Your Move Result");
        }

        display.display2(gameState.lastMoveRes);
        break;
    }
    case INVITE_SENT:
    {
        char buff[17];
        strncpy(buff, friendManager.getCurDisplayName(), 16);
        //               "-=-=-=-=-=-=-=-="
        display.display1("Invite Sent To:");
        display.display2(buff);
        break;
    }
    case NO_MORE_FRIENDS:
    {
        //               "-=-=-=-=-=-=-=-="
        display.display1("No More Friends:");
        display.display2("Please Scroll Up");
        break;
    }
    case INVALID_SHIP_LAYOUT:
    {
        //               "-=-=-=-=-=-=-=-="
        display.display1("Invalid Ship");
        display.display2("Layout");
        break;
    }
    case NOTIFY_POSITION_SHIPS:
    {
        //               "-=-=-=-=-=-=-=-="
        display.display1("Ship Layout");
        display.display2("Accepted!");
        break;
    }
    case NOTIFY_INVALID_MOVE:
    {
        //                "-=-=-=-=-=-=-=-="
        display.display1("Invalid Move");
        display.display2(gameState.lastMoveRes);
        break;
    }
    case LEFT_GAME:
    {
        //                "-=-=-=-=-=-=-=-="
        display.display1("Left Game");
        break;
    }
    case OPPONENT_READY_UP:
    {
        //               "-=-=-=-=-=-=-=-="
        char buff[17];
        strncpy(buff, gameState.opponent, 16);
        display.display1(buff);
        display.display2("Is Ready to Play");
        break;
    }
    case PLAYER_IN_LOBBY:
    {
        //               "-=-=-=-=-=-=-=-="
        char buff[17];
        strncpy(buff, gameState.opponent, 16);
        display.display1(buff);
        display.display2("Is In Lobby");
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
    case GAME_OVER:
    {
        //               "-=-=-=-=-=-=-=-="
        display.display1("Game Over");
        display.display2(gameState.lastMoveRes);

        break;
    }
    case FIND_GAME:
    {
        //               "-=-=-=-=-=-=-=-="
        display.display1("Find Games");
        display.display2("Scroll Down");
        break;
    }
    case READY_UP_SHIPS:
    {
        if (shipManager.isReady())
        {
            //               "-=-=-=-=-=-=-=-="
            display.display1("Ready Up");
            display.display2("Press Enter");
        }
        else
        {
            display.display1("Position Ships");
            char buff[17];
            //                           "-=-=-=-=-=-=-=-="
            snprintf(buff, sizeof(buff), "%d Ships Left", shipManager.shipsRemaining());
            display.display2(buff);
        }
        break;
    }
    case INVITE_FRIEND:
    {
        //               "-=-=-=-=-=-=-=-="
        display.display1("Invite Friend");
        display.display2("Scroll Down");
        break;
    }
    case FRIENDS_LIST:
    {
        char buff[17];
        strncpy(buff, friendManager.getCurDisplayName(), 16);
        //               "-=-=-=-=-=-=-=-="
        display.display1("Select Friend:");
        display.display2(buff);
        break;
    }
    case ATTACK:
    {
        //               "-=-=-=-=-=-=-=-="
        display.display1("Enter Attack");
        char buff[17];
        snprintf(buff, sizeof(buff), "Coords: %c%c, %c%c",
                 gameState.get_attack_col_tag(),
                 gameState.get_attack_col() + 'A',
                 gameState.get_attack_row_tag(),
                 gameState.get_attack_row() + '1');
        display.display2(buff);
        break;
    }
#ifdef DEBUG_SHIP_POSITIONS
    case SHIP_POSITION_DEBUG:
    {
        //               "-=-=-=-=-=-=-=-="
        display.display1("Ship Pos Debug");
        char buff[17];
        snprintf(buff, sizeof(buff), "Coords: C%c, R%c",
                 shipManager.col + '0',
                 shipManager.row + '0');
        display.display2(buff);
        break;
    }
#endif
    }
}

void ScreenManager::rightPage(void)
{
    switch (gameState.getState())
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
            ESP_LOGI(TAG, "Right Page Ignored!");
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
            ESP_LOGI(TAG, "Right Page Ignored!");
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
            ESP_LOGI(TAG, "Right Page Ignored!");
            break;
        }
        }
        break;
    }
    }

    this->render();
}

void ScreenManager::leftPage(void)
{
    switch (gameState.getState())
    {
    case SETUP:
    {
        switch (this->state)
        {
        case INIT_PAIRING:
        {
            this->state = FIND_GAME;
            break;
        }
        case CREATE_GAME:
        {
            this->state = INIT_PAIRING;
            break;
        }
        case FIND_GAME:
        {
            this->state = CREATE_GAME;
            break;
        }
        default:
        {
            ESP_LOGI(TAG, "Left Page Ignored!");
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
            ESP_LOGI(TAG, "Left Page Ignored!");
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
            ESP_LOGI(TAG, "Left Page Ignored!");
            break;
        }
        }
        break;
    }
    }

    this->render();
}

void ScreenManager::rightArrow(void)
{
#ifndef DEBUG_SHIP_POSITIONS
    switch (this->state)
    {
    case ATTACK:
    {
        gameState.request_right_sel_attack();
        this->render();
        break;
    }
    default:
    {
        ESP_LOGI(TAG, "Right Arrow Ignored");
        break;
    }
    }
#else
    shipManager.right();
    this->render();
#endif
}

void ScreenManager::leftArrow(void)
{
#ifndef DEBUG_SHIP_POSITIONS
    switch (this->state)
    {
    case ATTACK:
    {
        gameState.request_left_sel_attack();
        this->render();
        break;
    }
    default:
    {
        ESP_LOGI(TAG, "Left Arrow Ignored");
        break;
    }
    }
#else
    shipManager.left();
    this->render();
#endif
}

void ScreenManager::upArrow(void)
{
#ifndef DEBUG_SHIP_POSITIONS
    switch (this->state)
    {
    case ATTACK:
    {
        gameState.increment_attack();
        this->render();
        break;
    }
    case FRIENDS_LIST:
    {
        if (friendManager.prev())
        {
            this->render();
        }
        else
        {
            switch (gameState.getState())
            {
            case SETUP:
            {
                // FIND GAME
                this->setState(FIND_GAME);
                break;
            }
            case LOBBY:
            {
                // INVITE FRIEND
                this->setState(INVITE_FRIEND);
                break;
            }
            default:
            {
                ESP_LOGI(TAG, "Error: Invalid state change from friends list");
                break;
            }
            }
        }
        break;
    }
    case NO_MORE_FRIENDS:
    {
        this->setState(FRIENDS_LIST);
        break;
    }
    default:
    {
        ESP_LOGI(TAG, "Up Arrow Ignored!");
        break;
    }
    }

#else
    shipManager.up();
    this->render();
#endif
}

void ScreenManager::downArrow(void)
{
#ifndef DEBUG_SHIP_POSITIONS
    switch (this->state)
    {
    case ATTACK:
    {
        gameState.decrement_attack();
        this->render();
        break;
    }
    case FIND_GAME:
    case INVITE_FRIEND:
    {
        websocket.send(messenger.build_db_msg(GET_FRIENDS, NULL));
        break;
    }
    case FRIENDS_LIST:
    {
        if (friendManager.next())
        {
            this->render();
        }
        else
        {
            this->setState(NO_MORE_FRIENDS);
            break;
        }
        break;
    }
    default:
    {
        ESP_LOGI(TAG, "Down Arrow Ignored!");
        break;
    }
    }

#else
    shipManager.down();
    this->render();
#endif
}

void ScreenManager::enter(void)
{
    switch (gameState.getState())
    {
    case SETUP:
    {
        switch (this->state)
        {
        case INIT_PAIRING:
        {
            this->state = WAITING_PAIRING;
            this->render();
            websocket.send(messenger.build_registration_msg(ENQUEUE));
            break;
        }
        case CONFIRM_PAIRING:
        {
            websocket.send(messenger.build_registration_msg(CONFIRM));
            break;
        }
        case CREATE_GAME:
        {
            websocket.send(messenger.build_new_game_msg());
            break;
        }
        case FRIENDS_LIST:
        {
            // JOIN GAME
            websocket.send(messenger.build_join_game_msg(friendManager.getCurUsername()));
            break;
        }
        default:
        {
            ESP_LOGI(TAG, "Enter Ignored!");
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
            if (shipManager.isReady())
            {
                websocket.send(messenger.build_position_ships());
                gameState.setState(IN_PROGRESS);
            }
            else
            {
                this->splash(INVALID_SHIP_LAYOUT);
            }
            break;
        }
        case FRIENDS_LIST:
        {
            // INVITE
            websocket.send(messenger.build_db_msg(INVITE, friendManager.getCurUsername()));
            break;
        }
        default:
        {
            ESP_LOGI(TAG, "Enter Ignored!");
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
            websocket.send(messenger.build_attack_msg(gameState.get_attack_row() + '0', gameState.get_attack_col() + '0', SOLO, gameState.opponent));
            break;
        }
        default:
        {
            ESP_LOGI(TAG, "Enter Ignored!");
            break;
        }
        }
        break;
    }
    }
}

void ScreenManager::press(int num)
{
    switch (this->state)
    {
    case ATTACK:
    {
        gameState.set_attack(num);
        this->render();
        break;
    }
    default:
    {
        ESP_LOGI(TAG, "Key %c Ignored!", num);
        break;
    }
    }
}

void ScreenManager::back(void)
{
    switch (gameState.getState())
    {
    case SETUP:
    {
        switch (this->state)
        {
        case WAITING_PAIRING:
        {
            websocket.send(messenger.build_registration_msg(DEQUEUE));
            break;
        }
        default:
        {
            ESP_LOGI(TAG, "back ignored!");
            break;
        }
        }
        break;
    }
    case LOBBY:
    {
        websocket.send(messenger.build_leave_game_msg());
        break;
    }
    case IN_PROGRESS:
    {
        // TODO:
        break;
    }
    }
}

void ScreenManager::splash(SCREEN_STATE state)
{
    this->splash(state, this->state);
}

void ScreenManager::splash(SCREEN_STATE splashState, SCREEN_STATE returnState)
{
    this->setState(splashState);
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    this->setState(returnState);
}