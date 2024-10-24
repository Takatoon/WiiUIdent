#include "Screen.hpp"
#include "Gfx.hpp"

void Screen::DrawTopBar(const char* name)
{
    // draw top bar
    Gfx::DrawRectFilled(0, 0, Gfx::SCREEN_WIDTH, 127, Gfx::COLOR_BARS);

    // draw top bar content
    Gfx::DrawIcon(139, 74, 48, Gfx::COLOR_WHITE, 0xf002, Gfx::ALIGN_CENTER);
    Gfx::Print(187 + 8, 74, 48, Gfx::COLOR_WHITE, "WiiUIdent", Gfx::ALIGN_VERTICAL);
    Gfx::Print(Gfx::GetTextWidth(60, "WiiUIdent") + 139 + 16, 77, 40, Gfx::COLOR_ALT_TEXT, "v" APP_VERSION, Gfx::ALIGN_VERTICAL);
    if (name)
        Gfx::Print(Gfx::SCREEN_WIDTH - 139, 74, 48, Gfx::COLOR_ALT_TEXT, name, Gfx::ALIGN_VERTICAL | Gfx::ALIGN_RIGHT);
    
    // draw topbar shadow
    Gfx::DrawRectFilled(0, 127, Gfx::SCREEN_WIDTH, 1, { 0x00, 0x00, 0x00, 0x69 }); 
    Gfx::DrawRectFilled(0, 128, Gfx::SCREEN_WIDTH, 1, { 0x00, 0x00, 0x00, 0x65 }); 
    Gfx::DrawRectFilled(0, 129, Gfx::SCREEN_WIDTH, 1, { 0x00, 0x00, 0x00, 0x5E }); 
    Gfx::DrawRectFilled(0, 130, Gfx::SCREEN_WIDTH, 1, { 0x00, 0x00, 0x00, 0x55 }); 
    Gfx::DrawRectFilled(0, 131, Gfx::SCREEN_WIDTH, 1, { 0x00, 0x00, 0x00, 0x4E }); 
    Gfx::DrawRectFilled(0, 132, Gfx::SCREEN_WIDTH, 1, { 0x00, 0x00, 0x00, 0x47 }); 
    Gfx::DrawRectFilled(0, 133, Gfx::SCREEN_WIDTH, 1, { 0x00, 0x00, 0x00, 0x3B }); 
    Gfx::DrawRectFilled(0, 134, Gfx::SCREEN_WIDTH, 1, { 0x00, 0x00, 0x00, 0x33 }); 
    Gfx::DrawRectFilled(0, 135, Gfx::SCREEN_WIDTH, 1, { 0x00, 0x00, 0x00, 0x2B }); 
    Gfx::DrawRectFilled(0, 136, Gfx::SCREEN_WIDTH, 1, { 0x00, 0x00, 0x00, 0x23 }); 
    Gfx::DrawRectFilled(0, 137, Gfx::SCREEN_WIDTH, 1, { 0x00, 0x00, 0x00, 0x1B }); 
    Gfx::DrawRectFilled(0, 138, Gfx::SCREEN_WIDTH, 1, { 0x00, 0x00, 0x00, 0x11 }); 
    Gfx::DrawRectFilled(0, 139, Gfx::SCREEN_WIDTH, 1, { 0x00, 0x00, 0x00, 0x0C });

}

void Screen::DrawBottomBar(const char* leftHint, const char* centerHint, const char* rightHint)
{
    // draw bottom separator
    Gfx::DrawRectFilled(139, Gfx::SCREEN_HEIGHT - 102, Gfx::SCREEN_WIDTH - 278, 3, {0xd4, 0xd4, 0xd4, 0xff});
    Gfx::DrawRectFilled(140, Gfx::SCREEN_HEIGHT - 101, Gfx::SCREEN_WIDTH - 280, 1, {0xf6, 0xf6, 0xf6, 0xff});

    // draw bottom bar content
    if (leftHint)
        Gfx::Print(139, Gfx::SCREEN_HEIGHT - 100 / 2, 36, Gfx::COLOR_TEXT, leftHint, Gfx::ALIGN_VERTICAL);
    if (centerHint)
        Gfx::Print(Gfx::SCREEN_WIDTH / 2, Gfx::SCREEN_HEIGHT - 100 / 2, 36, Gfx::COLOR_TEXT, centerHint, Gfx::ALIGN_CENTER);
    if (rightHint)
        Gfx::Print(Gfx::SCREEN_WIDTH - 139, Gfx::SCREEN_HEIGHT - 100 / 2, 36, Gfx::COLOR_TEXT, rightHint, Gfx::ALIGN_VERTICAL | Gfx::ALIGN_RIGHT);
}

int Screen::DrawHeader(int x, int y, int w, uint16_t icon, const char* text)
{
    const int iconWidth = Gfx::GetIconWidth(50, icon);
    const int width = iconWidth + 32 + Gfx::GetTextWidth(50, text);
    const int xStart = x + (w / 2) - (width / 2);

    Gfx::DrawIcon(xStart, y, 50, Gfx::COLOR_TEXT, icon, Gfx::ALIGN_VERTICAL);
    Gfx::Print(xStart + iconWidth + 32, y, 50, Gfx::COLOR_TEXT, text, Gfx::ALIGN_VERTICAL);
    Gfx::DrawRectFilled(x, y + 32, w, 4, Gfx::COLOR_ACCENT);

    return y + 64;
}

int Screen::DrawList(int x, int y, int w, ScreenList items)
{
    int yOff = y;
    for (auto it = items.begin(); it != items.end(); ++it) {
        const auto& item = *it;
        Gfx::Print(x, yOff, 40, Gfx::COLOR_ALT_TEXT, item.first, Gfx::ALIGN_VERTICAL);
        Gfx::Print(x + w, yOff, 40, Gfx::COLOR_ALT_TEXT, item.second.string, Gfx::ALIGN_VERTICAL | Gfx::ALIGN_RIGHT, item.second.monospace);
        yOff += std::max(Gfx::GetTextHeight(40, item.first), Gfx::GetTextHeight(40, item.second.string, item.second.monospace));
        
        auto nextIt = std::next(it);
        if (nextIt != items.end() && !nextIt->first.empty()) {
            Gfx::DrawRectFilled(x, yOff - 10, w, 2, { 0x03f, 0x03f, 0x03f, 0xff });
        }
        yOff += 40;
    }

    return yOff + 40;
}

int Screen::DrawVerticalMenu(int x, int y, const char* text, int icon, bool selected)
{
    SDL_Color color = selected ? Gfx::COLOR_TEXT : Gfx::COLOR_ALT_TEXT;
    
    if (selected) {
        Gfx::DrawRectFilled(x - 38 , y - 42, 466, 122, Gfx::COLOR_BARS);
        Gfx::DrawRectFilled(x - 38 + 9 , y - 42 + 9, 466 - 18, 122 - 18, Gfx::COLOR_ALT_BACKGROUND);
        Gfx::DrawRectFilled(x - 38 + 20 , y - 20, 6, 77, Gfx::COLOR_BARS);
    } 

    const int iconWidth = Gfx::GetIconWidth(40, icon) + 10;

    Gfx::DrawIcon(x, y + 3, 40, color, icon, Gfx::ALIGN_TOP);
    Gfx::Print(x + iconWidth, y - 5, 40, color, text, Gfx::ALIGN_TOP);

    return y + 122; 
}
