#pragma once
#include "Display.h"
#include "Websocket.h"
#include "Messenger.h"
#include "GameState.h"
#include "FriendManager.h"

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
};

extern ScreenManager screenManager;