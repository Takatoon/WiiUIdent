#include "AboutScreen.hpp"
#include "Gfx.hpp"
#include "Audio.hpp"

AboutScreen::AboutScreen() : selectedSection(CREDITS)
{
    creditList.push_back({"Developers:", "GaryOderNichts"});
    creditList.push_back({"",            "GerbilSoft"});

    fontList.push_back({"Main Font:", "Wii U System Font"});
    fontList.push_back({"Icon Font:", "FontAwesome"});
    fontList.push_back({"Monospace Font:", "Terminus Font"});

    linkList.push_back({"GitHub:", {"github.com/GaryOderNichts/WiiUIdent", true}});
    linkList.push_back({"System Database:", {DATABASE_URL, true}});
}

AboutScreen::~AboutScreen()
{
}

void AboutScreen::Draw()
{
    // Left sidebar
    Gfx::DrawRectFilled(0, 127, 612, 851, { 0x32, 0x32, 0x32, 0xff });
        
    DrawTopBar("About");

    // Draw vertical menu
    int menuYOff = 233;
    menuYOff = DrawVerticalMenu(160, menuYOff, "Credits", 0xf121, selectedSection == CREDITS);
    menuYOff = DrawVerticalMenu(160, menuYOff, "Fonts", 0xf031, selectedSection == FONTS);
    menuYOff = DrawVerticalMenu(160, menuYOff, "Links", 0xf08e, selectedSection == LINKS);

    // Draw the selected content 
    int contentYOff = 265;
    switch (selectedSection) {
        case CREDITS:
            contentYOff = DrawList(705, contentYOff, 1076, creditList);
            break;
        case FONTS:
            contentYOff = DrawList(705, contentYOff, 1076, fontList);
            break;
        case LINKS:
            contentYOff = DrawList(705, contentYOff, 1076, linkList);
            break;
        default:
            break;
    }

    DrawBottomBar("\ue07d Navigate", "\ue044 Exit", "\ue001 Back");
}

bool AboutScreen::Update(VPADStatus& input)
{
    if (input.trigger & VPAD_BUTTON_B) {
        Audio::PlaySound(Audio::BACK);
        return false;
    }

    if (input.trigger & VPAD_BUTTON_DOWN) {
        if (selectedSection < SECTION_COUNT - 1) {
            Audio::PlaySound(Audio::NAVIGATE);
            selectedSection = static_cast<MenuSection>(selectedSection + 1);
        }
    } else if (input.trigger & VPAD_BUTTON_UP) {
        if (selectedSection > 0) {
            Audio::PlaySound(Audio::NAVIGATE);
            selectedSection = static_cast<MenuSection>(selectedSection - 1);
        }
    }

    return true;
}
