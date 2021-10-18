#pragma once

enum SCREEN_STATE
{
    // SYSTEM STATES
    WIFI_CONNECTING,
    HOME, // TODO: CHANGE TO WELCOME
    CONFIRM_PAIRING,
    WAITING_PAIRING,

    // SETUP
    INIT_PAIRING,
    CREATE_GAME,
    FIND_GAME,
    JOIN_GAME,

    // GAME LOBBY
    READY_UP_SHIPS, // ready up ships
    INVITE_FRIEND,

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
    void rightArrow(void);
    void leftArrow(void);
    void enter(void);
    void back(void); // cancel or back
    void setState(SCREEN_STATE state);
    void splash(SCREEN_STATE state);
};

extern ScreenManager screenManager;