#include "MenuScreen.hpp"
#include "Gfx.hpp"
#include "Audio.hpp"
#include "AboutScreen.hpp"
#include "DRXInfoScreen.hpp"
#include "GeneralScreen.hpp"
#include "StorageScreen.hpp"
#include "SubmitScreen.hpp"

#include <vector>

MenuScreen::MenuScreen()
 : entries({
        { MENU_ID_GENERAL, { 0xf085, "General System Information" }},
        { MENU_ID_STORAGE, { 0xf7c2, "Storage Information" }},
        { MENU_ID_DRX,     { 0xf11b, "DRC/DRH Information" }},
        { MENU_ID_SUBMIT,  { 0xf0ee, "Submit System Information" }},
        // { MENU_ID_TITLE, { 0xf022, "Title Information" }},
        { MENU_ID_ABOUT,   { 0xf05a, "About WiiUIdent" }},
        // { MENU_ID_EXIT, { 0xf057, "Exit" }},
    })
{

}

MenuScreen::~MenuScreen()
{
}

void MenuScreen::Draw()
{
    if (subscreen) {
        subscreen->Draw();
        return;
    }

    SDL_Color pulsatingColor = Gfx::GetAnimatedColor(Gfx::COLOR_BARS, { 0x27, 0x3F, 0x33, 0xFF }, 0.5f);
    
    DrawTopBar(nullptr);

    // draw entries
    for (MenuID id = MENU_ID_MIN; id <= MENU_ID_MAX; id = static_cast<MenuID>(id + 1)) {

        if (id < MENU_ID_MAX) {
            uint32_t xOff = 332 + static_cast<int>(id) * 322;

            if (id == selected) {
                Gfx::DrawRectFilled(xOff - 20, 270 - 20, 284 + 40, 284 + 40, pulsatingColor);
                Gfx::DrawRectFilled(xOff - 11, 270 - 11, 284 + 22, 284 + 22, Gfx::COLOR_ALT_ACCENT);
                if (xOff + Gfx::GetTextWidth(48,entries[id].name) > Gfx::SCREEN_WIDTH) {
                    Gfx::Print(xOff + 284 + 20, 610, 48, Gfx::COLOR_TEXT, entries[id].name, Gfx::ALIGN_VERTICAL | Gfx::ALIGN_RIGHT);
                } else { 
                    Gfx::Print(xOff - 20, 610, 48, Gfx::COLOR_TEXT, entries[id].name, Gfx::ALIGN_VERTICAL);
                }
                Gfx::DrawIcon(xOff + 284 / 2, 270 + 284 / 2, 162, Gfx::COLOR_WHITE, entries[id].icon);
            } else {
                Gfx::DrawRectFilled(xOff - 8, 270 - 8, 284 + 16, 284 + 16, {0x00, 0x00, 0x00, 0x24 });
                Gfx::DrawRectFilled(xOff - 7, 270 - 7, 284 + 14, 284 + 14, {0x00, 0x00, 0x00, 0x69 });
                Gfx::DrawRectFilled(xOff - 6, 270 - 6, 284 + 12, 284 + 12, { 0x37, 0x37, 0x37, 0xff });
                Gfx::DrawRectFilled(xOff, 270, 284, 284, { 0x30, 0x30, 0x30, 0xff });
                Gfx::DrawIcon(xOff + 284 / 2, 270 + 284 / 2, 128, Gfx::COLOR_TEXT, entries[id].icon);
            }

        } else {
    
            if (id == selected) {
                Gfx::DrawRectFilled(900 - 15, 735 - 15, 120 + 30, 120 + 30, pulsatingColor);
                Gfx::DrawRectFilled(900 - 6, 735 - 6, 120 + 12, 120 + 12, Gfx::COLOR_ALT_ACCENT);
                Gfx::Print(900 + 120 / 2, 735 + 120 + 56, 48, Gfx::COLOR_TEXT, entries[id].name, Gfx::ALIGN_VERTICAL | Gfx::ALIGN_CENTER);
                Gfx::DrawIcon(900 + 60 , 735 + 60 , 64, Gfx::COLOR_WHITE, entries[id].icon);

            
            } else {
                Gfx::DrawRectFilled(900 - 8, 735 - 8, 120 + 16, 120 + 16, {0x00, 0x00, 0x00, 0x24 });
                Gfx::DrawRectFilled(900 - 7, 735 - 7, 120 + 14, 120 + 14, {0x00, 0x00, 0x00, 0x69 });
                Gfx::DrawRectFilled(900 - 6, 735 - 6, 120 + 12, 120 + 12, { 0x37, 0x37, 0x37, 0xff });
                Gfx::DrawRectFilled(900, 735, 120, 120, { 0x30, 0x30, 0x30, 0xff });
                Gfx::DrawIcon(900 + 60 , 735 + 60 , 48, Gfx::COLOR_TEXT, entries[id].icon);
            }
        }

    }
    DrawBottomBar("\ue07d Navigate", "\ue044 Exit", "\ue000 Select");
    Gfx::Print(139, Gfx::SCREEN_HEIGHT - 100 / 2, 36, Gfx::COLOR_TEXT, "\ue07e", Gfx::ALIGN_VERTICAL); // A little hacky, but it works

}

bool MenuScreen::Update(VPADStatus& input)
{
    if (subscreen) {
        if (!subscreen->Update(input)) {
            // subscreen wants to exit
            subscreen.reset();
        }
        return true;
    }

    if (input.trigger & VPAD_BUTTON_RIGHT) {
        if (selected < MENU_ID_MAX - 1) {
            Audio::PlaySound(Audio::NAVIGATE);
            selected = static_cast<MenuID>(selected + 1);
        }
    } else if (input.trigger & VPAD_BUTTON_LEFT) {
        if (selected > MENU_ID_MIN && selected < MENU_ID_ABOUT) {
            Audio::PlaySound(Audio::NAVIGATE);
            selected = static_cast<MenuID>(selected - 1);
        }
    } else if (input.trigger & VPAD_BUTTON_DOWN) {
        if (selected < MENU_ID_MAX ) {
            gSelected = selected;
            Audio::PlaySound(Audio::NAVIGATE);
            selected = MENU_ID_ABOUT;
        }
    } else if (input.trigger & VPAD_BUTTON_UP) {
        if (selected == MENU_ID_ABOUT) {
            Audio::PlaySound(Audio::NAVIGATE);
            selected = gSelected;
        }
    }

    if (input.trigger & VPAD_BUTTON_A) {
        Audio::PlaySound(Audio::SELECT);
        switch (selected) {
        case MENU_ID_GENERAL:
            subscreen = std::make_unique<GeneralScreen>();
            break;
        case MENU_ID_STORAGE:
            subscreen = std::make_unique<StorageScreen>();
            break;
        case MENU_ID_DRX:
            subscreen = std::make_unique<DRXInfoScreen>();
            break;
        case MENU_ID_SUBMIT:
            subscreen = std::make_unique<SubmitScreen>();
            break;
        case MENU_ID_ABOUT:
            subscreen = std::make_unique<AboutScreen>();
            break;
        }
    }

    return true;
}
