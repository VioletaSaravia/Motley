#pragma once

#ifndef RAYGUI_IMPLEMENTATION
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#endif

#include <string.h>  // Required for: strcpy()

#include "../types.h"
#include "raylib.h"
#include "tilemap.h"
#include "window.h"

typedef struct {
    Window Window;

    bool PaintTileActive;
    bool PaintFgActive;
    bool PaintBgActive;
    bool PaintRotActive;

    Color ColorPickerFgValue;
    Color ColorPickerBgValue;

} ToolbarState;

ToolbarState InitToolbar(v2 anchor) {
    ToolbarState state = {.ColorPickerFgValue = (Color){},
                          .PaintFgActive      = true,
                          .PaintRotActive     = true,
                          .PaintTileActive    = true,
                          .PaintBgActive      = true,
                          .ColorPickerBgValue = (Color){}};

    state.Window =
        (Window){.Active = true, .Anchor = anchor, .Size = (v2){136, 344}, .Title = "Toolbar"};

    return state;
}

void UpdateToolbar(ToolbarState *state) {
    if (GlobalGuiState.Editing) return;
    if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_T)) state->Window.Active = true;

    if (IsActionPressed(ACTION_PAINT_TILE)) state->PaintTileActive = !state->PaintTileActive;
    if (IsActionPressed(ACTION_PAINT_FG)) state->PaintFgActive = !state->PaintFgActive;
    if (IsActionPressed(ACTION_PAINT_BG)) state->PaintBgActive = !state->PaintBgActive;
    if (IsActionPressed(ACTION_PAINT_ROT)) state->PaintRotActive = !state->PaintRotActive;
}

void DrawToolbar(ToolbarState *state, PopupNewState *newTilemapMenuState,
                 PopupLoadState *popupLoadState, Tilemap *tilemap, TilemapCursor *cursor) {
    static const char *NewButtonText  = "#08#New";
    static const char *LoadButtonText = "#05#Load";
    static const char *PaintTileText  = "#23#";
    static const char *PaintFgText    = "#165#";
    static const char *PaintBgText    = "#168#";
    static const char *PaintRotText   = "#076#";

    if (!state->Window.Active) return;

    UpdateWindow(&state->Window);

    if (tilemap->Window.Active) {
        // FIXME: Overrides the first colors with white
        // tilemap->Palette[cursor->FG] = state->ColorPickerFgValue;
        // tilemap->Palette[cursor->BG] = state->ColorPickerBgValue;
    }

    GuiLine((Rectangle){state->Window.Anchor.x + 8, state->Window.Anchor.y + 56, 104, 16}, "");

    if (GuiButton((Rectangle){state->Window.Anchor.x + 8, state->Window.Anchor.y + 32, 56, 24},
                  NewButtonText))
        newTilemapMenuState->Window.Active = true;

    if (GuiButton((Rectangle){state->Window.Anchor.x + 72, state->Window.Anchor.y + 32, 56, 24},
                  LoadButtonText))
        popupLoadState->Window.Active = true;

    GuiToggle((Rectangle){state->Window.Anchor.x + 8, state->Window.Anchor.y + 72, 24, 24},
              PaintTileText, &state->PaintTileActive);
    GuiToggle((Rectangle){state->Window.Anchor.x + 40, state->Window.Anchor.y + 72, 24, 24},
              PaintFgText, &state->PaintFgActive);
    GuiToggle((Rectangle){state->Window.Anchor.x + 72, state->Window.Anchor.y + 72, 24, 24},
              PaintBgText, &state->PaintBgActive);
    GuiToggle((Rectangle){state->Window.Anchor.x + 104, state->Window.Anchor.y + 72, 24, 24},
              PaintRotText, &state->PaintRotActive);

    GuiColorPicker((Rectangle){state->Window.Anchor.x + 8, state->Window.Anchor.y + 240, 96, 40},
                   "", &state->ColorPickerFgValue);
    GuiColorPicker((Rectangle){state->Window.Anchor.x + 8, state->Window.Anchor.y + 296, 96, 40},
                   "", &state->ColorPickerBgValue);

    if (!tilemap->Window.Active) return;

    u32 i = 1;
    for (u32 y = 0; y < 4; y++) {
        for (u32 x = 0; x < 4; x++) {
            Rectangle r = {state->Window.Anchor.x + 8 + x * 32,
                           state->Window.Anchor.y + 104 + y * 32, 24, 24};

            DrawRectangleRec(r, tilemap->Palette[i]);

            if (cursor->FG == i) DrawRectangleLines(r.x, r.y, r.width, r.height, GREEN);
            if (cursor->BG == i) DrawRectangleLines(r.x, r.y, r.width, r.height, YELLOW);

            if (i++ >= tilemap->PaletteSize) goto END;
        }
    }
END:
}

typedef struct {
    Window Window;
    f32    Scale;
} TilesetState;

TilesetState InitTilesetWindow(v2 pos) {
    return (TilesetState){
        .Scale = 1.0f, .Window = {.Active = true, .Anchor = pos, .Title = "Tileset", .Size = {}}};
}

void UpdateTileset(TilesetState *state, const Tilemap *map) {
    if (!state->Window.Active || !map->Window.Active) return;

    state->Scale = map->tileSize.x == 24 ? 0.5f : 1.0f;

    state->Window.Size =
        (v2){map->Tileset.width * state->Scale, map->Tileset.height * state->Scale + 24};
}

void DrawTileset(TilesetState *state, const Tilemap *map, TilemapCursor *cursor,
                 Shader shaders[2]) {
    if (!state->Window.Active || !map->Window.Active) return;

    UpdateWindow(&state->Window);

    v2 tAnchor = Vector2Add(state->Window.Anchor, (v2){0, 24});

    BeginShaderMode(shaders[0]);
    DrawTextureEx(map->Tileset, tAnchor, 0.0f, state->Scale, map->Palette[cursor->FG]);
    EndShaderMode();

    BeginShaderMode(shaders[1]);
    DrawTextureEx(map->Tileset, tAnchor, 0.0f, state->Scale, map->Palette[cursor->BG]);
    EndShaderMode();

    v2i scaledTile = (v2i){map->tileSize.x * state->Scale, map->tileSize.y * state->Scale};

    DrawRectangleLines((i32)(cursor->Selected.x * scaledTile.x) + tAnchor.x,
                       (i32)(cursor->Selected.y * scaledTile.y) + tAnchor.y, scaledTile.x,
                       scaledTile.y, GREEN);

    if (CheckCollisionPointRec(GetMousePosition(),
                               (Rectangle){tAnchor.x, tAnchor.y, map->Tileset.width * state->Scale,
                                           map->Tileset.height * state->Scale})) {
        v2 cur = {GetMouseX() - tAnchor.x - GetMouseX() % scaledTile.x,
                  GetMouseY() - tAnchor.y - GetMouseY() % scaledTile.y};

        DrawRectangleLines(cur.x + tAnchor.x, cur.y + tAnchor.y, scaledTile.x, scaledTile.y, BLUE);

        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            cursor->Selected = (v2i){cur.x / scaledTile.x, cur.y / scaledTile.y};
        }
    }
}