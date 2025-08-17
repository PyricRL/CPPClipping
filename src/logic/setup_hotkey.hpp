#pragma once
#include "windows.h"
#include <iostream>
#include <minwindef.h>

#define HOTKEY_ID 1

extern int selectedKey;
extern UINT selectedModifiers;

// Callback for when hotkey is triggered
extern std::function<void()> hotkey_callback;

int register_global_hotkey(HWND hwnd, int key, UINT modifier, std::function<void()> callback);

void handle_hotkey_messages(MSG& msg);

void set_hotkey(bool& running);

void load_hotkey(HWND hwnd, std::function<void ()> callback);

std::string hotkey_to_string();