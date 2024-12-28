#pragma once

#ifndef RAYGUI_IMPLEMENTATION
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#endif

#include "../types.h"
#include "raylib.h"

typedef struct {
    v2          Anchor;
    v2          Size;
    v2          TitlebarSize;
    bool        Active;
    bool        Moving;
    const char* Title;
} Window;

void UpdateWindow(Window* state) {
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) &&
        IsPointInRectangle(GetMousePosition(),
                           (Rectangle){state->Anchor.x, state->Anchor.y, state->TitlebarSize.x,
                                       state->TitlebarSize.y})) {
        state->Moving = true;
    }

    if (state->Moving) {
        state->Anchor = Vector2Add(state->Anchor, GetMouseDelta());
    }

    if ((IsMouseButtonReleased(MOUSE_BUTTON_LEFT) && state->Moving) ||
        IsMouseButtonUp(MOUSE_BUTTON_LEFT)) {
        state->Moving = false;
    }

    state->Active = !GuiWindowBox(
        (Rectangle){state->Anchor.x, state->Anchor.y, state->Size.x, state->Size.y}, state->Title);
}