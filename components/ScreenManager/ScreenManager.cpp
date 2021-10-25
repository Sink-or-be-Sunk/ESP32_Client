#include "ScreenManager.h"
#include "Display.h"
#include "Websocket.h"
#include "Messenger.h"
#include "GameState.h"

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
    this->state = WIFI_CONNECTING;
    this->render();
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
        char *boardPtr;
        int8_t *coordsPtr;
        if (gameState.myTurn) // NOTE: These are flipped because after I make a move
        // it will no longer be my turn, i.e. myTurn is false
        {
            //               "-=-=-=-=-=-=-=-="
            display.display1("Attack Made At");
            boardPtr = gameState.positionBoard;
            coordsPtr = gameState.opponentAttackCoords;
        }
        else
        {
            //               "-=-=-=-=-=-=-=-="
            display.display1("Your Move Result");
            boardPtr = gameState.attackBoard;
            coordsPtr = gameState.attackCoords;
        }

        char buff[17];
        char result[5];
        char res = boardPtr[coordsPtr[0] * BOARD_WIDTH + coordsPtr[1]];
        printf("c: %d, r: %d, res: %c\n", coordsPtr[0], coordsPtr[1], res);
        switch (res)
        {
        case RESULT_SUNK:
        {
            strncpy(result, "SUNK", 5);
            break;
        }
        case RESULT_HIT:
        {
            strncpy(result, "HIT", 5);
            break;
        }
        case RESULT_MISS:
        {
            strncpy(result, "MISS", 5);
            break;
        }
        default:
        {
            strncpy(result, "ERR", 5);
            break;
        }
        }
        //                 "-=-=-=-=-=-=-=-="
        snprintf(buff, 16, "%s at c%c, r%c", result, coordsPtr[0] + 'A', coordsPtr[1] + '1');
        display.display2(buff);
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
    case OPPONENT_JOINED_GAME:
    {
        //               "-=-=-=-=-=-=-=-="
        char buff[17];
        strncpy(buff, gameState.opponent, 16);
        display.display1(buff);
        display.display2("Has Joined Game");
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
        display.display1("Invite Friend");
        display.display2("Scroll :Down");
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
    }
}

void ScreenManager::rightPage(void)
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
            printf("Right Page Ignored!\n");
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
            printf("Right Page Ignored!\n");
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
            printf("Right Page Ignored!\n");
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
    switch (gameState.state)
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
            printf("Left Page Ignored!\n");
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
            printf("Left Page Ignored!\n");
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
            printf("Left Page Ignored!\n");
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
        printf("Ignoring Right Arrow\n");
        break;
    }
    }
}

void ScreenManager::leftArrow(void)
{
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
        printf("Ignoring Left Arrow\n");
        break;
    }
    }
}

void ScreenManager::upArrow(void)
{
    switch (this->state)
    {
    case ATTACK:
    {
        gameState.increment_attack();
        break;
    }
    default:
    {
        printf("Up Arrow Ignored!\n");
        break;
    }
    }
    this->render();
}

void ScreenManager::downArrow(void)
{
    switch (this->state)
    {
    case ATTACK:
    {
        gameState.decrement_attack();
        break;
    }
    default:
    {
        printf("Up Arrow Ignored!\n");
        break;
    }
    }
    this->render();
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
        // TODO:
        // case JOIN_GAME:
        // {
        //     break;
        // }
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
            gameState.updateShip(PATROL, 0, 0, 0, 1);
            gameState.updateShip(SUBMARINE, 1, 0, 1, 2);
            gameState.updateShip(BATTLESHIP, 2, 0, 2, 3);
            gameState.updateShip(CARRIER, 3, 0, 3, 4);
            websocket.send(messenger.build_position_ships());
            gameState.state = IN_PROGRESS;
            // this->state = ATTACK;//TODO: CHANGE TO WAITING FOR RESPONSE STATE
            // this->render();
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
            websocket.send(messenger.build_attack_msg(gameState.get_attack_row() + '0', gameState.get_attack_col() + '0', SOLO, gameState.opponent));
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
            this->render();
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
