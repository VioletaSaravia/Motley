#pragma once

#ifndef RAYGUI_IMPLEMENTATION
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#endif

#include "../types.h"
#include "raylib.h"

static struct {
    bool Editing;
    bool Moving;
} GlobalGuiState = {};

typedef struct {
    v2          Anchor;
    v2          Size;
    bool        Active;
    bool        Moving;
    const char* Title;
} Window;

Window InitWindowBox(const char* title, v2 size) {
    return (Window){.Active = true,
                    .Anchor = (v2){GetScreenWidth() * 0.5f - size.x * 0.5f,
                                   GetScreenHeight() * 0.5f - size.y * 0.5f},
                    .Size   = size,
                    .Title  = title};
}

void UpdateWindow(Window* state) {
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) &&
       CheckCollisionPointRec(GetMousePosition(),
                           (Rectangle){state->Anchor.x, state->Anchor.y, state->Size.x, RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT})) {
        state->Moving         = true;
        GlobalGuiState.Moving = true;
    }

    if (state->Moving) {
        state->Anchor = Vector2Add(state->Anchor, GetMouseDelta());
    }

    if ((IsMouseButtonReleased(MOUSE_BUTTON_LEFT) && state->Moving) ||
        IsMouseButtonUp(MOUSE_BUTTON_LEFT)) {
        state->Moving         = false;
        GlobalGuiState.Moving = false;
    }

    bool xPressed = GuiWindowBox(
        (Rectangle){state->Anchor.x, state->Anchor.y, state->Size.x, state->Size.y}, state->Title);

    if (xPressed && state->Active) {
        state->Active          = false;
        GlobalGuiState.Editing = false;
    }
}
