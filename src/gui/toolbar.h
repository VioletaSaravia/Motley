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
    // v2 anchor;
    //  bool ToolbarWindowActive;

    Window Window;

    bool PaintTileActive;
    bool PaintFgActive;
    bool PaintBgActive;
    bool PaintRotActive;

    Color ColorPickerFgValue;
    Color ColorPickerBgValue;

} ToolbarState;

ToolbarState InitToolbar(v2 anchor) {
    // TODO ZII ?
    ToolbarState state = {.ColorPickerFgValue = (Color){},
                          .PaintFgActive      = true,
                          .PaintRotActive     = true,
                          .PaintTileActive    = true,
                          .PaintBgActive      = true,
                          .ColorPickerBgValue = (Color){}};

    state.Window = (Window){.Active       = true,
                            .Anchor       = anchor,
                            .TitlebarSize = (v2){136, 24},
                            .Size         = (v2){136, 344},
                            .Title        = "Toolbar"};

    return state;
}

void UpdateToolbar(ToolbarState *state) {
    if (IsActionPressed(ACTION_PAINT_TILE)) state->PaintTileActive = !state->PaintTileActive;
    if (IsActionPressed(ACTION_PAINT_FG)) state->PaintFgActive = !state->PaintFgActive;
    if (IsActionPressed(ACTION_PAINT_BG)) state->PaintBgActive = !state->PaintBgActive;
    if (IsActionPressed(ACTION_PAINT_ROT)) state->PaintRotActive = !state->PaintRotActive;
}

static void NewButton(NewTilemapMenuState *state) { state->NewTilemapWindowActive = true; }
static void LoadButton() { printf_s("Not implemented\n"); }

void Toolbar(ToolbarState *state, NewTilemapMenuState *newTilemapMenuState, Tilemap *tilemap,
             TilemapCursor *cursor) {
    static const char *ToolbarWindowText = "Toolbar";
    static const char *NewButtonText     = "#08#New";
    static const char *LoadButtonText    = "#05#Load";
    static const char *PaintTileText     = "#23#";
    static const char *PaintFgText       = "#165#";
    static const char *PaintBgText       = "#168#";
    static const char *PaintRotText      = "#076#";

    if (!state->Window.Active) return;

    UpdateWindow(&state->Window);

    if (tilemap) {
        state->ColorPickerFgValue = tilemap->Palette[cursor->FG];
        state->ColorPickerBgValue = tilemap->Palette[cursor->BG];

        // TODO: Color setters
    }

    GuiLine((Rectangle){state->Window.Anchor.x + 8, state->Window.Anchor.y + 56, 104, 16}, "");

    if (GuiButton((Rectangle){state->Window.Anchor.x + 8, state->Window.Anchor.y + 32, 56, 24},
                  NewButtonText))
        NewButton(newTilemapMenuState);
    if (GuiButton((Rectangle){state->Window.Anchor.x + 72, state->Window.Anchor.y + 32, 56, 24},
                  LoadButtonText))
        LoadButton();

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

    if (!tilemap) return;

    u32 i = 0;
    for (u32 y = 0; y < 4; y++) {
        for (u32 x = 0; x < 4; x++) {
            Rectangle r = {state->Window.Anchor.x + 8 + x * 32,
                           state->Window.Anchor.y + 104 + y * 32, 24, 24};

            DrawRectangleRec(r, tilemap->Palette[i]);

            if (cursor->FG == i) DrawRectangleLines(r.x, r.y, r.width, r.height, GREEN);
            if (cursor->BG == i) DrawRectangleLines(r.x, r.y, r.width, r.height, YELLOW);

            if (i++ >= tilemap->PaletteSize) break;
        }
    }
}

typedef struct {
    Window Window;
    f32    Scale;
} TilesetState;

TilesetState NewTilesetWindow(v2 pos) {
    return (TilesetState){
        .Scale  = 1.0f / 3.0f,
        .Window = {
            .Active = true, .Anchor = pos, .Title = "Tileset", .Size = {}, .TitlebarSize = {}}};
}

void DrawTileset(TilesetState *state, const Tilemap *map, TilemapCursor *cursor) {
    state->Window.TitlebarSize = (v2){map->Tileset.width * state->Scale, 24};
    state->Window.Size =
        (v2){map->Tileset.width * state->Scale, map->Tileset.height * state->Scale + 24};
    UpdateWindow(&state->Window);

    v2 tAnchor = Vector2Add(state->Window.Anchor, (v2){0, 24});

    BeginShaderMode(FgTilemap);
    DrawTextureEx(map->Tileset, tAnchor, 0.0f, state->Scale, map->Palette[cursor->FG]);
    EndShaderMode();

    BeginShaderMode(BgTilemap);
    DrawTextureEx(map->Tileset, tAnchor, 0.0f, state->Scale, map->Palette[cursor->BG]);
    EndShaderMode();

    v2i scaledTile = (v2i){map->tileSize.x * state->Scale, map->tileSize.y * state->Scale};

    DrawRectangleLines((i32)(cursor->Selected.x * scaledTile.x) + tAnchor.x,
                       (i32)(cursor->Selected.y * scaledTile.y) + tAnchor.y, scaledTile.x,
                       scaledTile.y, GREEN);

    if (IsPointInRectangle(GetMousePosition(),
                           (Rectangle){tAnchor.x, tAnchor.y, map->Tileset.width * state->Scale,
                                       map->Tileset.height * state->Scale})) {
        v2 cur = {GetMouseX() - tAnchor.x - GetMouseX() % scaledTile.x,
                  GetMouseY() - tAnchor.y - GetMouseY() % scaledTile.y};

        DrawRectangleLines(cur.x + tAnchor.x, cur.y + tAnchor.y, scaledTile.x, scaledTile.y, BLUE);

        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            cursor->Selected = (v2){(i32)(cur.x / scaledTile.x), (i32)(cur.y / scaledTile.y)};
        }
    }
}