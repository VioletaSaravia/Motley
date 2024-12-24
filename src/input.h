#pragma once

#include "types.h"

typedef enum {
    ACTION_LEFT,
    ACTION_RIGHT,
    ACTION_UP,
    ACTION_DOWN,
    ACTION_ACCEPT,
    ACTION_CANCEL
} Action;

#define MAX_KEYBINDS 3

const int Keybinds[6][MAX_KEYBINDS] = {
    {KEY_A, KEY_G},
    {KEY_D},
    {KEY_W},
    {KEY_S},
    {KEY_K},
    {KEY_J},
};

bool IsActionPressed(Action action){
    for (int i = 0; i < MAX_KEYBINDS; i++)
        if (IsKeyPressed(Keybinds[action][i])) return true;
    return false;
}

bool IsActionPressedRepeat(Action action){
    for (int i = 0; i < MAX_KEYBINDS; i++)
        if (IsKeyPressedRepeat(Keybinds[action][i])) return true;
    return false;
}

bool IsActionReleased(Action action){
    for (int i = 0; i < MAX_KEYBINDS; i++)
        if (IsKeyReleased(Keybinds[action][i])) return true;
    return false;
}

bool IsActionUp(Action action){
    for (int i = 0; i < MAX_KEYBINDS; i++)
        if (IsKeyUp(Keybinds[action][i])) return true;
    return false;
}

bool IsActionDown(Action action){
    for (int i = 0; i < MAX_KEYBINDS; i++)
        if (IsKeyDown(Keybinds[action][i])) return true;
    return false;
}