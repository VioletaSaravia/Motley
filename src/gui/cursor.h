#pragma once

#ifndef RAYGUI_IMPLEMENTATION
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#endif

#include "../game.h"
#include "../types.h"
#include "raylib.h"
#include "tilemap.h"
#include "toolbar.h"

void DrawTileCursor(const Tilemap* map, const TilemapCursor* cursor, const ToolbarState* toolbar) {
    // if (!cursor->InMap || cursor->State == CURSOR_STATE_BOXING || !map->Window.Active) return;
    SetMouseCursor(cursor->InMap && cursor->State != CURSOR_STATE_BOXING ? MOUSE_CURSOR_CROSSHAIR
                                                                         : MOUSE_CURSOR_DEFAULT);

    v2 halfTile = {map->tileSize.x * 0.5f, map->tileSize.y * 0.5f};
    v2 mousePos = GetMousePosition();

    v2  selected = toolbar->PaintTileActive ? (v2){map->tileSize.x * cursor->Selected.x,
                                                   map->tileSize.y * cursor->Selected.y}
                                            : (v2){};
    f32 rotation = toolbar->PaintRotActive ? cursor->Rot * 90.0f : 0.0f;

    // TODO: I Hate this.

    // v2 coords = {mousePos.x + halfTile.x - fmodf(mousePos.x, map->tileSize.x) +
    //                  fmodf(map->Window.Anchor.x, map->tileSize.x),
    //              mousePos.y + halfTile.y - fmodf(mousePos.y, map->tileSize.y) +
    //                  fmodf(map->Window.Anchor.y, map->tileSize.y)};
    //
    // if (toolbar->PaintFgActive) {
    //     BeginShaderMode(FgCursor);
    //     DrawTexturePro(map->Tileset,
    //                    (Rectangle){selected.x, selected.y, map->tileSize.x, map->tileSize.y},
    //                    (Rectangle){coords.x, coords.y, map->tileSize.x, map->tileSize.y},
    //                    halfTile, rotation, map->Palette[cursor->FG]);
    //     EndShaderMode();
    // }
    //
    // if (toolbar->PaintBgActive) {
    //     BeginShaderMode(BgCursor);
    //     DrawTexturePro(map->Tileset,
    //                    (Rectangle){selected.x, selected.y, map->tileSize.x, map->tileSize.y},
    //                    (Rectangle){coords.x, coords.y, map->tileSize.x, map->tileSize.y},
    //                    halfTile, rotation, map->Palette[cursor->BG]);
    //     EndShaderMode();
    // }
}

void UpdateTileCursor(Tilemap* map, TilemapCursor* cursor, ToolbarState* toolbar) {
    if (!map->Window.Active || GlobalGuiState.Editing || GlobalGuiState.Moving) return;

    v2 halfTile = {map->tileSize.x * 0.5f, map->tileSize.y * 0.5f};
    v2 mousePos = GetMousePosition();

    cursor->InMap = CheckCollisionPointRec(
        mousePos, (Rectangle){map->Window.Anchor.x, map->Window.Anchor.y + 24,
                              map->Size.x * map->tileSize.x, map->Size.y * map->tileSize.y});

    v2  absPos = Vector2Subtract(mousePos, Vector2Add(map->Window.Anchor, (v2){0, 24}));
    v2i coords = {absPos.x / map->tileSize.x, absPos.y / map->tileSize.y};

    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) && cursor->InMap) {
        i32 i = (coords.y * map->Size.x + coords.x) * cursor->InMap;

        if (toolbar->PaintTileActive) map->Tile[i] = (v2){cursor->Selected.x, cursor->Selected.y};
        if (toolbar->PaintFgActive) map->FG[i] = cursor->FG;
        if (toolbar->PaintBgActive) map->BG[i] = cursor->BG;
        if (toolbar->PaintRotActive) map->Rot[i] = cursor->Rot;
    }

    // ===== PALETTE BUTTONS =====
    if (IsActionPressed(ACTION_LEFT) && cursor->Selected.x != 0) cursor->Selected.x -= 1;
    if (IsActionPressed(ACTION_RIGHT) &&
        cursor->Selected.x < map->Tileset.width / map->tileSize.x - 1)
        cursor->Selected.x += 1;
    if (IsActionPressed(ACTION_UP) && cursor->Selected.y != 0) cursor->Selected.y -= 1;
    if (IsActionPressed(ACTION_DOWN) &&
        cursor->Selected.y < map->Tileset.height / map->tileSize.y - 1)
        cursor->Selected.y += 1;
    if (IsKeyPressed(KEY_R)) cursor->Rot = (cursor->Rot + 1) % 4;

    if (IsKeyPressed(KEY_F)) {
        u8 newFg                    = (cursor->FG + 1) % map->PaletteSize;
        cursor->FG                  = newFg != cursor->BG ? newFg : (newFg + 1) % map->PaletteSize;
        toolbar->ColorPickerFgValue = map->Palette[cursor->FG];
    }
    if (IsKeyPressed(KEY_B)) {
        u8 newBg                    = (cursor->BG + 1) % map->PaletteSize;
        cursor->BG                  = newBg != cursor->FG ? newBg : (newBg + 1) % map->PaletteSize;
        toolbar->ColorPickerBgValue = map->Palette[cursor->BG];
    }
    if (IsKeyPressed(KEY_G)) {
        u8 swap    = cursor->FG;
        cursor->FG = cursor->BG;
        cursor->BG = swap;
    }
    return;

    // ===== BOX SELECT =====
    if (!cursor->InMap) return;

    if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
        cursor->State  = CURSOR_STATE_BOXING;
        cursor->Box[0] = coords;
    }

    if (cursor->State == CURSOR_STATE_BOXING) {
        cursor->Box[1] = (v2i){coords.x - cursor->Box[0].x, coords.y - cursor->Box[0].y};
    }

    if (cursor->State == CURSOR_STATE_BOXING && IsMouseButtonReleased(MOUSE_BUTTON_RIGHT)) {
        cursor->State = CURSOR_STATE_BOXED;
    }

    if (cursor->State == CURSOR_STATE_BOXED && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        cursor->State = CURSOR_STATE_SINGLE;
    }

    if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_X) && cursor->State == CURSOR_STATE_BOXED) {
        if (cursor->BoxSelection) free(cursor->BoxSelection);
        cursor->BoxSelection = MALLOC_T(v2, abs(cursor->Box[1].y * cursor->Box[1].x) + 1);

        i32 neg_x = cursor->Box[1].x < cursor->Box[0].x ? -1 : 1;
        i32 neg_y = cursor->Box[1].y < cursor->Box[0].y ? -1 : 1;
        for (u32 y = 0; y < abs(cursor->Box[1].y); y++) {
            for (u32 x = 0; x < abs(cursor->Box[1].x); x++) {
                u32 i =
                    (cursor->Box[0].y + y * neg_y) * map->Size.x + (cursor->Box[0].x + x * neg_x);

                cursor->BoxSelection[y * abs(cursor->Box[1].x) + x] = map->Tile[i];

                map->Tile[i] = (v2){};
            }
        }

        cursor->State = CURSOR_STATE_SINGLE;
    }

    if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_V) && cursor->BoxSelection &&
        cursor->State != CURSOR_STATE_BOXING) {
        i32 neg_x = cursor->Box[1].x < cursor->Box[0].x ? -1 : 1;
        i32 neg_y = cursor->Box[1].y < cursor->Box[0].y ? -1 : 1;

        for (u32 y = 0; y < abs(cursor->Box[1].y); y++) {
            for (u32 x = 0; x < abs(cursor->Box[1].x); x++) {
                map->Tile[(coords.y + cursor->Box[0].y + y * neg_y) * map->Size.x +
                          (coords.x + cursor->Box[0].x + x * neg_x)] =
                    cursor->BoxSelection[y * abs(cursor->Box[1].x) + x];
            }
        }
    }
}

void DrawBoxCursor(const Tilemap* map, const TilemapCursor* cursor) {
    if (!(cursor->State == CURSOR_STATE_BOXING) && !(cursor->State == CURSOR_STATE_BOXED)) return;

    DrawRectangleLines(
        cursor->Box[0].x * map->tileSize.x - map->tileSize.x * 0.5f + map->Window.Anchor.x,
        cursor->Box[0].y * map->tileSize.y - map->tileSize.y * 0.5f + map->Window.Anchor.y,
        cursor->Box[1].x * map->tileSize.x, cursor->Box[1].y * map->tileSize.y, GRAY);
}
