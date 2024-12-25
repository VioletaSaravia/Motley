#pragma once

#ifdef _WIN32
#ifdef INPUT_EXPORTS
#define INPUT_API __declspec(dllexport)
#else
#define INPUT_API
#endif
#else
#define INPUT_API
#endif

#include "types.h"

typedef enum {
    ACTION_NULL,
    ACTION_LEFT,
    ACTION_RIGHT,
    ACTION_UP,
    ACTION_DOWN,
    ACTION_ACCEPT,
    ACTION_CANCEL,
    ACTION_CAM_ROT_LEFT,
    ACTION_CAM_ROT_RIGHT,
    ACTION_CAM_ROT_UP,
    ACTION_CAM_ROT_DOWN,
    ACTION_COUNT
} Action;

#define MAX_KEYBINDS 3

const int Keybinds[ACTION_COUNT][MAX_KEYBINDS] = {
    {},
    {KEY_A, KEY_LEFT},
    {KEY_D, KEY_RIGHT},
    {KEY_W, KEY_UP},
    {KEY_S, KEY_DOWN},
    {KEY_C},
    {KEY_V},
    {KEY_J},
    {KEY_L},
    {KEY_I},
    {KEY_K},
};

INPUT_API bool IsActionPressed(Action action) {
    for (int i = 0; i < MAX_KEYBINDS; i++)
        if (IsKeyPressed(Keybinds[action][i]))
            return true;
    return false;
}

INPUT_API bool IsActionPressedRepeat(Action action) {
    for (int i = 0; i < MAX_KEYBINDS; i++)
        if (IsKeyPressedRepeat(Keybinds[action][i]))
            return true;
    return false;
}

INPUT_API bool IsActionReleased(Action action) {
    for (int i = 0; i < MAX_KEYBINDS; i++)
        if (IsKeyReleased(Keybinds[action][i]))
            return true;
    return false;
}

INPUT_API bool IsActionUp(Action action) {
    for (int i = 0; i < MAX_KEYBINDS; i++)
        if (IsKeyUp(Keybinds[action][i]))
            return true;
    return false;
}

INPUT_API bool IsActionDown(Action action) {
    for (int i = 0; i < MAX_KEYBINDS; i++)
        if (IsKeyDown(Keybinds[action][i]))
            return true;
    return false;
}