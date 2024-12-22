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

typedef struct Keybind Keybind;
struct Keybind {
    int Key;
    Keybind* Next;
};

const Keybind Keybinds[6] = {
    {KEY_A, &(Keybind){KEY_G, NULL}},
    {KEY_D, NULL},
    {KEY_W, NULL},
    {KEY_S, NULL},
    {KEY_K, NULL},
    {KEY_J, NULL},
};

bool IsActionPressed(Action action){
    for (Keybind const* next = &Keybinds[action]; next != NULL; next = next->Next) {
        if (IsKeyPressed(next->Key)) return true;
    }
    return false;
}

bool IsActionPressedRepeat(Action action){
    for (Keybind const* next = &Keybinds[action]; next != NULL; next = next->Next) {
        if (IsKeyPressedRepeat(next->Key)) return true;
    }
    return false;
}

bool IsActionReleased(Action action){
    for (Keybind const* next = &Keybinds[action]; next != NULL; next = next->Next) {
        if (IsKeyReleased(next->Key)) return true;
    }
    return false;
}

bool IsActionUp(Action action){
    for (Keybind const* next = &Keybinds[action]; next != NULL; next = next->Next) {
        if (IsKeyUp(next->Key)) return true;
    }
    return false;
}

bool IsActionDown(Action action){
    for (Keybind const* next = &Keybinds[action]; next != NULL; next = next->Next) {
        if (IsKeyDown(next->Key)) return true;
    }
    return false;
}