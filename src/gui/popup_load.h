#pragma once

#include "raylib.h"

#ifndef RAYGUI_IMPLEMENTATION
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#endif

#include <string.h>  // Required for: strcpy()

#include "window.h"

typedef struct {
    Window Window;

    bool TilemapPathTextBoxEditMode;
    char TilemapPathTextBoxText[256];

} PopupLoadState;

PopupLoadState InitPopupLoad() {
    PopupLoadState state = {
        .Window = {.Size = {168, 112}, .Active = false, .Title = "Load Tilemap"}};

    state.Window.Anchor = (v2){GetScreenWidth() / 2 - state.Window.Size.x / 2,
                               GetScreenHeight() / 2 - state.Window.Size.y / 2};

    return state;
}

bool DrawPopupLoad(PopupLoadState *state) {
    static const char *LoadTilemapWindowText = "Load Tilemap";
    static const char *LoadButtonText        = "Load";
    static const char *TilemapPathLabelText  = "Path";

    if (!state->Window.Active) return false;
    
    UpdateWindow(&state->Window);

    if (GuiTextBox((Rectangle){state->Window.Anchor.x + 48, state->Window.Anchor.y + 32, 104, 24},
                   state->TilemapPathTextBoxText, 128, state->TilemapPathTextBoxEditMode))
        state->TilemapPathTextBoxEditMode = !state->TilemapPathTextBoxEditMode;
    GuiLabel((Rectangle){state->Window.Anchor.x + 8, state->Window.Anchor.y + 32, 32, 24},
             TilemapPathLabelText);

    if (GuiButton((Rectangle){state->Window.Anchor.x + 24, state->Window.Anchor.y + 72, 120, 24},
                  LoadButtonText)) {
        state->Window.Active = false;
        return true;
    }

    return false;
}