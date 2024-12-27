#pragma once

#include <string.h>  // Required for: strcpy()

#include "../tilemap.h"
#include "../types.h"
#include "raylib.h"

typedef struct {
    Vector2 anchor01;

    bool createTilemap;

    bool NewTilemapWindowActive;
    bool TilesetPathEditMode;
    char TilesetPathText[256];
    bool tileSizeXEditMode;
    i32  tileSizeXValue;
    bool tileSizeYEditMode;
    i32  tileSizeYValue;
    bool PalettePathEditMode;
    char PalettePathText[256];
    bool mapSizeXEditMode;
    i32  mapSizeXValue;
    bool mapSizeYEditMode;
    i32  mapSizeYValue;

} NewTilemapMenuState;

NewTilemapMenuState InitNewTilemapMenu() {
    NewTilemapMenuState state = {0};

    state.anchor01 = (Vector2){GetScreenWidth() * 0.5f - 140, GetScreenHeight() * 0.5f - 76};

    state.NewTilemapWindowActive = true;
    state.TilesetPathEditMode    = false;
    strcpy(state.TilesetPathText, "assets/mrmox3.png");
    state.tileSizeXEditMode   = false;
    state.tileSizeXValue      = 24;
    state.tileSizeYEditMode   = false;
    state.tileSizeYValue      = 24;
    state.PalettePathEditMode = false;
    strcpy(state.PalettePathText, "assets/dreamscape8.hex");
    state.mapSizeXEditMode = false;
    state.mapSizeXValue    = 20;
    state.mapSizeYEditMode = false;
    state.mapSizeYValue    = 15;

    return state;
}
static void CreateTilemapButton(NewTilemapMenuState *state) { state->createTilemap = true; }

Tilemap StartTilemapEditor(NewTilemapMenuState *state) {
    state->createTilemap          = false;
    state->NewTilemapWindowActive = false;

    return NewTilemap(state->TilesetPathText, state->PalettePathText,
                      (v2i){state->tileSizeXValue, state->tileSizeYValue},
                      (v2i){state->mapSizeXValue, state->mapSizeYValue});
}

void NewTilemapMenu(NewTilemapMenuState *state) {
    const char *NewTilemapWindowText    = "New Tilemap";
    const char *tileSizeLabelText       = "Tile Size";
    const char *mapSizeLabelText        = "Map Size";
    const char *CreateTilemapButtonText = "Create Tilemap";

    if (state->NewTilemapWindowActive) {
        state->NewTilemapWindowActive =
            !GuiWindowBox((Rectangle){state->anchor01.x + 0, state->anchor01.y + 0, 280, 152},
                          NewTilemapWindowText);
        if (GuiTextBox((Rectangle){state->anchor01.x + 8, state->anchor01.y + 32, 120, 24},
                       state->TilesetPathText, 128, state->TilesetPathEditMode))
            state->TilesetPathEditMode = !state->TilesetPathEditMode;
        if (GuiValueBox((Rectangle){state->anchor01.x + 192, state->anchor01.y + 32, 32, 24}, "",
                        &state->tileSizeXValue, 0, 100, state->tileSizeXEditMode))
            state->tileSizeXEditMode = !state->tileSizeXEditMode;
        if (GuiValueBox((Rectangle){state->anchor01.x + 232, state->anchor01.y + 32, 32, 24}, "",
                        &state->tileSizeYValue, 0, 100, state->tileSizeYEditMode))
            state->tileSizeYEditMode = !state->tileSizeYEditMode;
        if (GuiTextBox((Rectangle){state->anchor01.x + 8, state->anchor01.y + 72, 120, 24},
                       state->PalettePathText, 128, state->PalettePathEditMode))
            state->PalettePathEditMode = !state->PalettePathEditMode;
        if (GuiValueBox((Rectangle){state->anchor01.x + 192, state->anchor01.y + 72, 32, 24}, "",
                        &state->mapSizeXValue, 0, 100, state->mapSizeXEditMode))
            state->mapSizeXEditMode = !state->mapSizeXEditMode;
        if (GuiValueBox((Rectangle){state->anchor01.x + 232, state->anchor01.y + 72, 32, 24}, "",
                        &state->mapSizeYValue, 0, 100, state->mapSizeYEditMode))
            state->mapSizeYEditMode = !state->mapSizeYEditMode;
        GuiLabel((Rectangle){state->anchor01.x + 136, state->anchor01.y + 32, 48, 24},
                 tileSizeLabelText);
        GuiLabel((Rectangle){state->anchor01.x + 136, state->anchor01.y + 72, 56, 24},
                 mapSizeLabelText);
        if (GuiButton((Rectangle){state->anchor01.x + 72, state->anchor01.y + 112, 120, 24},
                      CreateTilemapButtonText))
            CreateTilemapButton(state);
    }
}