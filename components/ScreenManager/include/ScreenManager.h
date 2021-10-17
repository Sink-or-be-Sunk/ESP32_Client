#pragma once

enum SCREEN_STATE
{
    WIFI_CONNECTING,
    HOME,
    INIT_PAIRING,
    CONFIRM_PAIRING,
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
    void enter(void);
    void setState(SCREEN_STATE state);
};

extern ScreenManager screenManager;