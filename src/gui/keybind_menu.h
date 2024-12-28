#pragma once

#include <string.h>  // Required for: strcpy()

#ifndef RAYGUI_IMPLEMENTATION
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#endif

#include "game.h"
#include "raylib.h"

typedef struct {
    Vector2 anchor01;

    Rectangle ScrollPanelScrollView;
    Vector2   ScrollPanelScrollOffset;
    Vector2   ScrollPanelBoundsOffset;
    bool      RebindButtons[ACTION_COUNT * MAX_KEYBINDS];

    Rectangle layoutRecs[4];
} KeybindMenuState;

KeybindMenuState NewKeybindMenu() {
    KeybindMenuState state = {};

    state.layoutRecs[0] = (Rectangle){state.anchor01.x + 0, state.anchor01.y + 0, 264, 264};
    state.layoutRecs[1] = (Rectangle){state.anchor01.x + 32, state.anchor01.y + 24, 120, 24};
    state.layoutRecs[2] = (Rectangle){state.anchor01.x + 104, state.anchor01.y + 24, 48, 24};

    return state;
}

void KeybindMenu(KeybindMenuState *state) {
    GuiScrollPanel((Rectangle){state->layoutRecs[0].x, state->layoutRecs[0].y,
                               state->layoutRecs[0].width - state->ScrollPanelBoundsOffset.x,
                               state->layoutRecs[0].height - state->ScrollPanelBoundsOffset.y},
                   "", state->layoutRecs[0], &state->ScrollPanelScrollOffset,
                   &state->ScrollPanelScrollView);

    char buf[8];
    for (u32 i = 0; i < ACTION_COUNT * MAX_KEYBINDS; i += MAX_KEYBINDS) {
        sprintf(buf, "%d", i);

        GuiLabel((Rectangle){.x = 0, .y = 0 + i * 24, .width = 48, .height = 24}, buf);

        state->RebindButtons[i]     = GuiButton((Rectangle){.x      = state->layoutRecs[2].x,
                                                            .y      = state->layoutRecs[2].y + i * 8,
                                                            .width  = 48,
                                                            .height = 24},
                                                buf);
        state->RebindButtons[i + 1] = GuiButton((Rectangle){.x     = state->layoutRecs[2].x + 24,
                                                            .y     = state->layoutRecs[2].y + i * 8,
                                                            .width = 48,
                                                            .height = 24},
                                                buf);
        state->RebindButtons[i + 2] = GuiButton((Rectangle){.x     = state->layoutRecs[2].x + 48,
                                                            .y     = state->layoutRecs[2].y + i * 8,
                                                            .width = 48,
                                                            .height = 24},
                                                buf);
        state->RebindButtons[i + 3] = GuiButton((Rectangle){.x     = state->layoutRecs[2].x + 72,
                                                            .y     = state->layoutRecs[2].y + i * 8,
                                                            .width = 48,
                                                            .height = 24},
                                                buf);
    }
}

void RebindMenu(KeybindMenuState *state) {
    for (u32 i = 0; i < ACTION_COUNT * MAX_KEYBINDS; i++) {
        if (state->RebindButtons[i]) {
            Keys[(u32)floorf(i / 4)][i % 4] = (Keybind){INPUT_TYPE_Keyboard, 0};
        }
    }
}