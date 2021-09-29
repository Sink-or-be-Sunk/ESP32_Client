#pragma once

enum SCREEN_STATE
{
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
};

extern ScreenManager screenManager;