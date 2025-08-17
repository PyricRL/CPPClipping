#include "windows.h"
#include <iostream>
#include <minwindef.h>

#include "settings_loader.hpp"

std::function<void()> hotkey_callback;
#define HOTKEY_ID 1

int selectedKey = get_setting<int>("hotkey_key");
UINT selectedModifiers = get_setting<UINT>("hotkey_modifier");

int register_global_hotkey(HWND hwnd, int key, UINT modifier, std::function<void()> callback) {
    hotkey_callback = callback;
    if (!RegisterHotKey(hwnd, HOTKEY_ID, modifier, key)) {
        std::cerr << "Failed to register hotkey!" << std::endl;
        return 1;
    }

    std::cout << "Success" << std::endl;

    return 0;
}

void handle_hotkey_messages(MSG& msg) {
    if (msg.message == WM_HOTKEY && msg.wParam == HOTKEY_ID) {
        if (hotkey_callback) {
            hotkey_callback();
        }
    }
}

void set_hotkey(bool& running) {
    for (int key = 0; key < 256; key++) {
        if (GetAsyncKeyState(key) & 0x8000) {
            if (key == VK_CONTROL || key == VK_SHIFT || key == VK_MENU ||
                key == VK_LMENU || key == VK_RMENU ||
                key == VK_LCONTROL || key == VK_RCONTROL ||
                key == VK_LSHIFT || key == VK_RSHIFT)
            {
                continue; // skip pure modifiers
            }

            selectedKey = key;
            selectedModifiers = 0;
            if ((GetAsyncKeyState(VK_CONTROL) & 0x8001) != 0) selectedModifiers |= MOD_CONTROL;
            if ((GetAsyncKeyState(VK_SHIFT) & 0x8001) != 0) selectedModifiers |= MOD_SHIFT;
            if ((GetAsyncKeyState(VK_MENU) & 0x8001) != 0) selectedModifiers |= MOD_ALT;

            std::cout << selectedModifiers << std::endl;
            std::cout << selectedKey << std::endl;

            if (selectedKey != 0 && selectedModifiers != 0) {
                set_settings("hotkey_key", selectedKey);
                set_settings("hotkey_modifier", selectedModifiers);
                running = false;
                break;
            }
        }
    }
}

void load_hotkey(HWND hwnd, std::function<void ()> callback) {
    int key = get_setting<int>("hotkey_key");
    UINT modifier = get_setting<UINT>("hotkey_modifier");

    register_global_hotkey(hwnd, key, modifier, callback);
}

std::string hotkey_to_string() {
    std::string result;

    UINT modifier = get_setting<UINT>("hotkey_modifier");
    int key = get_setting<int>("hotkey_key");

    if (modifier & MOD_CONTROL) result += "Ctrl+";
    if (modifier & MOD_SHIFT)   result += "Shift+";
    if (modifier & MOD_ALT)     result += "Alt+";

    // simple case for letters/numbers
    if (key >= 'A' && key <= 'Z') result += static_cast<char>(key);
    else if (key >= '0' && key <= '9') result += static_cast<char>(key);
    else if (key >= VK_F1 && key <= VK_F12) result += "F" + std::to_string(key - VK_F1 + 1);
    else result += "?"; // fallback for other keys

    return result;
}