#pragma once
#include "System.h"
#include "esp_log.h"
#include "Display.h"
#include "Websocket.h"
#include "Messenger.h"
#include "GameState.h"
#include "FriendManager.h"
#include "ShipManager.h"

enum SCREEN_STATE
{
    // SYSTEM STATES
    WIFI_CONNECTING,
    HOME, // TODO: CHANGE TO WELCOME
    CONFIRM_PAIRING,
    WAITING_PAIRING,
    DEVICE_PAIRED,
    REBOOT,
    MOVE_MADE,
    INVITE_SENT,
    NO_MORE_FRIENDS,
    INVALID_SHIP_LAYOUT,
    NOTIFY_POSITION_SHIPS,
    NOTIFY_INVALID_MOVE,
    GAME_OVER,
    LEFT_GAME,

    // OPPONENT EVENTS
    OPPONENT_READY_UP,
    PLAYER_IN_LOBBY,

    // SETUP
    INIT_PAIRING,
    CREATE_GAME,
    FIND_GAME,

    // GAME LOBBY
    READY_UP_SHIPS, // ready up ships
    INVITE_FRIEND,
    FRIENDS_LIST,

    // GAME IN PROGRESS
    ATTACK

#ifdef DEBUG_SHIP_POSITIONS
    ,
    SHIP_POSITION_DEBUG
#endif
};

class ScreenManager
{
private:
    SCREEN_STATE state;
    void render(void);

public:
    void init(void);
    void upArrow(void);
    void downArrow(void);
    void rightPage(void);
    void leftPage(void);
    void rightArrow(void);
    void leftArrow(void);
    void enter(void);
    void back(void); // cancel or back
    void setState(SCREEN_STATE state);
    SCREEN_STATE getState(void);
    void splash(SCREEN_STATE state);
    void splash(SCREEN_STATE splashState, SCREEN_STATE returnState);
    void conditionalRender(SCREEN_STATE state);
    void press(int num);
};

extern ScreenManager screenManager;