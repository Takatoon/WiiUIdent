#pragma once

#include "Screen.hpp"

class DRXInfoScreen : public Screen
{
public:
    DRXInfoScreen();
    virtual ~DRXInfoScreen();

    void Draw();

    bool Update(VPADStatus& input);

private:
    ScreenList mDRCList;
    ScreenList mExtIdList;

    ScreenList mDRHList;

    enum MenuSection {
        DRC_INFO,
        DRC_EXT,
        DRH_INFO,
        SECTION_COUNT
    };

    MenuSection selectedSection;
};
