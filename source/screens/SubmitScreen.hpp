#pragma once

#include "Screen.hpp"
#include "Utils.hpp"
#include <map>

class MessageBox;

class SubmitScreen : public Screen
{
public:
    SubmitScreen();
    virtual ~SubmitScreen();

    void Draw();

    bool Update(VPADStatus& input);

private:
    enum State {
        STATE_INFO,
        STATE_SUBMITTING,
        STATE_SUBMITTED,
    };
    State state = STATE_INFO;

    static size_t CurlWriteCallback(void* contents, size_t size, size_t nmemb, void* userp);

    void SubmitSystemData();

    std::string error;
    std::string response;

    enum MenuID {
        MENU_ID_VIEW_DATA,
        MENU_ID_SEND_DATA,
        MENU_ID_MIN = MENU_ID_VIEW_DATA,
        MENU_ID_MAX = MENU_ID_SEND_DATA,
    };

    struct MenuEntry {
        uint16_t icon;
        const char* name;
    };

    std::map<MenuID, MenuEntry> entries;
    MenuID selected = MENU_ID_MIN;

    std::unique_ptr<MessageBox> mMessageBox;
};
