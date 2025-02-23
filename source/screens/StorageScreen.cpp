#include "StorageScreen.hpp"
#include "Gfx.hpp"
#include "Audio.hpp"
#include "system/MemoryDevice.hpp"
#include "Utils.hpp"
#include <functional>

StorageScreen::StorageScreen() : selectedSection(MLC)
{
    for (const MemoryDevice& dev : MemoryDevice::GetDevices()) {
        ScreenList* list = nullptr;
        if (dev.GetType() == MemoryDevice::TYPE_MLC) {
            list = &mlcList;
        } else if (dev.GetType() == MemoryDevice::TYPE_SD_CARD) {
            list = &sdList;
        } else {
            continue;
        }

        list->push_back({"Type:", dev.GetCardType() == MemoryDevice::CARD_TYPE_SD ? "SD" : "MMC"});
        list->push_back({"Manufacturer:", Utils::sprintf("%s (0x%02x)", dev.GetManufacturerName().c_str(), dev.GetMID())});
        list->push_back({"Product Name:", {dev.GetName(), true}});
        list->push_back({"Product Revision:", Utils::sprintf("%d.%d (0x%02x)", dev.GetPRV() >> 4, dev.GetPRV() & 0xf, dev.GetPRV())});
        if (dev.GetType() == MemoryDevice::TYPE_MLC)
            list->push_back({"Production Date:", dev.GetProductionDate()});
        list->push_back({"Size:", Utils::sprintf("%llu MiB", dev.GetTotalSize() / 1024ull / 1024ull)});
        list->push_back({"CID:", {Utils::ToHexString(dev.GetCID().data(), dev.GetCID().size()), true}});
        list->push_back({"CSD:", {Utils::ToHexString(dev.GetCSD().data(), dev.GetCSD().size()), true}});
    }

    if (mlcList.empty()) {
        mlcList.push_back({"Not Attached", ""});
    }

    if (sdList.empty()) {
        sdList.push_back({"Not Attached", ""});
    }
}

StorageScreen::~StorageScreen()
{
}

void StorageScreen::Draw()
{
    // Left sidebar
    Gfx::DrawRectFilled(0, 127, 612, 851, { 0x32, 0x32, 0x32, 0xff });
    
    DrawTopBar("Storage Information");

    int menuYOff = 233;
    menuYOff = DrawVerticalMenu(160, menuYOff, "MLC", 0xf2db, selectedSection == MLC);
    menuYOff = DrawVerticalMenu(160, menuYOff, "SD", 0xf7c2, selectedSection == SD);

    int contentYOff = 255;
    switch (selectedSection) {
        case MLC:
            contentYOff = DrawList(705, contentYOff, 1076, mlcList);
            break;
        case SD:
            contentYOff = DrawList(705, contentYOff, 1076, sdList);
            break;
        default:
            break;
    }

    DrawBottomBar("\ue07d Navigate", "\ue044 Exit", "\ue001 Back");
}

bool StorageScreen::Update(VPADStatus& input)
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
