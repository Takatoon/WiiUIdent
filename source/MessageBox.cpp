/*
 *   Copyright (C) 2024 GaryOderNichts
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */
#include "MessageBox.hpp"
#include "Gfx.hpp"
#include "Audio.hpp"
// #include "ThemeUtils.hpp"

MessageBox::MessageBox(const std::string& title, const std::string& message, const std::vector<Option> options)
 : mTitle(title),
   mMessage(message),
   mOptions(std::move(options)),
   mSelected(0)
{
}

MessageBox::~MessageBox()
{
}

void MessageBox::Draw()
{
    // Dim the background
    Gfx::DrawRectFilled(0, 0, Gfx::SCREEN_WIDTH, Gfx::SCREEN_WIDTH, { 0, 0, 0, 0xa0 });

    // Draw the background
    Gfx::DrawRectFilled(128, 128, Gfx::SCREEN_WIDTH - 256, Gfx::SCREEN_HEIGHT - 256, Gfx::COLOR_ALT_BACKGROUND);

    // Print message
    Gfx::Print(Gfx::SCREEN_WIDTH / 2, 128 + 44, 64, Gfx::COLOR_TEXT, mTitle, Gfx::ALIGN_HORIZONTAL | Gfx::ALIGN_TOP);
    Gfx::Print(Gfx::SCREEN_WIDTH / 2 - ( Gfx::GetTextWidth(40, mMessage) / 2) , 128 + 64 + Gfx::GetTextHeight(64, mTitle) + 8, 40, Gfx::COLOR_TEXT, mMessage);

    uint32_t xSize = Gfx::SCREEN_WIDTH - 256;
    xSize /= mOptions.size();
    uint32_t xOff = 128;
    
    Gfx::DrawRectFilled(128, Gfx::SCREEN_HEIGHT - 256, Gfx::SCREEN_WIDTH - 256, 2, Gfx::COLOR_WHITE);
    
    SDL_Color pulsatingColor = Gfx::GetAnimatedColor(Gfx::COLOR_BARS, { 0x27, 0x3F, 0x33, 0xFF }, 0.5f);

    for (size_t i = 0; i < mOptions.size(); i++) {
        if (i != mSelected && i != mOptions.size() - 1) {
            Gfx::DrawRectFilled(xOff + xSize - 2, Gfx::SCREEN_HEIGHT - 256, 2, 128, Gfx::COLOR_WHITE);
        } 

        if (i == mSelected) {
            Gfx::DrawRectFilled(xOff - 8, Gfx::SCREEN_HEIGHT - 256 - 8, xSize + 16, 128 + 16, pulsatingColor);
            Gfx::DrawRectFilled(xOff, Gfx::SCREEN_HEIGHT - 256, xSize, 128, Gfx::COLOR_ALT_ACCENT);
        }
        
        uint32_t iconWidth = 0;
        if (mOptions[i].icon) {
            iconWidth = Gfx::GetIconWidth(40, mOptions[i].icon);
        }

        uint32_t textStart = xOff + (xSize - iconWidth - Gfx::GetTextWidth(40, mOptions[i].text)) / 2;

        if (iconWidth) {
            Gfx::DrawIcon(textStart, Gfx::SCREEN_HEIGHT - 192, 40, Gfx::COLOR_TEXT, mOptions[i].icon);
        }
        Gfx::Print(textStart + iconWidth, Gfx::SCREEN_HEIGHT - 192, 40, Gfx::COLOR_TEXT, mOptions[i].text, Gfx::ALIGN_VERTICAL | Gfx::ALIGN_LEFT);

        xOff += xSize;
    }
}

bool MessageBox::Update(VPADStatus& input)
{
    if (input.trigger & VPAD_BUTTON_B) {
        Audio::PlaySound(Audio::BACK);
        return false;
    }

    if (input.trigger & VPAD_BUTTON_A) {
        Audio::PlaySound(Audio::SELECT);
        mOptions[mSelected].callback();
        return false;
    }

    if (input.trigger & VPAD_BUTTON_RIGHT) {
        if (mSelected < mOptions.size() - 1) {
            Audio::PlaySound(Audio::NAVIGATE);
            mSelected++;
        }
    } else if (input.trigger & VPAD_BUTTON_LEFT) {
        if (mSelected > 0) {
            Audio::PlaySound(Audio::NAVIGATE);
            mSelected--;
        }
    }

    return true;
}
